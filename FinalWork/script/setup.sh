cd ..
rm -rf ./bin
mkdir -p ./bin
mkdir -p ./bin/testData
g++ ./src/main.cc ./src/FileSort.cc ./src/ThreadPool.h -o ./bin/FileSort -lpthread -g -fsanitize=address
echo "Compilation completed main.cc"
g++ -o ./bin/generate ./script/generate.cc
echo "Compilation completed generate.cc"

cd bin
./generate
echo "Data generated"
./FileSort
echo "FileSort finished"