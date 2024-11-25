#ifndef FILETHREAD_H
#define FILETHREAD_H

#include <pthread.h>
#include "FileStatus.h"
class FileThread
{
public:
    FileThread();
    virtual ~FileThread();
    FileStatus Run();
    FileStatus WaitForDeath();

private:
    static void *StartFunctionOfThread(void *);

protected:
    virtual FileStatus RunThreadFunction() = 0;
    pthread_t m_ThreadID;
};

#endif
