#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "FileWithCache.h"

#define LOG_FILE_NAME "a.txt"
#define BUFFER_SIZE 4096

FileWithCache *FileWithCache::m_pFile = 0;
pthread_mutex_t *FileWithCache::m_pMutexForCreatingFile = FileWithCache::InitializeMutex();

pthread_mutex_t *FileWithCache::InitializeMutex()
{
    pthread_mutex_t *p = new pthread_mutex_t;

    if (pthread_mutex_init(p, 0) != 0)
    {
        delete p;
        return 0;
    }

    return p;
}

/* ------------------------ 构造函数 ------------------------ */
/*
 * 打开log文件
 * 初始化缓存区
 * 初始化互斥锁
 * 初始化相关flag
 */

FileWithCache::FileWithCache()
{
    m_Fd = open(LOG_FILE_NAME, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if (m_Fd == -1)
        throw "In FileWithCache::FileWithCache(), open error";

    Buffer = new char[BUFFER_SIZE];
    BufferUsedBytes = 0;

    m_bFlagForProcessExit = false; // 标记进程是否已经退出
    m_bFlagForLastReaded = false;  // 标记上次操作是否为读
    m_pMutexForUseFile = new pthread_mutex_t;
    if (pthread_mutex_init(m_pMutexForUseFile, 0) != 0) // 当初始化过程中出现错误时，会及时清理已分配的资源，并抛出异常，以防止资源泄露。
    {
        delete m_pMutexForUseFile;
        delete[] Buffer;
        close(m_Fd);

        throw "In FileWithCache::CLFileWR(), pthread_mutex_init error";
    }
}

/* ------------------------ 写入文件 ------------------------ */
/*
 * 打开log文件
 * 初始化缓存区
 * 初始化互斥锁
 * 初始化相关flag
 */

FileStatus FileWithCache::FileWrite(const char *wMsg)
{
    FileWithCache *pFile = FileWithCache::GetInstance(); // 获取文件操作对象
    if (pFile == 0)
        return FileStatus(-1, 0);

    FileStatus s = pFile->FWrite(wMsg);
    if (s.IsSuccess())
        return FileStatus(0, 0);
    else
        return FileStatus(-1, 0);
}

FileStatus FileWithCache::FWrite(const char *wMsg)
{

    if (wMsg == 0)
        return FileStatus(-1, 0);
    int len_strmsg = strlen(wMsg);

    if (len_strmsg == 0)
        return FileStatus(-1, 0);

    // 写入操作之前申请互斥锁
    if (pthread_mutex_lock(m_pMutexForUseFile) != 0)
        return FileStatus(-1, 0);

    // 如果前一次操作为读操作则将缓存清空重置
    if (m_bFlagForLastReaded)
    {
        // 将当前缓存的数据量置空
        BufferUsedBytes = 0;
    }
    // 修改读标志
    m_bFlagForLastReaded = false;
    try
    {
        // 如果写入数据大于缓存的size则直接写入文件，跳过缓存
        if ((len_strmsg > BUFFER_SIZE) || (m_bFlagForProcessExit))
            throw WriteMsgToFile(wMsg);

        // 待写入的数据长度大于缓存剩余空间，那么先将当前缓存中的内容写入文件，然后将待写入的数据写入缓冲中。
        int nleftroom = BUFFER_SIZE - BufferUsedBytes;
        if (len_strmsg > nleftroom)
        {
            lseek(m_Fd, 0, SEEK_END);
            if (write(m_Fd, Buffer, BufferUsedBytes) == -1)
                throw FileStatus(-1, errno);

            BufferUsedBytes = 0;
        }

        memcpy(Buffer + BufferUsedBytes, wMsg, len_strmsg);
        BufferUsedBytes += len_strmsg;
        throw FileStatus(0, 0);
    }
    catch (FileStatus &s)
    {
        // 写操作结束，解锁
        if (pthread_mutex_unlock(m_pMutexForUseFile) != 0)
            return FileStatus(-1, 0);

        return s;
    }
}

FileStatus FileWithCache::WriteMsgToFile(const char *wMsg)
{
    lseek(m_Fd, 0, SEEK_END);
    // 先将缓存中的数据写入文件，保证文件内容顺序的正确。
    if (write(m_Fd, Buffer, BufferUsedBytes) == -1)
        throw FileStatus(-1, errno);
    BufferUsedBytes = 0;
    if (write(m_Fd, wMsg, strlen(wMsg)) == -1)
        return FileStatus(-1, errno);

    return FileStatus(0, 0);
}

FileStatus FileWithCache::Flush()
{
    if (m_bFlagForLastReaded) // 如果上次操作是读，则不能刷新缓存
    {
        return FileStatus(0, 0);
    }

    if (pthread_mutex_lock(m_pMutexForUseFile) != 0)
        return FileStatus(-1, 0);

    try
    {
        if (BufferUsedBytes == 0) // 缓存数据长度为0
            throw FileStatus(0, 0);
        lseek(m_Fd, 0, SEEK_END);
        if (write(m_Fd, Buffer, BufferUsedBytes) == -1)
            throw FileStatus(-1, errno);

        BufferUsedBytes = 0;
        printf("flush the buffer.\n"); // 提示信息

        throw FileStatus(0, 0);
    }
    catch (FileStatus &s)
    {
        if (pthread_mutex_unlock(m_pMutexForUseFile) != 0)
            return FileStatus(-1, 0);

        return s;
    }
}

// 获取文件实例对象
FileWithCache *FileWithCache::GetInstance()
{
    // 查m_pFile是否为0，如果不为0，表示FileWithCache对象已经创建，函数直接返回该对象。
    if (m_pFile != 0)
        return m_pFile;
    // 检查m_pMutexForCreatingFile是否为0，如果为0，表示互斥锁未初始化，函数返回0
    if (m_pMutexForCreatingFile == 0)
        return 0;

    if (pthread_mutex_lock(m_pMutexForCreatingFile) != 0)
        return 0;

    if (m_pFile == 0)
    {
        // 查m_pFile是否为0，如果为0，表示FileWithCache对象未创建，
        // 函数尝试创建FileWithCache对象。如果创建对象失败，函数解锁互斥锁并返回0。
        try
        {
            m_pFile = new FileWithCache;
        }
        catch (const char *)
        {
            pthread_mutex_unlock(m_pMutexForCreatingFile);
            return 0;
        }
        // 尝试注册OnProcessExit函数，该函数在进程退出时被调用。
        // 如果注册失败，函数设置m_bFlagForProcessExit为true，表示进程已经退出或即将退出。
        if (atexit(FileWithCache::OnProcessExit) != 0)
        {
            m_pFile->m_bFlagForProcessExit = true;

            if (pthread_mutex_unlock(m_pMutexForCreatingFile) != 0)
                return 0;
        }
        else if (pthread_mutex_unlock(m_pMutexForCreatingFile) != 0)
            return 0;

        return m_pFile;
    }

    if (pthread_mutex_unlock(m_pMutexForCreatingFile) != 0)
        return 0;

    return m_pFile;
}

void FileWithCache::OnProcessExit() // 程序退出时，需要自动刷新缓存
{
    FileWithCache *pFile = FileWithCache::GetInstance();
    if (pFile != 0)
    {
        pthread_mutex_lock(pFile->m_pMutexForUseFile);
        pFile->m_bFlagForProcessExit = true;
        pthread_mutex_unlock(pFile->m_pMutexForUseFile);

        pFile->Flush();
    }
}
FileStatus FileWithCache::FileRead(char *rMsg, int rLength)
{
    FileWithCache *pFile = FileWithCache::GetInstance(); // 获取文件操作对象
    if (pFile == 0)
        return FileStatus(-1, 0);

    if (rMsg == 0)
    {
        return FileStatus(-1, 0);
    }
    if (rLength == 0)
    {
        return FileStatus(0, 0);
    }

    FileStatus s = pFile->FRead(rMsg, rLength);
    if (s.IsSuccess())
        return FileStatus(0, 0);
    else
        return FileStatus(-1, 0);
}

FileStatus FileWithCache::FRead(char *rMsg, int rLength)
{

    if (rLength == 0)
        return FileStatus(0, 0);
    // 获取mutex，保证读写互斥
    if (pthread_mutex_lock(m_pMutexForUseFile) != 0)
        return FileStatus(-1, 0);

    // 检查上次操作是否为写操作，如果是写操作，则将缓存内容写入文件
    if (!m_bFlagForLastReaded)
    {
        lseek(m_Fd, 0, SEEK_END);
        if (write(m_Fd, Buffer, BufferUsedBytes) == -1)
            throw FileStatus(-1, errno);
        BufferUsedBytes = 0;
    }
    m_bFlagForLastReaded = true;
    try
    {
        // 读取的长度大于缓存的大小，则从文件中读出内容并将缓存填满
        if (rLength > BUFFER_SIZE)

        {
            lseek(m_Fd, 0, SEEK_SET);
            if (read(m_Fd, rMsg, rLength) == -1)
                throw FileStatus(-1, 0);
            memcpy(Buffer, rMsg, BUFFER_SIZE);
            BufferUsedBytes = BUFFER_SIZE;
            printf("read a lot,now read from file.\n"); // 提示信息
            throw FileStatus(0, 0);
        }
        // 反之则从缓存中读出数据
        if (rLength < BufferUsedBytes)
        {
            memcpy(rMsg, Buffer, rLength);
            printf("read from buffer.\n"); // 提示信息
            // printf("%s\n", rMsg);
            throw FileStatus(0, 0);
        }
        // 如果读取长度大于缓存中已有数据的长度，则从文件中读出内容，再将读出的内容放入缓存中
        if (rLength > BufferUsedBytes)
        {
            lseek(m_Fd, 0, SEEK_SET);
            if (read(m_Fd, rMsg, rLength) == -1)
                throw FileStatus(-1, 0);
            memcpy(Buffer, rMsg, rLength);
            BufferUsedBytes = rLength;
            printf("read from file.\n"); // 提示信息
            // printf("%s\n", rMsg);
            throw FileStatus(0, 0);
        }
    }
    catch (FileStatus &s)
    {
        if (pthread_mutex_unlock(m_pMutexForUseFile) != 0) // 读操作结束，解锁
            return FileStatus(-1, 0);

        return s;
    }

    return FileStatus(0, 0);
}
