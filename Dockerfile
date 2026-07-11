FROM ubuntu:26.04

# using [OCI](https://github.com/opencontainers/image-spec/blob/main/annotations.md) suggested label names
LABEL org.opencontainers.image.authors="Oğuzhan Türk <stkyoht@hotmail.com>"
LABEL org.opencontainers.image.version="0.1.0"

## install ground-system and dependencies
RUN apt-get update --quiet --yes \
    && DEBIAN_FRONTEND=noninteractive apt-get dist-upgrade -q -y \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y -q --no-install-recommends \
    # general stuff for the docker container
	git \
    # special build stuff
    build-essential \
    cmake \
    # dependencies
    protobuf-compiler-grpc \
    libprotobuf-dev \
    libgrpc++-dev \
    liblog4cpp5-dev \
    libgmock-dev \
    libgtest-dev \
    # dev stuff for debug and execution of executable
    clang-format \
    valgrind \
    lcov \
    gcovr
