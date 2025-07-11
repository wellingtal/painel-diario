#ifndef _PACKET_BASE
#define _PACKET_BASE


////////////////////////////////////////////////////////////////////////////////////////////////////
//															
//	������ ���۸� ����	
//
//	SS_SMOOTH			: ��Ȱ�� ���� ����.			
//	SS_LOADED			: �ణ�� ���ϰ� �ֽ�.	(50 % of bufferring)			
//	SS_BUSY				: ������ ���ϰ� �ֽ�.	(70 % of bufferring)			
//	SS_VERYBUSY			: �ؽ��� ������ ������	(��Ŷ �ս��� �ʷ��� �� �ֽ�.)			
//
////////////////////////////////////////////////////////////////////////////////////////////////////
union ServerInfo 
{
    enum ServerState 
    { 
        SS_SMOOTH, SS_LOADED, SS_BUSY, SS_VERYBUSY 
    };

	struct sSrvState
	{		
		unsigned short	wError;			// ���� �ڵ�
		unsigned short	wSrvState;		// ���� ����
	};
    
    sSrvState       SrvState;	
	unsigned long	dwServerInfo;
};




////////////////////////////////////////////////////////////////////////////////////////////////////
// �⺻ ��Ŷ									
#pragma pack(1)

struct PktBase 
{
	enum PktBaseErr
	{
		NO_SERVER_ERR	= 0,
		SERVER_ERROR	= 1
	};

	typedef unsigned char	StartType;
	typedef unsigned char	CMDType;
	typedef unsigned short	LengthType;
	
protected:

	StartType		m_StartBit;
	CMDType			m_Cmd;
	LengthType		m_Len;				// ��� ���̸� ������ ��Ŷ ����
	unsigned long	m_CodePage;
	ServerInfo		m_SrvInfo;

public:

	inline void InitPtHead(unsigned short Len_In, unsigned char Cmd_In,
		unsigned short State_In, unsigned short Error_In);

	inline void InitPtHead(unsigned short Len_In, unsigned char Cmd_In, unsigned long Tick_In);
	
	inline void SetError(unsigned short Error)		{ m_SrvInfo.SrvState.wError = Error;	}
	inline unsigned short GetError(void)			{ return m_SrvInfo.SrvState.wError;	}
    inline unsigned short GetState(void)            { return m_SrvInfo.SrvState.wSrvState; }

	inline StartType GetStartBit(void)				{ return m_StartBit; }

	inline void SetCodePage(unsigned long Code)		{ m_CodePage = Code; }
	inline unsigned long GetCodePage(void)			{ return m_CodePage; }

	inline void	SetServerInfo(unsigned long Info)	{ m_SrvInfo.dwServerInfo = Info; }
	inline unsigned long GetServerInfo(void)		{ return m_SrvInfo.dwServerInfo; }

	inline CMDType GetCmd(void) const               { return m_Cmd;	}
	inline bool IsCrypt(void);
	inline void SetCrypt(void);
	inline bool IsCompress(void);
	inline void SetCompress(void);
	inline LengthType GetUncompressLen(void);

	inline void SetLen(LengthType Len);
	inline LengthType GetLen(void);
};
typedef PktBase* LPPktBase;


// �������� �����ؼ� ���� �� ����ϴ� ��Ŷ ���.
struct PktBlockCompressedBase
{
public:

    unsigned short  GetLength() const { return m_usLength; }
    unsigned char   GetCmd()    const { return m_cCommand; }
    unsigned char   GetExtra()  const { return m_cExtra;   }

    void InitPtHead(unsigned short usLength, unsigned char cCmd, unsigned char cExtra)
    {
        m_usLength = usLength;
        m_cCommand = cCmd;
        m_cExtra = cExtra;
    }

protected:

    unsigned short  m_usLength;
    unsigned char   m_cCommand;
    unsigned char   m_cExtra;
};

#pragma pack()


const unsigned int	PktBaseSize = sizeof(PktBase);
const PktBase::StartType	StartBit	= 0xFF;		// ���� ��Ʈ
const PktBase::LengthType	Crypt		= 0x8000;	// ��ȣȭ ���� - 1000B
const PktBase::LengthType	Compress	= 0x4000;	// ���� ���� - 0100B
const PktBase::LengthType	LengthHdr	= 0xC000;	// 1100B
const PktBase::LengthType	LengthMask	= 0x3FFF;	// ���� ���� ���� ����ũ ��. ���� 14��Ʈ. �ִ� 16383byte.
const PktBase::LengthType	PktMinLen	= sizeof(PktBase);
const PktBase::LengthType	PktMaxLen	= 16383;

const PktBase::LengthType	MIN_PACKET_LEN	= sizeof(PktBase);
const PktBase::LengthType	MAX_PACKET_LEN	= 16383;

inline bool PktBase::IsCrypt(void)		{ return ((m_Len & Crypt) == Crypt) ? true : false; }
inline void PktBase::SetCrypt(void)		{ m_Len |= Crypt; }

inline bool PktBase::IsCompress(void)	{ return ((m_Len & Compress) == Compress) ? true : false; }
inline void PktBase::SetCompress(void)	{ m_Len |= Compress; }
inline PktBase::LengthType PktBase::GetUncompressLen(void) { return PktMaxLen; }

inline void PktBase::SetLen(PktBase::LengthType Len) { m_Len = (m_Len & LengthHdr) | Len; }
inline PktBase::LengthType PktBase::GetLen(void)	 { return m_Len & LengthMask; }



//Interface/////////////////////////////////////////////////////////////////////////////////////
//
// ��Ŷ �ش� �ʱ�ȭ
//
// Parameter :
//	1st : ����[In]
//	2st : Ŀ�ǵ�[In]
//	3st : ���� ����[In]
//	4st : ����[In]
//
// Return :
//
///////////////////////////////////////////////////////////////////////////////////////////////
inline void PktBase::InitPtHead(unsigned short Len_In, unsigned char Cmd_In, 
                                unsigned short State_In, unsigned short Error_In)
{
	m_StartBit						= StartBit;
	m_Cmd							= Cmd_In;
	m_Len							= Len_In;	
	m_CodePage						= 0;
	m_SrvInfo.SrvState.wSrvState	= State_In;
	m_SrvInfo.SrvState.wError		= Error_In;	
}

inline void PktBase::InitPtHead(unsigned short Len_In, unsigned char Cmd_In, unsigned long Tick_In)
{
	m_StartBit						= StartBit;
	m_Cmd							= Cmd_In;
	m_Len							= Len_In;	
	m_CodePage						= 0;
	m_SrvInfo.dwServerInfo			= Tick_In;
}


#endif