# This is a comment
FROM ubuntu:22.04
MAINTAINER me <little.mole@oha7.org>

# std dependencies
RUN DEBIAN_FRONTEND=noninteractive apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get upgrade -y
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y build-essential g++ \
libgtest-dev git pkg-config valgrind sudo joe wget \
clang libc++-dev libc++abi-dev libexpat-dev cmake

ARG CXX
ENV CXX=${CXX}

ARG BUILDCHAIN=make
ENV BUILDCHAIN=${BUILDCHAIN}

RUN echo "*********************"
RUN echo -e $CXX $BUILDCHAIN
RUN echo "*********************"

ADD docker/gtest.sh /usr/local/bin/gtest.sh
ADD docker/patex.sh /usr/local/bin/patex.sh
ADD docker/build.sh /usr/local/bin/build.sh
ADD docker/jsoncpp.sh /usr/local/bin/jsoncpp.sh

RUN /usr/local/bin/gtest.sh
RUN /usr/local/bin/patex.sh
RUN /usr/local/bin/jsoncpp.sh


RUN mkdir -p /usr/local/src/metacpp
ADD . /usr/local/src/metacpp

RUN "/usr/local/src/metacpp/docker/run.sh"

##CMD ["/bin/bash", "-c", "/user/local/src/metacpp/docker/run.sh"]
