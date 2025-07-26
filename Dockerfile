FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
   build-essential \
   make \
   ninja-build \
   gcc-mipsel-linux-gnu \
   binutils-mipsel-linux-gnu \
   cmake \
   xz-utils \
   unzip \
   wget \
   wine

WORKDIR /opt


# Download and install the correct MIPS toolchain (mipsel-none-elf)
RUN wget https://github.com/Lameguy64/PSn00bSDK/releases/download/v0.24/PSn00bSDK-0.24-Linux.zip && \
   unzip PSn00bSDK-0.24-Linux.zip -d /opt && \
   rm PSn00bSDK-0.24-Linux.zip

COPY tools/png2tim /opt/png2tim

# Build and install png2tim
RUN cd /opt/png2tim && \
   gcc *.c -o png2tim && \
   cp png2tim /usr/local/bin/ && \
   chmod +x png2tim.sh
   


ENV PATH="/opt/PSn00bSDK-0.24-Linux/bin:/opt/PSn00bSDK-0.24-Linux/include:${PATH}" \
   PSN00BSDK_LIBS="/opt/PSn00bSDK-0.24-Linux/lib/libpsn00b"

WORKDIR /workspace
CMD ["/bin/bash"]
