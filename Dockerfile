FROM debian:bookworm

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y \
        clang \
        gdb \
        less \
        llvm \
        scons

ARG username
RUN useradd $username
USER $username