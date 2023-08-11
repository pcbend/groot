#!/bin/bash


if [ ! -d "./build" ]; then 
  mkdir build
fi
cd build
cmake ..

if [ $# -gt 0 ]; then   
  re='^[0-9]+$'
  if [[ $1 =~ $re ]]; then
    make -j $1
  else 
  make
  fi
else 
  make
fi
if [ -d "./bin" ]; then 
  mv ./bin .. 
fi
cd ..

