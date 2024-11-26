#!/bin/bash

mkdir -p ./bin
g++ ./src/main.cc ./src/FileStatus.cc ./src/FileThread.cc ./src/FileThreadRW.cc ./src/FileWithCache.cc -o ./bin/FileWithCacheTest -std=c++11 -O3 -Wall -lpthread -g -fsanitize=address
echo "Compilation completed"