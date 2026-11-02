# ---- Stage 1: build ----
FROM ubuntu:22.04 AS build

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
 && apt-get install -y --no-install-recommends build-essential cmake ca-certificates \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY CMakeLists.txt ./
COPY app ./app
COPY member1_core ./member1_core
COPY member2_execution ./member2_execution
COPY member3_routing ./member3_routing
COPY member4_services ./member4_services
COPY member5_education ./member5_education
COPY member6_hospital ./member6_hospital
COPY member7_integration ./member7_integration

RUN mkdir -p tests \
 && for f in $(grep -oE 'tests/[A-Za-z0-9_]+\.cpp' CMakeLists.txt | sort -u); do : > "$f"; done

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DTIVIDY_BUILD_TESTS=OFF \
 && cmake --build build --target tividy --parallel "$(nproc)"

# ---- Stage 2: minimal runtime ----
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
 && apt-get install -y --no-install-recommends libstdc++6 curl ca-certificates \
 && rm -rf /var/lib/apt/lists/* \
 && useradd --system --no-create-home --uid 10001 tividy

WORKDIR /app
COPY --from=build /src/build/tividy /app/tividy
USER tividy

EXPOSE 8080

# RestServer exposes GET /health -> "OK"
HEALTHCHECK --interval=10s --timeout=3s --start-period=5s --retries=3 \
    CMD curl -fsS http://localhost:8080/health || exit 1

# usage: tividy [port] [host]   (0.0.0.0 so the container is reachable)
CMD ["/app/tividy", "8080", "0.0.0.0"]