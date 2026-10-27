# ---- Stage 1: build the C++ backend ----
FROM ubuntu:22.04 AS build

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
        build-essential \
        cmake \
        git \
        ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src

# Copy source (everything except frontend, build, docs — see .dockerignore)
COPY . .

# Configure and build. GoogleTest is fetched by FetchContent (needs internet).
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build --target tividy --parallel

# ---- Stage 2: minimal runtime ----
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
        libstdc++6 \
        ca-certificates \
        curl \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy the built executable from the build stage
COPY --from=build /src/build/tividy /app/tividy

EXPOSE 8080

# Healthcheck — RestServer.cpp exposes GET /health returning "OK"
HEALTHCHECK --interval=10s --timeout=3s --start-period=5s --retries=3 \
    CMD curl -fsS http://localhost:8080/health || exit 1

# Run the server on 0.0.0.0:8080 so it's reachable from outside the container
CMD ["/app/tividy", "8080", "0.0.0.0"]