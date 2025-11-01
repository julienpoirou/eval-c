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

#################
# Stage: Export #
#################
FROM scratch AS export

# Get the compiled version of the application
COPY --from=build --chmod=0755 /app/bin/t3c /bin/t3c
