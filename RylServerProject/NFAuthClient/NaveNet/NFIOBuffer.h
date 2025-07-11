/** 
 *  @file		NFIOBuffer.h
 *  @brief		I/O Buffering, Packet I/O Buffering
 *  @remarks	
 *  @author		������(edith2580@gmail.com)
 *  @date		2009-04-02
 */

#pragma once

namespace NaveNet { 

	#define IOBUF_DEF_SIZE	DEF_MAXPACKETSIZE*128		// IOBuffer�� �⺻ũ�� ��Ŷ 1024����ŭ ó������

	/**
	 @class		
	 @author	������(edith2580@gmail.com)
	 @date		2009-03-03
	 @brief		
	 @warning   
	*/
	/** 
	 *  @class		NFIOBuffer
	 *  @brief		I/O Buffering Ŭ����
	 *  @remarks	I/O Buffering �� ���� Ŭ����. Sock�� �̿��� ��Ŷ�� �ְ� ������ ��Ŷ��				\r\n
	 *  			���Ŀ��ų� ���ҵǾ� �ö� �� ��Ŷ�� ���� �� ó���Ҷ� ���ȴ�.(IOBuffer��			\r\n
	 *  			��ӹ��� PacektIOBuffer Ŭ������ ó���ϰ�..											\r\n
	 *  																								\r\n
	 *  			�⺻������ PacketBuffer�� ����Ǵ� ��Ʈ��(?)�� ������ ������ ����.					\r\n
	 *  			---------------------------------------------------------------------------------------	\r\n
	 *  			| 2byte | 2byte | CheckSum(4Byte) | Header.Siz-Headere ��ŭ�� ���� Packet �뷮 Header |	\r\n
	 *  			---------------------------------------------------------------------------------------	\r\n
	 *  			IOBufferŬ������ ���� ��Ŷ�� �ϳ��� �迭�� ���������� �־� �� ��Ŷ��				\r\n
	 *  			�����Ѵ�.  2Byte�� 65535������ ���̸� ó���Ѵ�.										\r\n
	 *  @par          
	 *  @author  Edith
	 *  @date    2009-04-04
	 */
	class NFIOBuffer
	{
	public:
		/// NFIOBuffer ������
		NFIOBuffer();
		/// NFIOBuffer �Ҹ���
		virtual ~NFIOBuffer();

	public:
		/// ���� ������ ũ�� ���.
		VOID	CalcBuffered();

		/**
		 * @brief	���� �����͸� ���ɴϴ�.
		 * @return  ���� ������
		 */
		inline CHAR* GetBuffer() { return	m_cBuf; }
		/**
		 * @brief	Head Pos�� �����մϴ�.
		 * @param Head Head Pos��
		 */
		inline VOID SetHead(INT Head) { m_iHead = Head; CalcBuffered(); }
		/**
		 * @brief	Head Pos�� �����ɴϴ�.
		 * @return	Head Pos ��
		 */
		inline INT	GetHead() { return m_iHead; }
		/**
		 * @brief	Tail Pos�� �����մϴ�.
		 * @param Tail Tail Pos��
		 */
		inline VOID SetTail(INT Tail) { m_iTail = Tail; }
		/**
		 * @brief	Tail Pos�� �����ɴϴ�.
		 * @return	Tail Pos ��
		 */
		inline INT	GetTail() { return m_iTail; }
		/**
		 * @brief	������ ��ü ũ�⸦ ���ɴϴ�.
		 * @return  ������ ��ü ũ��
		 */
		inline INT	GetBufSize() { return m_iBufSize; }

		/**
		 * @brief	������� ������ ���̸� ���ɴϴ�.
		 * @return	������� ������ ����
		 */
		inline INT GetBufferUsed() { return m_iBuffered; }
		/**
		 * @brief	����ִ� ������ ���̸� ���ɴϴ�.
		 * @return	����ִ� ������ ����
		 */
		inline INT GetEmptyBuffer() { return m_iBufSize - m_iBuffered; }

		/**
		 * @brief	���۸� �ʱ�ȭ�մϴ�.
		 */
		VOID	InitIOBuf();

		/**
		 * @brief	���۸� �߰��մϴ�.
		 * @param Buffer	����
		 * @param Size		ũ��
		 * @return	�߰��� ����
		 */
		INT		Append(const CHAR* Buffer, INT Size);

		/**
		 * @brief	���۸� �н��ϴ�.
		 * @param Buffer	����
		 * @param Size		���� ũ��
		 * @return	�о���� ����
		 */
		INT		GetData(CHAR* Buffer, INT Size);

		/**
		 * @brief	���۱��� üũ�մϴ�.
		 * @param Size	üũ�� ����
		 * @return	üũ�� ���� ������ Size���� ����
		 */
		INT		CheckData(INT Size);
		
		/**
		 * @brief	���۸� �����մϴ�.
		 * @param BufSize ������ ���� ũ��
		 */
		VOID	NewIOBuf(INT BufSize);

		/**
		 * @brief	���۸� �����մϴ�.
		 */
		VOID	DeleteIOBuf();

	private:
		/// ���� ������ ������ġ.
		INT		m_iHead;
		/// ���� ������ ��.
		INT		m_iTail;
		/// ������ ������ ������
		INT		m_iBufSize;
		/// ������ ������ ������.
		CHAR*	m_cBuf;

		/// ���� ������ ������ �̰��� ������ Over Flow..
		INT		m_iBuffered;
	};

	/** 
	 *  @class        NFPacketIOBuffer
	 *  @brief        Packet I/O Buffering Ŭ����
	 *  @remarks      NFPacket�� �ڵ�ȭ �Ǿ��ִ� Ŭ����		\r\n
	 *	   			  ������ Ŭ���̾�Ʈ�� ��ſ��� ���ȴ�.
	 *  @par          
	 *  @author  Edith
	 *  @date    2009-04-04
	 */
	class NFPacketIOBuffer : public NFIOBuffer
	{
	public:
		/// NFPacketIOBuffer ������
		NFPacketIOBuffer();
		/// NFPacketIOBuffer �Ҹ���
		virtual ~NFPacketIOBuffer();

	public:
		/// ���۸� Lock �մϴ�.
		VOID	Lock();

		/// ���۸� UnLock �մϴ�.
		VOID	UnLock();

		// �Ѱ��з��� ��Ŷ�� ���´�.
		INT		GetPacket(NFPacket* Packet);

		// ��Ŷüũ
		BOOL	CheckPacket();

	private:
		/// Lock �� UnLock���� ����� ���� 
		INT		m_iLockHead;
	};


}