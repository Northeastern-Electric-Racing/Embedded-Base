FROM ubuntu:latest

# Set up container and time zones
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive TZ="America/New_York" \
    apt-get -y install tzdata

# Download Linux support tools
RUN apt-get update && apt-get install -y \
    build-essential \
    wget \
    curl \
    openocd \
    git \
    gdb-multiarch \
    minicom \
    vim \
    clang-format \
    linux-tools-generic \
    gcc-arm-linux-gnueabihf \
    g++-arm-linux-gnueabihf \
    pkg-config \
    cmake \
    libssl-dev \
    python3 \
    python3-pip \
    ruby

RUN wget https://github.com/renode/renode/releases/download/v1.15.3/renode-1.15.3.linux-portable.tar.gz
RUN mkdir renode_portable && tar -xvf renode-*.linux-portable.tar.gz -C renode_portable --strip-components=1
ENV PATH $PATH:/renode_portable

RUN wget https://github.com/ThrowTheSwitch/CMock/archive/refs/tags/v2.6.0.tar.gz -O cmock.tar.gz
RUN mkdir cmock_portable && tar -xvf cmock.tar.gz -C cmock_portable --strip-components=1
ENV PATH $PATH:/cmock_portable

RUN wget https://github.com/ThrowTheSwitch/Unity/archive/refs/tags/v2.6.1.tar.gz -O unity.tar.gz 
RUN mkdir -p /cmock_portable/vendor/unity && tar -xvf unity.tar.gz -C /cmock_portable/vendor/unity --strip-components=1

# Set up a development tools directory
WORKDIR /home/dev
ADD . /home/dev

COPY ./scripts .

RUN echo "source /home/dev/scripts/alias.sh" >> ~/.bashrc

# for usb/ip
RUN ln -sf /usr/lib/linux-tools-*/* /usr/bin/

# Install cross compiler
RUN wget -qO- https://developer.arm.com/-/media/Files/downloads/gnu/11.3.rel1/binrel/arm-gnu-toolchain-11.3.rel1-x86_64-arm-none-eabi.tar.xz | tar -xJv

ENV PATH $PATH:/home/dev/arm-gnu-toolchain-11.3.rel1-x86_64-arm-none-eabi/bin

WORKDIR /home/app

# Set up safe directory
RUN git config --global --add safe.directory /home/app
