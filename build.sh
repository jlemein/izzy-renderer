#!/bin/sh

cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -B build
cmake --build ./build --config Release -j2
