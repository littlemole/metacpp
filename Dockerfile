# This is a comment
FROM ubuntu:22.04
MAINTAINER me <little.mole@oha7.org>

# std dependencies
RUN DEBIAN_FRONTEND=noninteractive apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get upgrade -y
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y build-essential g++ \
libgtest-dev cmake git pkg-config valgrind sudo joe wget \
clang libc++-dev libc++abi-dev libexpat-dev 

ARG CXX
ENV CXX=${CXX}

ADD docker/gtest.sh /usr/local/bin/gtest.sh
RUN /usr/local/bin/gtest.sh


RUN cd /usr/local/src && git clone https://github.com/littlemole/patex
RUN cd /usr/local/src/patex && make clean && make -e test && make -e install

RUN mkdir -p /usr/local/src/metacpp
ADD . /usr/local/src/metacpp

ARG BUILDCHAIN=make
ENV BUILDCHAIN=${BUILDCHAIN}

RUN echo -e $CXX $BUILDCHAIN

RUN /usr/local/src/metacpp/docker/jsoncpp.sh


##CMD /bin/bash
RUN "/usr/local/src/metacpp/docker/run.sh"

##CMD ["/bin/bash", "-c", "/user/local/src/metacpp/docker/run.sh"]
