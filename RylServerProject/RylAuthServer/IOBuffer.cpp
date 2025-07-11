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
	// Head�� ���� ������ ������ġ.
	// Tail�� ���Ǿ��� ������ ��.
	m_iHead = m_iTail = 0;

	// ������ ������ ������
	m_iBufSize = 0;
	// ������ ������ ������.
	m_cBuf = NULL;

	// ���� ������ ������ �̰��� ������ Over Flow..
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

// Size ��ŭ Buffer�� ������ ����.
int CIOBuffer::Append(char* Buffer, int Size)
{
	// �����÷ο찡 �ȴ�. 
	if(m_iBuffered + Size >= m_iBufSize)
	{
//		throw "CIOBuffer::Append : Buffer overflow";
		return -1;
	}

	int aSize = 0;
	int Added = 0;

	// ��� Size�� �߰��Ҷ����� ó���Ѵ�.
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

// Size��ŭ ����Ÿ�� �о� Buffer�� ����.
int	CIOBuffer::GetData(char* Buffer, int Size)
{
	// ���ִ� ����Ÿ ���� ���� ������ ���ִ� ����Ÿ�� �а� �Ѵ�.
	if(GetBufferUsed() < Size)
		Size = GetBufferUsed();

	if(Size <= 0)
		return 0;

	// �߷�������. ó���Ѵ�.
	if(m_iHead+Size >= m_iBufSize)
	{
		// ���⿡ ���� ���װ� �ִ�.
		int Size1 = m_iBufSize - m_iHead;
		memcpy(Buffer, m_cBuf+m_iHead, Size1);
		memcpy(Buffer+Size1, m_cBuf, Size-Size1);
	}
	else	// ���߷� ������.
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
	// ���ִ� ����Ÿ ���� ���� ������ ���ִ� ����Ÿ�� �а� �Ѵ�.
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
	// �ʱ�ȭ.
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
		// ����
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
