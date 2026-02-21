# Use official Ubuntu image
FROM ubuntu:22.04

# Install g++ and curl
RUN apt-get update && apt-get install -y \
    g++ \
    curl \
    && rm -rf /var/lib/apt/lists/*

# Create logs folder
RUN mkdir /logs

# Set working directory
WORKDIR /app

# Copy source files into container
COPY monitor.cpp .
COPY json.hpp .
COPY start.sh .

# Make start.sh executable
RUN chmod +x start.sh

# Compile monitor
RUN g++ monitor.cpp -o monitor -pthread -std=c++17

# Default command
CMD ["./start.sh"]
