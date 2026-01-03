FROM ubuntu:22.04

ARG TARGETARCH
ENV DEBIAN_FRONTEND=noninteractive
ENV EMBRYOS=/embryos
WORKDIR $EMBRYOS

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential gcc g++ make git curl zip unzip \
    gdb-multiarch python3 python3-pip python3-venv \
    ca-certificates nano vim \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

RUN set -e; \
    if [ "$TARGETARCH" = "amd64" ]; then XARCH="x64"; else XARCH="$TARGETARCH"; fi; \
    ARCH_STR="linux-${XARCH}"; \
    \
    echo "Downloading for architecture: ${ARCH_STR}"; \
    \
    curl -L -o /gcc.tar.gz "https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack/releases/download/v15.2.0-1/xpack-riscv-none-elf-gcc-15.2.0-1-${ARCH_STR}.tar.gz" \
    && tar -zxvf /gcc.tar.gz -C / && rm /gcc.tar.gz; \
    \
    curl -L -o /qemu.tar.gz "https://github.com/xpack-dev-tools/qemu-riscv-xpack/releases/download/v9.2.4-1/xpack-qemu-riscv-9.2.4-1-${ARCH_STR}.tar.gz" \
    && tar -zxvf /qemu.tar.gz -C / && rm /qemu.tar.gz

ENV PATH="$PATH:/xpack-riscv-none-elf-gcc-15.2.0-1/bin:/xpack-qemu-riscv-9.2.4-1/bin"

CMD ["/bin/bash"]
