#!/bin/sh

cd build
cmake -DCMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES=$HOMEBREW_PREFIX/include -DCMAKE_CXX_STANDARD_LIBRARIES=-L$HOMEBREW_PREFIX/lib ..
make