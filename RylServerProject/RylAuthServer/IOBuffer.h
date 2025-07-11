// IOBuffer.h: interface for the CIOBuffer class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

/*****************************************************************************
I/O Buffering �� ���� Ŭ����. Sock�� �̿��� ��Ŷ�� �ְ� ������ ��Ŷ�� 
���Ŀ��ų� ���ҵǾ� �ö� �� ��Ŷ�� ���� �� ó���Ҷ� ���ȴ�.(IOBuffer�� 
��ӹ��� PacektIOBuffer Ŭ������ ó���ϰ�..

�⺻������ PacketBuffer�� ����Ǵ� ��Ʈ��(?)�� ������ ������ ����.
-------------------------------------------
| Header(4Byte-PACKET_HEADSIZE) | StringData (Header Size) |
-------------------------------------------
IOBufferŬ������ ���� ��Ŷ�� �ϳ��� �迭�� ���������� �־� �� ��Ŷ��
�����Ѵ�.  2Byte�� 65535������ ���̸� ó���Ѵ�.
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#define MAX_PACKETSIZE 512
#define IOBUF_DEF_SIZE	MAX_PACKETSIZE*10		// IOBuffer�� �⺻ũ�� ��Ŷ 1024����ŭ ó������

class CIOBuffer
{
protected:
	// Head�� ���� ������ ������ġ.
	// Tail�� ���Ǿ��� ������ ��.
	int		m_iHead,m_iTail;
	// ������ ������ ������
	int		m_iBufSize;
	// ������ ������ ������.
	char*	m_cBuf;

	// ���� ������ ������ �̰��� ������ Over Flow..
	int		m_iBuffered;

public:
	// ���� ������ ũ�� ���ϱ�.
	void	CalcBuffered();

	// ��Ÿ inline
	inline char* GetBuffer() { return	m_cBuf; }
	inline void SetHead(int Head) { m_iHead = Head; CalcBuffered(); }
	inline int	GetHead() { return m_iHead; }
	inline void SetTail(int Tail) { m_iTail = Tail; }
	inline int	GetTail() { return m_iTail; }
	inline int	GetBufSize() { return m_iBufSize; }
	// ������� ������ ��
	inline int GetBufferUsed() { return m_iBuffered; }
	// ����ִ� ������ ��
	inline int GetEmptyBuffer() { return m_iBufSize - m_iBuffered; }

	void	InitIOBuf();

	//		�߰��Ѵ�.
	int		Append(char* Buffer, int Size);
	//		�����´�.
	int		GetData(char* Buffer, int Size);
	//		üũ�Ѵ�
	int		CheckData(int Size);
	
	void	NewIOBuf(int BufSize);
	void	DeleteIOBuf();


	CIOBuffer();
	virtual ~CIOBuffer();
};

class CPacketIOBuffer : public CIOBuffer
{
	int		LockHead;
public:
	void	Lock();
	void	UnLock();

	// �Ѱ��з��� ��Ŷ�� ���´�.
	int		GetPacket(char* Packet);

	CPacketIOBuffer();
	virtual ~CPacketIOBuffer();
};
