#include "FileSort.h"

bool FileSort::ChunkAndSortData(const string &dirPath)
{
    cout << "Stage One Start" << endl;
    // 记录当前读取的size
    uint64_t totalSize = 0;
    // 存放读入的数据
    vector<int64_t> data;
    // 记录目录信息
    DIR *dirp = opendir(dirPath.c_str());
    struct dirent *dp;
    // 读取目录下文件
    while ((dp = readdir(dirp)) != nullptr)
    {
        // 跳过根目录
        if (dp->d_name[0] == '.' || (dp->d_name[0] == '.' && dp->d_name[1] == '.') || dp->d_name[0] == 'o')
        {
            continue;
        }
        // 获取文件信息，包括文件名文件路径等文件路径
        struct stat buf;
        string fileName;
        fileName.assign(dp->d_name, strlen(dp->d_name));
        string filePath = dirPath + "/" + fileName;
        // 初始化输入流
        ifstream file(filePath, ios::in | ios::binary);
        int64_t num;
        cout << "Process File " << fileName << endl;
        while (file.read((char *)&num, sizeof(int64_t)))
        {
            data.push_back(num);
            totalSize += sizeof(int64_t);
            // 按照内存限制读入数据
            if (totalSize >= MEMORY_LIMIT)
            {
                // 排序
                sort(data.begin(), data.end());
                // 写入文件
                string fileName = dirPath + "/" + "output-" + to_string(sn++);
                ofstream outFile(fileName, ios::out | ios::binary);
                outFile.write((char *)data.data(), data.size() * sizeof(int64_t));
                outFile.close();
                // 清空
                data.clear();
                totalSize = 0;
            }
        }
        file.close();
    }

    if (!data.empty())
    {
        // 排序
        sort(data.begin(), data.end());
        // 写入文件
        string fileName = dirPath + "/" + "output-" + to_string(sn++);
        ofstream outFile(fileName, ios::out | ios::binary);
        outFile.write((char *)data.data(), data.size() * sizeof(int64_t));
        outFile.close();
        // 清空
        data.clear();
        totalSize = 0;
    }

    closedir(dirp);
    // 清理生成的中间文件
    DeleteFileWithPattern(dirPath, "^test.*");

    cout << "Stage One Finish" << endl;

    return true;
}

void FileSort::DeleteFile(const string &filePath)
{
    if (std::remove(filePath.c_str()) != 0)
    {
        // 删除文件失败，可能是文件不存在或者其他错误
        cout << "delete file fail!!!" << endl;
        return;
    }
    else
    {
        // 删除文件成功
        cout << "delete file done!" << endl;
        return;
    }
}

void FileSort::DeleteFileWithPattern(const string &dirPath, const string &pattern)
{
    // 按照正则表达式匹配删除
    std::regex re(pattern);
    DIR *dirp = opendir(dirPath.c_str());
    struct dirent *dp;
    while (() != nullptr)
    {
        if (std::regex_match(dp->d_name, re))
        {
            std::string filePath = dirPath + "/" + dp->d_name;
            unlink(filePath.c_str());
        }
    }
    closedir(dirp);
}

void FileSort::MergeTwoFile(vector<string> &fileName, string &outputFile)
{
    // MergeFile时考虑到内存限制，按照规定大小批量读入进行归并排序。
    // 因为上一步已经将多个结果文件排成有序的所以可以这样做。
    const int SIZE = 4 * 1024 * 1024 / sizeof(int64_t); // 4MiB
    // 俩个vector用来存放读入的数据
    vector<int64_t> data1(SIZE), data2(SIZE);
    // 初始化输入输出流
    ifstream in1(fileName[0], ios::in | ios::binary), in2(fileName[1], ios::in | ios::binary);
    ofstream out(outputFile, ios::out | ios::binary);

    int read1 = 0, read2 = 0;
    while (!in1.eof() && !in2.eof())
    {
        // 读入俩个文件的数据
        if (in1.read((char *)data1.data(), SIZE * sizeof(int64_t)))
        {
            read1 = in1.gcount() / sizeof(int64_t);
        }
        else
        {
            read1 = 0;
        }

        if (in2.read((char *)data2.data(), SIZE * sizeof(int64_t)))
        {
            read2 = in2.gcount() / sizeof(int64_t);
        }
        else
        {
            read2 = 0;
        }
        // 进行归并排序
        size_t i = 0, j = 0;
        while (i < read1 && j < read2)
        {
            if (data1[i] < data2[j])
            {
                out.write((char *)&data1[i++], sizeof(int64_t));
            }
            else
            {
                out.write((char *)&data2[j++], sizeof(int64_t));
            }
        }

        // 可能存在一个文件已经读完，但是另一个文件还没有，因此需要处理数据
        while (i < read1)
        {
            out.write((char *)&data1[i++], sizeof(int64_t));
        }
        while (j < read2)
        {
            out.write((char *)&data2[j++], sizeof(int64_t));
        }
        cout << "4 MiB process done" << endl;
    }
    // 可能存在一个文件已经读完，但是另一个文件还没有，因此需要处理数据
    if (read1 > 0)
    {
        while (in1.read((char *)data1.data(), SIZE * sizeof(int64_t)))
        {
            read1 = in1.gcount() / sizeof(int64_t);
            out.write((char *)data1.data(), read1 * sizeof(int64_t));
        }
    }
    if (read2 > 0)
    {
        while (in2.read((char *)data2.data(), SIZE * sizeof(int64_t)))
        {
            read2 = in2.gcount() / sizeof(int64_t);
            out.write((char *)data2.data(), read2 * sizeof(int64_t));
        }
    }

    cout << "merge two file done" << endl;
    // 删除中间文件
    DeleteFile(fileName[0]);
    DeleteFile(fileName[1]);
    in1.close();
    in2.close();
    out.close();
}