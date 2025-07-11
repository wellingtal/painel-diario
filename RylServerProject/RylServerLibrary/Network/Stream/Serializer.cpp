#include "stdafx.h"
#include "Serializer.h"
#include "SendStream.h"

CSerializer::CSerializer(char* szBuffer, unsigned long nBufferSize)
:   m_szBuffer(szBuffer), m_nBufferSize(nBufferSize),
    m_nReadSize(0), m_nWriteSize(0)
{

}

CSerializer::CSerializer(CSendStream& SendStream, unsigned long nBufferSize)
:   m_szBuffer(SendStream.GetBuffer(nBufferSize)), 
    m_nBufferSize(nBufferSize), 
    m_nReadSize(0), m_nWriteSize(0)
{
    
}

CSerializer::CSerializer(unsigned long nBufferSize)
:   m_szBuffer(new char[nBufferSize]), m_nBufferSize(nBufferSize),
    m_nReadSize(0), m_nWriteSize(0)
{
    m_Flags[ALLOC_INTERNAL_BUFFER] = true;
}

CSerializer::~CSerializer()
{
    if(m_Flags[ALLOC_INTERNAL_BUFFER])
    {
        delete [] m_szBuffer;
    }
}


bool CSerializer::PutBuffer(const void* Data, unsigned long nLength)
{
    if(NULL != m_szBuffer && m_nWriteSize + nLength <= m_nBufferSize)
    {
        memcpy(m_szBuffer + m_nWriteSize, &Data, nLength);
        m_nWriteSize += nLength;
        return true;
    }
    
    m_Flags[INVALID_WRITE] = true;
    return false;
}

bool CSerializer::GetBuffer(const void* Data, unsigned long nLength)
{
    if(NULL != m_szBuffer && m_nReadSize + nLength <= m_nBufferSize)
    {
        memcpy(m_szBuffer + m_nReadSize, &Data, nLength);
        m_nReadSize += nLength;
        return true;
    }

    m_Flags[INVALID_READ] = true;
    return false;
}
