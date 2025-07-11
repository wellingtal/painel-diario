#ifndef _CHAR_MOVE_PACKET_H_
#define _CHAR_MOVE_PACKET_H_

// CharMovePacket.h
#include <winsock2.h>
#include <windows.h>
#include <Network/Packet/PacketBase.h>
#include <DB/DBdefine.h>
#include <cmath>

#pragma pack(1)

class CNetworkPos
{
private:

    static const float  ms_fToRadian;
    static const float  ms_fToAngle;
    static const float  ms_fPositionDetail;
    static const float  ms_fVelocityDetail;

    unsigned short  m_usXPos;
    unsigned short  m_usYPos;
    unsigned short  m_usZPos;
    unsigned char   m_cDirection;   //  ���е� - 0~180 (1�� 2��)
    unsigned char   m_cVelocity;    //  �ӵ�/������(1�����ӿ� ������ �� �ִ� �ִ�Ÿ�.)
                                    //  �ӵ� = �ʴ� �̵��Ÿ� = 1~5����.
                                    //  ������ = �ִ� 60������.
                                    //  �ִ� ������ �ӵ�/������ * 1000 = 0 ~ 255
                                    //  �ӵ�/������ = 0 ~ 0.255����
    // TODO : maximum 10meter/sec    

public:

    CNetworkPos() : m_usXPos(0), m_usYPos(0), m_usZPos(0), m_cDirection(0), m_cVelocity(0) { }
    CNetworkPos(float fXPos, float fYPos, float fZPos, float fDir = 0.0f, float fVel = 0.0f) { Initialize(fXPos, fYPos, fZPos, fDir, fVel); }
    CNetworkPos(const POS& pos, float fDir = 0.0f, float fVel = 0.0f) { Initialize(pos.fPointX, pos.fPointY, pos.fPointZ, fDir, fVel); }

    void Initialize(float fXPos, float fYPos, float fZPos, float fDir, float fVel);

    float GetXPos() const { return static_cast<float>(m_usXPos) / ms_fPositionDetail; }
    float GetYPos() const { return static_cast<float>(m_usYPos) / ms_fPositionDetail; }
    float GetZPos() const { return static_cast<float>(m_usZPos) / ms_fPositionDetail; }

    float GetDirection() const { return static_cast<float>(m_cDirection) * 2 * ms_fToRadian; }
    float GetVelocity()  const { return static_cast<float>(m_cVelocity) / ms_fVelocityDetail; }

    float GetSquaredRange(const CNetworkPos& lhs) const
    {
        const float xDiff = GetXPos() - lhs.GetXPos();
        const float yDiff = GetYPos() - lhs.GetYPos();
        const float zDiff = GetZPos() - lhs.GetZPos();
        
        return (xDiff * xDiff) + (yDiff * yDiff) + (zDiff * zDiff);
    }

    float GetRange(const CNetworkPos& lhs) const { return std::sqrtf(GetSquaredRange(lhs)); }

    inline bool operator == (const CNetworkPos& lhs)
    {
        return (m_usXPos == lhs.m_usXPos && m_usYPos == lhs.m_usYPos && m_usZPos == lhs.m_usZPos
            && m_cDirection == lhs.m_cDirection && m_cVelocity == lhs.m_cVelocity);
    }

    inline bool operator != (const CNetworkPos& lhs) { return !(*this == lhs); }
};

inline void CNetworkPos::Initialize(float fXPos, float fYPos, float fZPos, float fDir, float fVel)
{
	float fbuf;
	fbuf = fXPos * ms_fPositionDetail;
	m_usXPos = static_cast<unsigned short>(fbuf);
	fbuf = fYPos * ms_fPositionDetail;
	m_usYPos = static_cast<unsigned short>(fbuf);
	fbuf = fZPos * ms_fPositionDetail;
	m_usZPos = static_cast<unsigned short>(fbuf);

	/*
	// float�� �Ҽ��� ���꿡 ������ �־� �ݿø��մϴ�.
    m_usXPos        = static_cast<unsigned short>(fXPos * ms_fPositionDetail + 0.5f);
    m_usYPos        = static_cast<unsigned short>(fYPos * ms_fPositionDetail + 0.5f);
    m_usZPos        = static_cast<unsigned short>(fZPos * ms_fPositionDetail + 0.5f);
	*/

    float fAngle = fDir * ms_fToAngle;
    while (360 <= fAngle) { fAngle -= 360; }

    m_cDirection    = static_cast<unsigned char>(fAngle/2);

    unsigned int nVelocity = static_cast<unsigned int>(fVel * ms_fVelocityDetail);
    if(255 < nVelocity) { nVelocity = 255; }

    m_cVelocity     = static_cast<unsigned char>(nVelocity);
}


