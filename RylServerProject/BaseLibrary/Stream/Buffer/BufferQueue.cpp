#include "stdafx.h"
#include "Buffer.h"
#include "BufferQueue.h"
#include "BufferFactory.h"

CBufferQueue::CBufferQueue()
:	m_lpHead(0),
    m_lpTail(0), 
    m_bufferNum(0),
    m_queueSize(0),
    m_maxQueueSize(0xFFFFFFFF)
{

}


CBufferQueue::~CBufferQueue()
{
    clear();
}


bool CBufferQueue::enqueue(CBuffer* lpBuffer, bool bPendHead)
{
    if(0 != lpBuffer)
    {        
		size_t bufferUsage = lpBuffer->length();

        if(m_queueSize + bufferUsage < m_maxQueueSize)
        {
            if(0 == m_lpHead)
            {
                m_lpHead = m_lpTail = lpBuffer;
            }
            else if(!bPendHead)
            {
                m_lpTail->next(lpBuffer);
				lpBuffer->prev(m_lpTail);

                m_lpTail = lpBuffer;                				
            }
            else
            {
                lpBuffer->next(m_lpHead);
				m_lpHead->prev(lpBuffer);

                m_lpHead = lpBuffer;
            }

            ++m_bufferNum;
            m_queueSize += bufferUsage;
            return true;
        }
    }

    return false;
}


CBuffer* CBufferQueue::dequeue()
{
    CBuffer* lpBuffer = 0;
    
    if(0 != m_lpHead)
    {
        lpBuffer = m_lpHead;
        m_lpHead = m_lpHead->next();

		lpBuffer->prev(0);
		lpBuffer->next(0);

        if(m_lpTail == lpBuffer)
        {
            m_lpTail = 0;
        }

        --m_bufferNum;
        m_queueSize -= lpBuffer->length();
    }

    return lpBuffer;
}


void CBufferQueue::clear()
{
    CBuffer* lpDelete = 0;

	while(0 != m_lpHead)
    {
        lpDelete = m_lpHead;
        m_lpHead = m_lpHead->next();

		SAFE_RELEASE_BUFFER(lpDelete);
    }

    m_lpTail = 0;
    m_bufferNum = 0;
    m_queueSize = 0;
}

void CBufferQueue::splice(CBufferQueue& Buffer_In, bool bPendHead)
{
	// ����� BufferQueue�� ������ �� BufferQueue�� ���δ�.
	if(0 != Buffer_In.m_lpHead)
	{
		if(0 == m_lpHead)
		{
			m_lpHead = Buffer_In.m_lpHead;
			m_lpTail = Buffer_In.m_lpTail;
		}
		else if(!bPendHead)
		{		
			m_lpTail->next(Buffer_In.m_lpHead);
			Buffer_In.m_lpHead->prev(m_lpTail);

			m_lpTail = Buffer_In.m_lpTail;			
		}
		else
		{
			Buffer_In.m_lpTail->next(m_lpHead);
			m_lpHead->prev(Buffer_In.m_lpTail);

			m_lpHead = Buffer_In.m_lpHead;
		}

		m_bufferNum += Buffer_In.getBufferNum();
		m_queueSize += Buffer_In.getQueueSize();

		Buffer_In.m_lpHead = 0;
		Buffer_In.m_lpTail = 0;
		Buffer_In.m_bufferNum = 0;
		Buffer_In.m_queueSize = 0;
	}
}

void CBufferQueue::splice_n(CBufferQueue& Buffer_In, unsigned long dwSpliceNum, bool bPendHead)
{
	// ����� BufferQueue�� ������, �ִ� dwSpliceNum�� ��ŭ�� �� BufferQueue�� ���δ�.
	if (Buffer_In.getBufferNum() <= dwSpliceNum)
	{
		// �ִ밳�� �̸��̸� ���� ���δ�.
		splice(Buffer_In, bPendHead);
	}
	else if(0 != Buffer_In.m_lpHead)
	{
		// �ִ밳�� �̻��� ������ �����Ƿ�, �ϴ� Ž�� ���� �߶󳽴�.
		
		unsigned long dwBufferNum = 0;	// ���� ����
		unsigned long dwQueueSize = 0;	// ������ ����(byte)
		
		CBuffer* lpNewBufferHead = Buffer_In.m_lpHead;

		for(; dwBufferNum < dwSpliceNum; ++dwBufferNum)
		{
			dwQueueSize += lpNewBufferHead->length();
			lpNewBufferHead = lpNewBufferHead->next();
		}

		// �յڸ� �߶󳽴�.
		CBuffer* lpSplicedHead = Buffer_In.m_lpHead;
		CBuffer* lpSplicedTail = lpNewBufferHead;
		
		Buffer_In.m_lpHead = lpSplicedTail->next();
		Buffer_In.m_lpHead->prev(0);
		lpSplicedTail->next(0);

		Buffer_In.m_bufferNum -= dwBufferNum;
		Buffer_In.m_queueSize -= dwQueueSize;       

		if(0 == m_lpHead)
		{
			m_lpHead = lpSplicedHead;
			m_lpTail = lpSplicedTail;
		}
		else if(!bPendHead)
		{		
			m_lpTail->next(lpSplicedHead);
			lpSplicedHead->prev(m_lpTail);

			m_lpTail = lpSplicedTail;			
		}
		else
		{
			lpSplicedTail->next(m_lpHead);
			m_lpHead->prev(lpSplicedTail);

			m_lpHead = lpSplicedHead;
		}

		m_bufferNum += dwBufferNum;
		m_queueSize += dwQueueSize;
	}
}