// IOBuffer.h: interface for the CIOBuffer class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

/*****************************************************************************
I/O Buffering 을 위한 클래스. Sock을 이용해 패킷을 주고 받을때 패킷이 
합쳐오거나 분할되어 올때 그 패킷을 관리 및 처리할때 사용된다.(IOBuffer을 
상속받은 PacektIOBuffer 클래스로 처리하게..

기본적으로 PacketBuffer에 저장되는 스트링(?)의 형식은 다음과 같다.
-------------------------------------------
| Header(4Byte-PACKET_HEADSIZE) | StringData (Header Size) |
-------------------------------------------
IOBuffer클래스는 위의 패킷을 하나의 배열에 순차적으로 넣어 그 패킷을
관리한다.  2Byte는 65535까지의 길이를 처리한다.
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#define MAX_PACKETSIZE 512
#define IOBUF_DEF_SIZE	MAX_PACKETSIZE*10		// IOBuffer의 기본크기 패킷 1024개만큼 처리가능

class CIOBuffer
{
protected:
	// Head는 읽을 버퍼의 시작위치.
	// Tail은 사용되어진 버퍼의 끝.
	int		m_iHead,m_iTail;
	// 생성한 버퍼의 사이즈
	int		m_iBufSize;
	// 생성한 버퍼의 포인터.
	char*	m_cBuf;

	// 사용된 버퍼의 사이즈 이값이 음수면 Over Flow..
	int		m_iBuffered;

public:
	// 사용된 버퍼의 크기 구하기.
	void	CalcBuffered();

	// 기타 inline
	inline char* GetBuffer() { return	m_cBuf; }
	inline void SetHead(int Head) { m_iHead = Head; CalcBuffered(); }
	inline int	GetHead() { return m_iHead; }
	inline void SetTail(int Tail) { m_iTail = Tail; }
	inline int	GetTail() { return m_iTail; }
	inline int	GetBufSize() { return m_iBufSize; }
	// 사용중인 버퍼의 양
	inline int GetBufferUsed() { return m_iBuffered; }
	// 비어있는 버퍼의 양
	inline int GetEmptyBuffer() { return m_iBufSize - m_iBuffered; }

	void	InitIOBuf();

	//		추가한다.
	int		Append(char* Buffer, int Size);
	//		가져온다.
	int		GetData(char* Buffer, int Size);
	//		체크한다
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

	// 한개분량의 패킷을 얻어온다.
	int		GetPacket(char* Packet);

	CPacketIOBuffer();
	virtual ~CPacketIOBuffer();
};
