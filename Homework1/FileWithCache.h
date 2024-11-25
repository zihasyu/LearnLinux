#ifndef FILEWITHCACHE_H
#define FILEWITHCACHE_H

#include <pthread.h>
#include "FileStatus.h"

class FileWithCache
{
public:
    static FileWithCache *GetInstance();                 // GetInstance()函数用于获取FileWithCache类的唯一实例，实现单例模式。
    static FileStatus FileWrite(const char *wMsg);       // 写入文件
    static FileStatus FileRead(char *rMsg, int rLength); // 读取文件

    FileStatus FWrite(const char *wMsg);
    FileStatus FRead(char *rMsg, int rLength);
    FileStatus Flush();

private:
    static void OnProcessExit();
    FileStatus WriteMsgToFile(const char *wMsg);
    static pthread_mutex_t *InitializeMutex();

private:
    FileWithCache(const FileWithCache &);
    FileWithCache &operator=(const FileWithCache &);

    FileWithCache();
    ~FileWithCache();

private:
    int m_Fd;
    pthread_mutex_t *m_pMutexForUseFile;

    static FileWithCache *m_pFile;
    static pthread_mutex_t *m_pMutexForCreatingFile;

private:
    char *Buffer;
    int BufferUsedBytes;

private:
    bool m_bFlagForProcessExit;
    bool m_bFlagForLastReaded;
};

#endif
