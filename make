#!/bin/bash

doBuild=0
doClean=0
J=1

re='^[0-9]+$'

if [ $# -eq 0 ]; then
  doBuild=1
elif [[ $1 =~ $re ]]; then
  doBuild=1
  J=$1
elif [ $1 = "clean" ]; then
  doClean=1
else 
  echo "unknown command: " $@
fi


if [ $doBuild -eq 1 ]; then

  if [ ! -d "./build" ]; then 
    mkdir build
  fi
  cd build
  cmake ..  || cmake3 ..

  make -j$J

  if [ -d "./bin" ]; then 
    mv ./bin .. 
  fi
  cd ..
fi

if [ $doClean -eq 1 ]; then
  echo cleaning...
  if [ -d "./build" ]; then
    rm -rf ./build
  fi

  if [ -d "./bin" ]; then
    rm -rf ./bin
  fi
fi
