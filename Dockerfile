FROM ubuntu:22.04

# Install build dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    make \
    ninja-build \
    cmake \
    unzip \
    wget \
    ca-certificates \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /opt

# Download and install PSn00bSDK (includes mipsel-none-elf-gcc 12.3.0, mkpsxiso, elf2x)
RUN wget -q https://github.com/Lameguy64/PSn00bSDK/releases/download/v0.24/PSn00bSDK-0.24-Linux.zip && \
    unzip -q PSn00bSDK-0.24-Linux.zip -d /opt && \
    ln -s /opt/PSn00bSDK-0.24-Linux /opt/psn00bsdk && \
    rm PSn00bSDK-0.24-Linux.zip

# Build and install png2tim
COPY tools/png2tim /opt/png2tim
RUN cd /opt/png2tim && \
    gcc -O2 *.c -o /usr/local/bin/png2tim && \
    chmod +x /usr/local/bin/png2tim png2tim.sh

ENV PATH="/opt/psn00bsdk/bin:${PATH}" \
    PSN00BSDK_LIBS="/opt/psn00bsdk/lib/libpsn00b"

WORKDIR /workspace
CMD ["/bin/bash"]
