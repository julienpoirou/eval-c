################
# Stage: Build #
################
FROM gcc:15.2.0 AS build

# Application directory
WORKDIR /app

# Copy the application source files
COPY Makefile ./Makefile
COPY src ./src/

# Application environment (default: development)
ARG APP_ENVIRONMENT=DEVELOPMENT
ENV APP_ENVIRONMENT=${APP_ENVIRONMENT}

# Build application
RUN make

##############
# Stage: Run #
##############
FROM debian:trixie

# Application directory
WORKDIR /app

# OCI (Open Container Initiative) standard
LABEL org.opencontainers.image.title="T3C" \
      org.opencontainers.image.description="Tool for Chain-Condensate Correspondence" \
      org.opencontainers.image.documentation="https://github.com/julienpoirou/eval-c/blob/main/README.md" \
      org.opencontainers.image.version="v0.1.0" \
      org.opencontainers.image.authors="Julien Poirou <julienpoirou@protonmail.com>" \
      org.opencontainers.image.source="https://github.com/julienpoirou/eval-c"

# Create a no-root user
RUN mkdir -p /app/output \
    && groupadd -r -g 1000 app \
    && useradd -r -u 1000 -g 1000 -s /usr/sbin/nologin -d /app app \
    && chown app:app /app/output

# Get the compiled version of the application
COPY --from=build --chown=app:app --chmod=0755 /app/bin/t3c /app/bin/t3c

# No-root user
USER app:app

# Run the application
ENTRYPOINT ["/app/bin/t3c"]
