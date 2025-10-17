# ------------------------------------------------------------
# Dockerfile for building and running EmbryOS on macOS/arm64
# ------------------------------------------------------------
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
ENV EMBRYOS=/workspace
WORKDIR $EMBRYOS

# Install build tools, RISC-V toolchain, QEMU, ...
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    gcc \
    make \
    git \
    wget \
    zip \
    unzip \
    gdb-multiarch \
    ca-certificates \
    nano \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Clone EmbryOS
RUN git clone https://github.com/rvanren/EmbryOS.git

# Setup the RISC-V compiler
RUN wget https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack/releases/download/v14.2.0-3/xpack-riscv-none-elf-gcc-14.2.0-3-linux-arm64.tar.gz && \
    tar -xzf xpack-riscv-none-elf-gcc-14.2.0-3-linux-arm64.tar.gz
ENV PATH="${PATH}:${EMBRYOS}/xpack-riscv-none-elf-gcc-14.2.0-3/bin"

# Setup QEMU
RUN wget https://github.com/xpack-dev-tools/qemu-riscv-xpack/releases/download/v8.2.2-1/xpack-qemu-riscv-8.2.2-1-linux-arm64.tar.gz && \
    tar -xzf xpack-qemu-riscv-8.2.2-1-linux-arm64.tar.gz
ENV PATH="${PATH}:${EMBRYOS}/xpack-qemu-riscv-8.2.2-1/bin"

CMD ["/bin/bash"]
