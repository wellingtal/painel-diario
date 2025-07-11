#ifndef _DBAGENT_SERVER_GAME_DISPATCH_H_
#define _DBAGENT_SERVER_GAME_DISPATCH_H_

#include <Network/Dispatch/RylServerDispatch.h>
#include <Network/Dispatch/MultiDispatchStorage.h>

#include <Item/ItemSerialMgr.h>

// forward decl.
struct PktSL;
struct PktBase;
struct PktUK;
struct PktAdmin;

namespace DBAgent
{
    class CGameDispatch : public CRylServerDispatch
    {
    public:

        static CMultiDispatch& GetDispatchTable();

        enum Const
        {
			// edith 2008.03.04 CGameDispatch ����
            MAX_PACKET_DISPATCH_PER_PULSE   = 100,
            MAX_STREAM_BUFFER_SIZE          = 16000
        };

        CGameDispatch(CSession& Session);
        virtual ~CGameDispatch();

        virtual void Connected();
        virtual void Disconnected();
        virtual bool DispatchPacket(PktBase* lpPktBase);
        
        void IncCharNum(int nRace);
        void DecCharNum(int nRace);

        int GetCharNum(int nRace) const;    // �׻� �� �ο������� �Ѹ��� ����.
        int GetCharNum() const;             // �׻� �� �ο������� �Ѹ��� ����.

        unsigned long GetServerID() const { return m_dwServerID; }

    private:

        bool ServerLogin(PktBase* lpPktBase);
        bool ServerLogout(PktBase* lpPktBase);

        bool UserKillAck(PktBase* lpPktBase);
        bool CharAdminCmd(PktBase* lpPktBase);
        bool GiveItemToTempInven(PktBase* lpPktBase);
        bool ChangeName(PktBase* lpPktBase);

        Item::CItemSerialMgr    m_GameItemSerialMgr;

        // �ο����� �׻� �Ѹ��� ���� �����ϰ�, �����ٶ��� 1�� ���� �����ش�.
        // �� ������ ä���� ���� ���θ� �ο����� 0���� �ƴ����� �ľ��ϱ� �����̴�.
        int m_nHumanNum;    
        int m_nAkhanNum;        

        unsigned long m_dwServerID;
    };

    
    // ��ü ĳ���� ���� ������ �Լ����̴�. CMultiDispatch::Process �� ����Ѵ�.
    class CGetTotalCount
    {
    public:

        CGetTotalCount(unsigned short& usHumanCount, unsigned short& usAkhanCount, 
            unsigned char& cChannelCount);
        bool operator () (unsigned long dwServerID, CPacketDispatch& packetDispatch);

    private:

        unsigned short& m_usHumanCount;
        unsigned short& m_usAkhanCount;
        unsigned char&  m_cChannelCount;
    };

    // ���� ĳ���� �� �ۼ�Ƽ���� ������ �Լ����̴�. CMultiDispatch::Process �� ����Ѵ�.
    class CUserPercentageInZone
    {
    public:

        CUserPercentageInZone(unsigned short* lpChannelUserNum,
            unsigned char cZone, unsigned char cChannelNum);
        bool operator () (unsigned long dwServerID, CPacketDispatch& packetDispatch);

    private:

        int             m_nMaxUserNum;
        unsigned short* m_lpChannelUserNum;
        unsigned char   m_cZone;
        unsigned char   m_cChannelNum;
    };

    // Ư�� ���� ��� ä�ο� ��Ŷ�� ������.
    class CSendPacketToZone
    {
    public:

        CSendPacketToZone(const char* szData, unsigned long dwDataLen, 
            unsigned char cPacketCmd, unsigned char cZone);
        bool operator () (unsigned long dwServerID, CPacketDispatch& packetDispatch);

    private:

        const char*     m_szData;
        unsigned long   m_dwDataLen;
        unsigned char   m_cPacketCmd;
        unsigned char   m_cZone;
    };

}

#endif