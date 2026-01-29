FROM ubuntu:latest

USER 0

RUN apt update && apt install -y cmake build-essential libboost-all-dev libgtest-dev

WORKDIR /usr/src/app

COPY . .

# Build
RUN rm -rf build && \
    cmake -S . -B build && \
    cmake --build build --parallel 8

# Test
RUN build/test/test-employee-lib