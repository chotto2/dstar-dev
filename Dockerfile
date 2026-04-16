# syntax=docker/dockerfile:1
FROM gcc:latest

RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    apt-get update && apt-get install -y \
        cmake \
        libgmp-dev \
        pkg-config

WORKDIR /app
COPY . /app/

RUN mkdir -p build && cd build && \
    cmake .. && \
    make

RUN cp build/dstar /usr/local/bin/

CMD ["/bin/bash"]
