#ifndef CLTHREADRW_H
#define CLTHREADRW_H

#include"FileThread.h"
#include"FileStatus.h"
class FileReadThread:public FileThread
{
public:
	FileReadThread(int length);
	
	virtual ~FileReadThread();
	
public:
	virtual FileStatus RunThreadFunction();
	
private:
	char m_ReadContext[20];
	int rLength;
	
};

class FileWriteThread:public FileThread
{
public:
	FileWriteThread(char *wStr);
	
	virtual ~FileWriteThread();
	
public:
	virtual FileStatus RunThreadFunction();
	
private:
	char m_WriteContext[32];
};

#endif
