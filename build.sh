#!/bin/sh

#rm -rf ./build/* && mkdir build
rm -rf ./build

# we are using GCC compile >= 5.1, so we need to set the compiler.libcxx to the new ABI.
conan profile new default --detect
conan profile update settings.compiler.libcxx=libstdc++11 default
conan install . -if build

cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -B build
cmake --build ./build --config Release -j2
