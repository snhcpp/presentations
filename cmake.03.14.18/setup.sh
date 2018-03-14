#!/bin/bash

install="tesseract-ocr-eng tesseract-ocr libtesseract-dev libleptonica-dev ninja-build"
install+=" libboost-system-dev libboost-timer-dev"

echo "installing packages - ${install}"
sudo apt-get install -qqy ${install}

#EXTRA_CMAKE_FLAGS="-DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++"

rm -rf build-debug
mkdir build-debug || :
cd build-debug && cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug .. ${EXTRA_CMAKE_FLAGS}
