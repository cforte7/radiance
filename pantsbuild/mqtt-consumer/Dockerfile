# mqtt_consumer/Dockerfile
FROM golang:1.23.3 as builder

# Set working directory
WORKDIR /app

# Copy and build the application
COPY . .
RUN go mod download && go build -o mqtt_consumer .

# Create a lightweight container
FROM debian:bullseye-slim

# Copy the compiled binary
COPY --from=builder /app/mqtt_consumer /usr/local/bin/mqtt_consumer

# Set the entry point
ENTRYPOINT ["mqtt_consumer"]
