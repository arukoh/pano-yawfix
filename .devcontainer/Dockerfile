FROM mcr.microsoft.com/devcontainers/cpp:ubuntu-24.04

# Basic system dependencies
RUN apt-get update && apt-get install -y \
    ocl-icd-opencl-dev \
    opencl-headers \
    ffmpeg \
    pkg-config \
    ninja-build \
    clang-format \
    cppcheck \
    doxygen \
    graphviz \
    curl \
    mingw-w64 \
    mingw-w64-tools \
    g++-mingw-w64-x86-64 \
    gcc-mingw-w64-x86-64 \
    && rm -rf /var/lib/apt/lists/*

# Place OpenCL-Headers for MinGW-w64
ARG OPENCL_HEADERS_VERSION=v2024.10.24
RUN git clone --branch ${OPENCL_HEADERS_VERSION} --depth 1 https://github.com/KhronosGroup/OpenCL-Headers.git /tmp/OpenCL-Headers && \
    mkdir -p /usr/x86_64-w64-mingw32/include/CL && \
    cp -r /tmp/OpenCL-Headers/CL/* /usr/x86_64-w64-mingw32/include/CL/ && \
    rm -rf /tmp/OpenCL-Headers

# Install Taskfile.dev
RUN curl -sL https://taskfile.dev/install.sh | sh && \
    mv ./bin/task /usr/local/bin/task && \
    chmod +x /usr/local/bin/task

# Set working directory
WORKDIR /workspace

# Set user on container startup
USER vscode
