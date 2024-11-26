#include "FileThread.h"

FileThread::FileThread()
{
}
FileThread::~FileThread()
{
}
FileStatus FileThread::Run()
{
    int r = pthread_create(&m_ThreadID, NULL, StartFunctionOfThread, this);
    if (r != 0)
        return FileStatus(-1, 0);
    return FileStatus(0, 0);
}

void *FileThread::StartFunctionOfThread(void *pThis)
{
    FileThread *pThreadThis = (FileThread *)pThis;
    FileStatus s = pThreadThis->RunThreadFunction();
    return (void *)s.m_clReturnCode;
}
FileStatus FileThread::WaitForDeath()
{
    int r = pthread_join(m_ThreadID, 0);
    if (r != 0)
        return FileStatus(-1, 0);
    return FileStatus(0, 0);
}
