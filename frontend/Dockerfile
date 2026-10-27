# ---- Stage 1: build the React app ----
FROM node:22-alpine AS build

WORKDIR /app

# Copy manifests first so the dependency layer is cached
COPY package.json package-lock.json ./

# Clean, reproducible install (matches your lockfile exactly)
RUN npm ci

# Now copy the rest of the source
COPY . .

# Type-check and build (matches your "build" script: tsc -b && vite build)
RUN npm run build

# ---- Stage 2: serve the static build with nginx ----
FROM nginx:1.27-alpine

# Replace the default site with our proxy config
COPY nginx.conf /etc/nginx/conf.d/default.conf

# Copy the built site from the build stage (Vite writes to dist/)
COPY --from=build /app/dist /usr/share/nginx/html

EXPOSE 80

CMD ["nginx", "-g", "daemon off;"]