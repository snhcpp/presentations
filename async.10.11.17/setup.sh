#!/bin/bash
mkdir build || :
cd build && cmake -G Ninja\
	-DCMAKE_USER_MAKE_RULES_OVERRIDE=CmakeFlags.txt \
	-DCMAKE_BUILD_TYPE=Debug ..

#  -DCMAKE_C_COMPILER=clang \
#  -DCMAKE_CXX_COMPILER=clang++

