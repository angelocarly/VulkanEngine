#!/bin/bash

mkdir build
cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ..
make

cd ..
ln -s build/compile_commands.json .
