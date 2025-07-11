#ifndef _SERIALIZER_H_
#define _SERIALIZER_H_

#include <bitset>

// 전방 참조
class CSendStream;

class CSerializer
{
protected:

    enum Flags
    {
        INVALID_READ            = (1 << 0),
        INVALID_WRITE           = (1 << 1),
        ALLOC_INTERNAL_BUFFER   = (1 << 2)
    };

public:

    CSerializer(char* szBuffer, unsigned long nBufferSize);
    CSerializer(CSendStream& SendStream, unsigned long nBufferSize);
    CSerializer(unsigned long nBufferSize);

    ~CSerializer();

    bool IsValid() { return (NULL != m_szBuffer && !m_Flags[INVALID_READ] && !m_Flags[INVALID_WRITE]); }
    
    const char*     GetRawBufferPtr() const { return m_szBuffer; }

    char*           GetReadPtr() const { return m_szBuffer + m_nReadSize; }
    char*           GetWritePtr() const { return m_szBuffer + m_nWriteSize; }

    unsigned int    GetWriteSize() const { return m_nWriteSize; }
    unsigned int    GetReadSize() const { return m_nReadSize; }
    int             GetRemainSize() const { return m_nBufferSize - m_nWriteSize; }
    
    template<typename T> CSerializer& operator << (T Data)
    {
        if(NULL != m_szBuffer && m_nWriteSize + sizeof(T) <= m_nBufferSize)
        {
            memcpy(m_szBuffer + m_nWriteSize, &Data, sizeof(T));
            m_nWriteSize += sizeof(T);
        }
        else
        {
            m_Flags[INVALID_WRITE] = true;
        }

        return *this;
    }

    template<typename T> CSerializer& operator >> (T& Data)
    {
        if(NULL != m_szBuffer && m_nReadSize + sizeof(T) <= m_nBufferSize)
        {
            memcpy(&Data, m_szBuffer + m_nReadSize, sizeof(T))
            m_nReadSize += sizeof(T);
        }
        else
        {
            m_Flags[INVALID_READ] = true;
        }

        return *this;
    }

    bool PutBuffer(const void* Data, unsigned long nLength);
    bool GetBuffer(const void* Data, unsigned long nLength);

private:

    char*           m_szBuffer;
    unsigned long   m_nBufferSize;
    unsigned long   m_nReadSize;
    unsigned long   m_nWriteSize;

    std::bitset<32> m_Flags;
};



#endif