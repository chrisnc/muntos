FROM debian:bookworm

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y \
        clang \
        llvm \
        scons

RUN useradd rt
USER rt
