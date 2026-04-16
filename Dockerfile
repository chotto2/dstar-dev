# syntax=docker/dockerfile:1
FROM gcc:latest

# 日本ミラー変更 + BuildKit キャッシュマウント
RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    sed -i 's|http://archive.ubuntu.com|http://ftp.jaist.ac.jp/pub/Linux/ubuntu|g' /etc/apt/sources.list && \
    apt-get update && apt-get install -y \
        build-essential \
        cmake \
        libgmp-dev \
        pkg-config

WORKDIR /app
COPY . /app/

# build
RUN mkdir -p build && cd build && \
    cmake .. && \
    make

# exe copy
RUN cp build/dstar /usr/local/bin/

CMD ["/bin/bash"]
