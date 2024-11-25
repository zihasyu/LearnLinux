#ifndef FILESORT_H
#define FILESORT_H
// 模拟内存限制
#define MEMORY_LIMIT 64 * 1024 * 1024 // 64 MiB

#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include <fstream>
#include <regex>
#include <unistd.h>
#include <queue>
#include <cstdio>

using namespace std;

class FileSort
{
public:
    // 类名
    string myName_ = "FileSort";
    // 将所有的数据按批次读入内存，每批数据排序后生成一个中间文件，此时该中间文件是有序的。
    // 这些中间文件再后续归并排序的过程中会动态删除，不会引入额外的存储开销
    bool ChunkAndSortData(const string &dirPath);
    // 文件序列号，用于命名排序过程中生成的中间文件
    uint64_t sn = 0;
    // 用来清理中间文件
    static void DeleteFile(const string &filePath);
    // 用来批量清理中间文件
    static void DeleteFileWithPattern(const string &dirPath, const string &pattern);
    // 该方法用来实现后续多个有序文件的归并排序，要作为线程的工作函数所以声明为静态方法
    static void MergeTwoFile(vector<string> &fileName, string &outputFile);
};

#endif