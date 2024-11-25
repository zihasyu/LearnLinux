#include "FileThreadRW.h"
#include "FileWithCache.h"
#include <stdio.h>
#include <string.h>

FileReadThread::FileReadThread(int length)
{
    rLength = length;
}
FileReadThread::~FileReadThread()
{
}
FileStatus FileReadThread::RunThreadFunction()
{
    FileStatus s = FileWithCache::FileRead(m_ReadContext, rLength);
    if (!s.IsSuccess())
    {
        printf("read file error.\n");
        return FileStatus(-1, 0);
    }
    m_ReadContext[rLength] = '\0';
    printf("read file success.\n"); // gai
    printf("%s\n", m_ReadContext);
    return FileStatus(0, 0);
}

FileWriteThread::FileWriteThread(char *wStr)
{
    strcpy(m_WriteContext, wStr);
}

FileWriteThread::~FileWriteThread()
{
}
FileStatus FileWriteThread::RunThreadFunction()
{
    FileStatus s = FileWithCache::FileWrite(m_WriteContext);

    if (!s.IsSuccess())
    {
        printf("write file error.\n");
        return FileStatus(-1, 0);
    }
    printf("write file success.\n"); // gai
    return FileStatus(0, 0);
}
