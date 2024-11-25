#include "FileStatus.h"

FileStatus::FileStatus(long lReturnCode, long lErrorCode) : m_clReturnCode(m_lReturnCode), m_clErrorCode(m_lErrorCode)
{
    m_lReturnCode = lReturnCode;
    m_lErrorCode = lErrorCode;
}

FileStatus::~FileStatus()
{
}

FileStatus::FileStatus(const FileStatus &s) : m_clReturnCode(m_lReturnCode), m_clErrorCode(m_lErrorCode)
{
    m_lReturnCode = s.m_lReturnCode;
    m_lErrorCode = s.m_lErrorCode;
}

bool FileStatus::IsSuccess()
{
    if (m_clReturnCode >= 0)
        return true;
    else
        return false;
}
