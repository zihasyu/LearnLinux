#!/bin/bash
cd ..
# 生成数据的目录
DATA_DIR="./bin/testData"
# 创建目录
mkdir -p $DATA_DIR

# 目标总大小（字节）
TARGET_TOTAL_SIZE=$((128 * 1024 * 1024 * 1024)) # 128GB
# 每个文件的最大大小（字节）
MAX_FILE_SIZE=$((8 * 1024 * 1024 * 1024)) # 8GB
# 每个文件的最小大小（字节）
MIN_FILE_SIZE=$((1024 * 16)) # 16KB

# 生成随机64位有符号数的函数
generate_random_number() {
    echo $((RANDOM * RANDOM * RANDOM * RANDOM))
}

# 生成文件大小的函数，使用指数分布
generate_file_size() {
    local lambda=0.0000000001
    local size=$(awk -v lambda=$lambda 'BEGIN{srand(); print int(-log(1-rand())/lambda)}')
    if [ $size -lt $MIN_FILE_SIZE ]; then
        size=$MIN_FILE_SIZE
    elif [ $size -gt $MAX_FILE_SIZE ]; then
        size=$MAX_FILE_SIZE
    fi
    echo $size
}

# 生成文件的函数
generate_file() {
    local file_path=$1
    local file_size=$2
    local current_size=0

    while [ $current_size -lt $file_size ]; do
        random_number=$(generate_random_number)
        echo $random_number >> $file_path
        current_size=$(($current_size + ${#random_number} + 1))
    done
}

# 生成数据文件
total_size=0
file_index=0

while [ $total_size -lt $TARGET_TOTAL_SIZE ]; do
    file_name="$DATA_DIR/file_$file_index.txt"
    remaining_size=$(($TARGET_TOTAL_SIZE - $total_size))
    if [ $remaining_size -lt $MIN_FILE_SIZE ]; then
        break
    fi
    file_size=$(generate_file_size)
    if [ $file_size -gt $remaining_size ]; then
        file_size=$remaining_size
    fi
    generate_file $file_name $file_size
    total_size=$(($total_size + $file_size))
    echo "Generated $file_name with size $file_size bytes"
    file_index=$(($file_index + 1))
done

echo "Data generation completed. Total size: $total_size bytes"