#include "ThreadPool.h"
#include "FileSort.h"

// 打印最终结果文件的前一百个元素来看看 >.<
void printResult(const string &fileName)
{
    cout << "Stage One Finished" << endl;
    cout << "Data is sorted" << endl;
    cout << "Print first 100 elements in file" << endl;
    ifstream inFile(fileName, ios::in | ios::binary);
    int64_t num;
    uint64_t counter = 0;
    while (inFile.read((char *)&num, sizeof(num)))
    {
        cout << num << endl;
        counter++;
        if (counter > 100)
            break;
    }
    inFile.close();
}
// 获取当前目录下的文件名和文件数量
void getFileCountAndNames(const std::string &dirPath, vector<string> &fileName)
{
    DIR *dirp = opendir(dirPath.c_str());
    struct dirent *dp;
    string fileNameStr;
    while ((dp = readdir(dirp)) != nullptr)
    {

        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {

            fileNameStr.assign(dp->d_name);

            fileName.push_back(fileNameStr);
        }
    }
    closedir(dirp);
}

int main()
{
    // 初始化线程池，开启8个线程，实验环境限制。
    ThreadPool pool(8);
    pool.init();
    // 生成FileSort对象， 指定数据集路径
    FileSort fs;
    string datasetPath = "./testData";
    // 执行排序第一阶段，将数据集整理成多个相同大小的局部有序文件
    fs.ChunkAndSortData(datasetPath);
    // 存储中间变量
    vector<string> fileName;
    // 用来获取线程执行结果
    vector<future<void>> futureVec;
    // 用于命名中间文件
    uint64_t sn = 0;
    cout << "Stage One Start" << endl;
    while (true)
    {
        // 获取档期目录下的文件信息
        getFileCountAndNames(datasetPath, fileName);
        cout << "merge start" << endl;
        if (fileName.size() == 1)
        {
            // 排序完毕，打印结果
            printResult(datasetPath + "/" + fileName[0]);
            break;
        }

        for (size_t i = 0; i < fileName.size(); i += 2)
        {
            // 处理文件个数是奇数的情况，最后一个文件下一轮处理
            if (i == fileName.size() - 1)
                break;
            // 待merge的files
            vector<string> files;
            files.push_back(datasetPath + "/" + fileName[i]);
            files.push_back(datasetPath + "/" + fileName[i + 1]);

            string outputFileName = datasetPath + "/merge-" + to_string(sn++);
            // 添加到线程池的任务队列
            auto future = pool.submit(FileSort::MergeTwoFile, files, outputFileName);
            // 记录future，方便后续回收结果
            futureVec.push_back(std::move(future));
        }
        for (auto &f : futureVec)
        {
            f.get();
        }
        // 清空vector
        futureVec.clear();
        fileName.clear();
    }
    // 关闭线程池
    pool.shutdown();
    return 1;
}