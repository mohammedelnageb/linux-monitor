# Use official GCC image
FROM gcc:13

# Set working directory
WORKDIR /app

# Copy source code
COPY exporter.cpp . 

# Compile the application
RUN g++ exporter.cpp -o exporter-pthread -std=c++17 

# Expose metrics port
EXPOSE 9100

# Copy start script
COPY start.sh .
RUN chmod +x start.sh

# Run the app
CMD ["./start.sh"]
