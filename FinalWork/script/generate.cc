#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>

const std::string DATA_DIR = "./testData";
const uint64_t TARGET_TOTAL_SIZE = 128ULL * 1024 * 1024 * 1024; // 128GB
const uint64_t MAX_FILE_SIZE = 8ULL * 1024 * 1024 * 1024;       // 8GB
const uint64_t MIN_FILE_SIZE = 16 * 1024;                       // 16KB

std::random_device rd;
std::mt19937_64 gen(rd());
std::uniform_int_distribution<int64_t> dist(INT64_MIN, INT64_MAX);
std::uniform_int_distribution<uint64_t> size_dist(MIN_FILE_SIZE, MAX_FILE_SIZE);

int64_t generate_random_number()
{
    return dist(gen);
}

uint64_t generate_file_size()
{
    return size_dist(gen);
}

void generate_file(const std::string &file_path, uint64_t file_size)
{
    std::ofstream outFile(file_path, std::ios::out | std::ios::binary);
    uint64_t current_size = 0;

    while (current_size < file_size)
    {
        int64_t random_number = generate_random_number();
        outFile.write(reinterpret_cast<const char *>(&random_number), sizeof(random_number));
        current_size += sizeof(random_number);
    }

    outFile.close();
}

int main()
{
    // 创建目录
    mkdir(DATA_DIR.c_str(), 0777);

    uint64_t total_size = 0;
    int file_index = 0;

    while (total_size < TARGET_TOTAL_SIZE)
    {
        std::string file_name = DATA_DIR + "/file_" + std::to_string(file_index) + ".bin";
        uint64_t remaining_size = TARGET_TOTAL_SIZE - total_size;
        if (remaining_size < MIN_FILE_SIZE)
        {
            break;
        }
        uint64_t file_size = generate_file_size();
        if (file_size > remaining_size)
        {
            file_size = remaining_size;
        }
        generate_file(file_name, file_size);
        total_size += file_size;
        std::cout << "Generated " << file_name << " with size " << file_size << " bytes" << std::endl;
        file_index++;
    }

    std::cout << "Data generation completed. Total size: " << total_size << " bytes" << std::endl;
    return 0;
}