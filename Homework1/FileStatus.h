#ifndef FILESTATUS_H
#define FILESTATUS_H

class FileStatus
{
public:
    FileStatus(long lReturnCode, long lErrorCode);
    FileStatus(const FileStatus &s);
    virtual ~FileStatus();

public:
    bool IsSuccess();

public:
    const long &m_clReturnCode;
    const long &m_clErrorCode;

private:
    long m_lReturnCode;
    long m_lErrorCode;
};

#endif
