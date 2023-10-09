#!/bin/bash
set -e

if [ "$CXX" == "g++" ]
then 
        FLAGS=""
else
        FLAGS="-stdlib=libc++"
fi

echo "**********************************"
echo "building metacpp with" 
echo "$CXX using $BUILDCHAIN"
echo "FLAGS= $FLAGS"
echo "**********************************"

cd /usr/local/src/metacpp

if [ "$BUILDCHAIN" == "make" ] 
then
    make clean
    make -e test
    make clean
    make -e install
else

    mkdir build
    cd build
    cmake .. -DCMAKE_CXX_COMPILER=$CXX -DCMAKE_CXX_FLAGS=$FLAGS -DCMAKE_CXX_STANDARD=20 -DWITH_TEST=$WITH_TEST
    make 
    if [ "$WITH_TEST" == "On" ]
    then
	    make test
    fi
    make install
fi

