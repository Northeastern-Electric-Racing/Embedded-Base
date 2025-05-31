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
    python3-pip


RUN wget https://github.com/renode/renode/releases/download/v1.15.3/renode-1.15.3.linux-portable.tar.gz
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
RUN wget -qO- https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.07/gcc-arm-none-eabi-10.3-2021.07-x86_64-linux.tar.bz2 | tar -xvj

ENV PATH $PATH:/home/dev/gcc-arm-none-eabi-10.3-2021.07/bin

WORKDIR /home/app

# Set up safe directory
RUN git config --global --add safe.directory /home/app
