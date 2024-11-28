cd ..
mkdir -p ./bin
g++ ./src/main.cc ./src/FileSort.cc ./src/ThreadPool.h -o ./bin/FileSort -lpthread -g -fsanitize=address
echo "Compilation completed"
# g++ main.cc FileSort.cc ThreadPool.h -o FileSort -lpthread -g -fsanitize=address