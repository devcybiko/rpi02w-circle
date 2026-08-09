FROM debian:bookworm-slim

RUN apt-get update && \
    apt-get install -y \
        build-essential \
        git \
        wget \
        xz-utils \
        ca-certificates \
        make \
        python3 \
        file \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /opt

RUN wget -q \
    https://developer.arm.com/-/media/Files/downloads/gnu/15.2.rel1/binrel/arm-gnu-toolchain-15.2.rel1-x86_64-arm-none-eabi.tar.xz \
    && tar -xf arm-gnu-toolchain-15.2.rel1-x86_64-arm-none-eabi.tar.xz \
    && rm arm-gnu-toolchain-15.2.rel1-x86_64-arm-none-eabi.tar.xz

ENV PATH="/opt/arm-gnu-toolchain-15.2.rel1-x86_64-arm-none-eabi/bin:${PATH}"

WORKDIR /work

CMD ["/bin/bash"]