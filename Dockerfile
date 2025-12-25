FROM ubuntu:22.04

ARG TARGETARCH
ENV ARCH=linux-${TARGETARCH}

ENV GCC_V=15.2.0-1
ENV QEMU_V=9.2.4-1

ENV DEBIAN_FRONTEND=noninteractive
ENV EMBRYOS=/embryos
WORKDIR $EMBRYOS

# Install packages
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    gcc \
    g++ \
    make \
    git \
    wget \
    zip \
    unzip \
    gdb-multiarch \
    python3 \
    python3-pip \
    python3-venv \
    ca-certificates \
    nano \
    vim \
    gcc-riscv64-unknown-elf \
    gcc-riscv64-linux-gnu \
    binutils-riscv64-linux-gnu \
    qemu-system-misc \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

CMD ["/bin/bash"]
