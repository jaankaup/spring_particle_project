#!/bin/bash

touch src/hello.cpp
cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" -H. -BbuildNative
cd buildNative
make -j4 all
#make all
cd ..
mkdir -p buildNative/bin/shaders
cp shadersNative/* buildNative/bin/shaders
