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
    tmux \
    libncurses5 \
    linux-tools-generic \
    gcc-arm-linux-gnueabihf \
    g++-arm-linux-gnueabihf \
    pkg-config \
    cmake \
    libssl-dev \
    python3 \
    python3-pip \
    udev \
    usbutils

# Install and setup rust
RUN curl https://sh.rustup.rs -sSf | bash -s -- -y
ENV PATH="/root/.cargo/bin:${PATH}"
RUN rustup target add armv7-unknown-linux-gnueabihf

# Install Python YAML Parser
RUN pip install "ruamel.yaml<0.18.0"

RUN wget https://builds.renode.io/renode-1.14.0+20231003gitf86ac3cf.linux-portable.tar.gz
RUN mkdir renode_portable && tar -xvf renode-*.linux-portable.tar.gz -C renode_portable --strip-components=1
ENV PATH $PATH:/renode_portable

# Set up a development tools directory
WORKDIR /home/dev
ADD . /home/dev

COPY ./scripts .

RUN echo "source /home/dev/scripts/alias.sh" >> ~/.bashrc

# for usb/ip
RUN ln -sf /usr/lib/linux-tools-*/* /usr/bin/

# Install cross compiler
RUN wget -qO- https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2 | tar -xvj

ENV PATH $PATH:/home/dev/gcc-arm-none-eabi-10.3-2021.10/bin

WORKDIR /home/app
