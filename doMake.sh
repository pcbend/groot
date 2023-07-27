#!/bin/bash


if [ ! -d "./build" ]; then 
  mkdir build
fi
cd build
cmake ..
make 
if [ -d "./bin" ]; then 
  mv ./bin .. 
fi
cd ..

