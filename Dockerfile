FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libgmp-dev \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . /app/

# build
RUN mkdir -p build && cd build && \
    cmake .. && \
    make

# exe copy
RUN cp build/dstar /usr/local/bin/


CMD {"/bin/bash"}

