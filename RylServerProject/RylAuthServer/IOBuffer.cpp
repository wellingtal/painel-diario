// IOBuffer.cpp: implementation of the CIOBuffer class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "IOBuffer.h"
#include <string.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIOBuffer::CIOBuffer()
{
	// Head는 읽을 버퍼의 시작위치.
	// Tail은 사용되어진 버퍼의 끝.
	m_iHead = m_iTail = 0;

	// 생성한 버퍼의 사이즈
	m_iBufSize = 0;
	// 생성한 버퍼의 포인터.
	m_cBuf = NULL;

	// 사용된 버퍼의 사이즈 이값이 음수면 Over Flow..
	m_iBuffered = 0;
}

CIOBuffer::~CIOBuffer()
{
	DeleteIOBuf();
}

void CIOBuffer::InitIOBuf()
{
	m_iHead = m_iTail = 0;
	m_iBuffered = 0;

	if(m_cBuf)
		memset(m_cBuf, 0, sizeof(m_cBuf));
}

void CIOBuffer::NewIOBuf(int BufSize)
{
	if(BufSize <= 0)
		BufSize = IOBUF_DEF_SIZE;
	
	m_cBuf = new char[BufSize];
	if(m_cBuf == NULL)
	{
//		throw "CIOBuffer::NewIOBuf : Memory allocation failure!";
		return;
	}
	
	m_iBufSize = BufSize;
	
	InitIOBuf();
}

void CIOBuffer::DeleteIOBuf()
{
	if(m_cBuf)
	{
		delete []m_cBuf;
		m_cBuf = NULL;
	}
	m_iBufSize = 0;
	m_iHead = m_iTail = 0;
	m_iBuffered = 0;
}

// Size 만큼 Buffer의 내용을 쓴다.
int CIOBuffer::Append(char* Buffer, int Size)
{
	// 오버플로우가 된다. 
	if(m_iBuffered + Size >= m_iBufSize)
	{
//		throw "CIOBuffer::Append : Buffer overflow";
		return -1;
	}

	int aSize = 0;
	int Added = 0;

	// 모든 Size를 추가할때까지 처리한다.
	while(Size > 0)
	{
		if(Size > m_iBufSize-m_iTail)
			aSize = m_iBufSize-m_iTail;
		else aSize = Size;

		if(aSize)
		{
			memcpy(m_cBuf+m_iTail, Buffer, aSize);

			Added += aSize;
			Size -= aSize;
			Buffer += aSize;
			m_iTail += aSize;
			if(m_iTail >= m_iBufSize)
				m_iTail -= m_iBufSize;
		}
	}

	CalcBuffered();

	return Added;
}

// Size만큼 데이타를 읽어 Buffer에 쓴다.
int	CIOBuffer::GetData(char* Buffer, int Size)
{
	// 써있는 데이타 보다 많이 읽으면 써있는 데이타만 읽게 한다.
	if(GetBufferUsed() < Size)
		Size = GetBufferUsed();

	if(Size <= 0)
		return 0;

	// 잘려있으면. 처리한다.
	if(m_iHead+Size >= m_iBufSize)
	{
		// 여기에 지금 버그가 있다.
		int Size1 = m_iBufSize - m_iHead;
		memcpy(Buffer, m_cBuf+m_iHead, Size1);
		memcpy(Buffer+Size1, m_cBuf, Size-Size1);
	}
	else	// 안잘려 있으면.
	{
		memcpy(Buffer, m_cBuf+m_iHead, Size);
	}

	m_iHead += Size;
	if(m_iHead >= m_iBufSize)
		m_iHead -= m_iBufSize;

	CalcBuffered();
	return Size;
}

int CIOBuffer::CheckData(int Size)
{
	// 써있는 데이타 보다 많이 읽으면 써있는 데이타만 읽게 한다.
	if(GetBufferUsed() < Size)
		Size = GetBufferUsed();

	if(Size <= 0)
		return 0;

	m_iHead += Size;
	if(m_iHead >= m_iBufSize)
		m_iHead -= m_iBufSize;

	CalcBuffered();
	return Size;
}


void CIOBuffer::CalcBuffered()
{
	if(m_iHead > m_iTail)
		m_iBuffered =  m_iBufSize - m_iHead + m_iTail;
	else
		m_iBuffered =  m_iTail-m_iHead;
}


CPacketIOBuffer::CPacketIOBuffer()
{
	// 초기화.
	CIOBuffer::CIOBuffer();
}

CPacketIOBuffer::~CPacketIOBuffer()
{
	DeleteIOBuf();
}

void CPacketIOBuffer::Lock()
{
	LockHead = GetHead();
}

void CPacketIOBuffer::UnLock()
{
	SetHead(LockHead);
}

int CPacketIOBuffer::GetPacket(char* Packet)
{
	if(m_iHead == m_iTail)
		return 0;

	memset(Packet, 0, MAX_PACKETSIZE);

	int Size = 0;
	int OldHead = GetHead();

	char End[3] = "\r\n";

	for(int i = 0; i < MAX_PACKETSIZE; ++i)
	{
		// 삽입
		Packet[i] = m_cBuf[m_iHead];

		m_iHead++;

		if(m_iHead+1 > m_iBufSize)
			m_iHead = 0;

		if(m_iHead == m_iTail)
			break;

		if(i < 1)
			continue;

		if(strcmp(&Packet[i-1], End)==0)
			break;
	}

	Size = (int)strlen(Packet);
	if(Size == 0)
	{
		SetHead(OldHead);
		return 0;
	}

	CalcBuffered();
	return Size;
}
