#ifndef _GAMA_SERVER_LIB_BUFFER_QUEUE_H_
#define _GAMA_SERVER_LIB_BUFFER_QUEUE_H_

// 전방 참조
#include <list>

class CBuffer;

class CBufferQueue
{
public:

    CBufferQueue();
    virtual ~CBufferQueue();

    void setMaxQueueSize(unsigned long dwMaxQueueSize) { m_maxQueueSize = dwMaxQueueSize; }

    bool enqueue(CBuffer* lpBuffer, bool bPendHead = false);
    CBuffer* dequeue();
    
    void splice(CBufferQueue& Buffer_In, bool bPendHead = false);
	void splice_n(CBufferQueue& Buffer_In, unsigned long dwSpliceNum, bool bPendHead = false);

    void clear();

    CBuffer* getHead() { return m_lpHead; }

	bool empty() const { return 0 == m_bufferNum; }	
    size_t getQueueSize() const { return m_queueSize; }
    size_t getBufferNum() const { return m_bufferNum; }

private:

	CBuffer*    m_lpHead;
    CBuffer*    m_lpTail;
    
    size_t      m_bufferNum;
    size_t      m_queueSize;
    size_t      m_maxQueueSize;
};

#endif