// ĳ���� �̵� ��Ŷ Ex (CharMove, TCP)
typedef struct PktMVEx* LPPktMVEx;
struct PktMVEx : public PktBase
{
    // CID�� PktBase�� dwServerInfo�� ��´�.
    CNetworkPos     m_NetworkPos;    
    unsigned char   m_cUAct;
    unsigned char   m_cLAct;
};

/*
    Deplicated

// ĳ���� �̵� ��Ŷ	(CharMove, UDP)
typedef struct PktMV* LPPktMV;
struct PktMV : public PktBase
{
	POS				m_Position;     // ĳ���� ��ġ
	float			m_fDir;         // ĳ���� ����
	unsigned long	m_dwCharID;     // ĳ���� ID
    unsigned long	m_dwChantEf;	// Chant Effect
	unsigned long	m_dwEnchantEf;  // Enchant Effect
	unsigned short	m_wUAct;        // �׼� ��ȣ
	unsigned short	m_wLAct;        // �׼� ��ȣ
	unsigned char	m_wLevel;       // ĳ���� ����
};

// ĳ���� �̵� ������Ʈ ��Ŷ	( Char Move Update )
typedef struct PktMU* LPPktMU;
struct PktMU : public PktBase 
{
	POS				m_Position;     // ��ġ
   	float			m_fDir;         // ����
	unsigned long	m_dwTick;       // Ŭ���̾�Ʈ ƽ
	bool			m_bSitMode;     // �ɱ� (true = �ɾ� ����.)
};

// ĳ���� �̵� ������Ʈ ��Ŷ Ack	( Char Move Update )
typedef struct PktMUAck* LPPktMUAck;
struct PktMUAck : public PktBase
{
	unsigned long	m_dwTick;       // Ŭ���̾�Ʈ ƽ (ó�� �ð� ���� ����)
	unsigned char	m_cFlag;        //	- �÷��� (0 = �̵�, 1 = ���̵�)
};
*/


// ��ȯ
typedef struct PktSummon* LPPktSummon;
struct PktSummon : public PktBase
{
	unsigned long	m_dwCharID;		// ĳ���� ID
	unsigned long	m_dwTargetID;	// ��� ID
	CNetworkPos		m_NetworkPos;	// ��Ʈ��ũ ��ġ
};

// ���� ������	( Monster Move )
typedef struct PktMM* LPPktMM;
struct PktMM : public PktBase
{
    unsigned long	m_dwMonID;      // ���� ���̵�
    CNetworkPos     m_NetworkPos;   // ��Ʈ��ũ ��ġ
    unsigned char	m_cAct;         // ���� �ൿ
	unsigned char	m_cAniNum;      // ���� �̵� Ƚ��
};


namespace CellCommand
{
    enum Type
    {
        CELL_MOVE   = 0,
        CELL_LOGIN  = 1,
        RESPAWN     = 2
    };
};

// ĳ���� �� �α��� ��Ŷ	(Char Cell Login)
typedef struct PktCCLi* LPPktCCLi;
struct PktCCLi : public PktBase
{
	unsigned char	m_cCmd;             // see namespace CellCommand
    // ��Ŷ �ڿ� BroadcastInfo::CharInfo::SLoginPhase�� �پ� �´�.
};

// ĳ���� �� �α׾ƿ� ��Ŷ	( Char Cell Logout )
typedef struct PktCCLo* LPPktCCLo;
struct PktCCLo : public PktBase
{
	unsigned long	m_dwCharID;
	unsigned char	m_cCmd;             // see namespace CellCommand
};

// �� ��ε� ĳ���� + Address����ü
typedef struct PktCB PktCB, *LPPktCB;
struct PktCB : public PktBase 
{
    unsigned char   m_cCharacterPhase;  // �� ��ε�ĳ���� Phase(ĳ����).
	unsigned short	m_usBufferSize;     // �� ���� ũ��
};				

#pragma pack()

#endif