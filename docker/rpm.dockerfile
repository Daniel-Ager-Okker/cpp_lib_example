FROM registry.red-soft.ru/ubi8/ubi:latest

USER 0

RUN dnf update -y && \
    dnf install -y libstdc++-static gcc-c++ make cmake boost-devel gtest-devel libasan libubsan

WORKDIR /usr/src/app

COPY . .

# Build
RUN rm -rf build && \
    cmake -S . -B build && \
    cmake --build build --parallel 8

# Test
RUN build/test/test-employee-lib