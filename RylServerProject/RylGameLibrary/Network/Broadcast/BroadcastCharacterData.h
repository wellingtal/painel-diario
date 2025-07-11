#ifndef _BROADCAST_2ND_CHARACTER_DATA_H_
#define _BROADCAST_2ND_CHARACTER_DATA_H_

#include <winsock2.h>
#include <windows.h>

#include <Network/Packet/PacketStruct/CharMovePacket.h>
#include <Item/ItemStructure.h>

//! �� ������ ����/Ŭ���̾�Ʈ�� ���� ����ϴ� �����̴�.
//! ĳ������ ������ �� Ŭ���̾�Ʈ�� �� �˾ƾ� �� ������� ��� �ִ�.

namespace Broadcast2nd
{
	inline void COPY_AND_ADVANCE_SRC(void* dst, const char*& src, size_t size)
	{
		memcpy(dst, src, size); src += size;
	}

	inline void COPY_AND_ADVANCE_DST(char*& dst, const void* src, size_t size)
	{
		memcpy(dst, src, size); dst += size;
	}

	enum DeltaInfo
	{
		// CID          : 4byte
		// UpdateFlag   : 4byte

		CHAR_DELTA_EQUIPMENT     	= 1 <<  0,		// ��� ���� (0~7��Ʈ�� ����Ѵ�.)			32byte
		CHAR_DELTA_CHAR_NAME     	= 1 <<  8,		// ĳ���� �̸�                              16byte
		CHAR_DELTA_STALL_NAME    	= 1 <<  9,		// ������ �̸�                              32byte
		CHAR_DELTA_GUILD_ID      	= 1 << 10,  	// ��� ID, �뺴                            5byte
		CHAR_DELTA_PARTY_ID      	= 1 << 11,  	// ��Ƽ ID									4byte
		CHAR_DELTA_FAME          	= 1 << 12,  	// ��, ���ϸ���                           8byte
		CHAR_DELTA_BODYINFO      	= 1 << 13,  	// �� ���� ���� ������Ʈ                    4byte
		CHAR_DELTA_CLASS         	= 1 << 14,  	// Ŭ���� ����                              1byte
		CHAR_DELTA_LEVEL         	= 1 << 15,  	// ���� ����                                1byte
		CHAR_DELTA_ENCHANT_INFO  	= 1 << 16,  	// æƮ/��æƮ ����(16~19��Ʈ�� ����Ѵ�.)	16byte
		CHAR_DELTA_NETWORK_POS   	= 1 << 20,  	// ĳ���� ��ǥ (x,y,z,����,�̵��Ÿ�)        8byte
		CHAR_DELTA_HP            	= 1 << 21,  	// ĳ���� HP                                2byte
		CHAR_DELTA_EQUIP_GRADE		= 1 << 22,		// ��� �׷��̵�(�Ӹ�, ��, ������, �޼�)	4byte
		CHAR_DELTA_WAR_FLAG			= 1 << 23,		// �����, ������ �÷���					2byte
		CHAR_DELTA_REALM_POINT		= 1 << 24		// �������� �������� ����Ʈ.				1byte.
        
		// Total : 144Byte
	};

    enum MonsterDelta
    {
        MON_DELTA_ENCHANT_INFO  = 1 <<  0,  // ���� ��æƮ ���� (0~3��Ʈ�� ����Ѵ�)  4*4byte
        MON_DELTA_NETWORK_POS   = 1 <<  4,  // ���� ��ǥ ����                         8byte
        MON_DELTA_HP            = 1 <<  5,  // ���� hp����                            2byte
        MON_DELTA_MASTERCID     = 1 <<  6	// ��ȯ ������ ������ CID                 4byte
    };


	//! �⺻ �� ����
	struct bcBodyInfo
	{
		unsigned long   m_cHair         : 4;    //! �Ӹ�Į
		unsigned long   m_cFace         : 4;    //! ��
		unsigned long   m_cNationality  : 3;    //! ����        
		unsigned long   m_cAdminLevel   : 3;    //! ��� ���� ( 0 : �Ϲ�, 1���� ũ�� ���..)
		unsigned long   m_cRace         : 2;    //! ����
		unsigned long   m_cSex          : 3;    //! 0 : ����,     1 : ����
		unsigned long   m_bPeaceMode    : 1;    //! 0 : ������, 1 : ��ȭ���
		unsigned long   m_cHandPos      : 1;    //! 0 : 1�� ��,   1 : 2�� ��
		unsigned long   m_cRideFlag     : 1;    //! 0 : x,   1 : ��Ž
		unsigned long   m_cReserved     : 12;   //! ��Ÿ �÷���
	};

	//! �⺻ �������ͽ� ����
	struct bcStatusInfo
	{
		union
		{
			bcBodyInfo      m_BodyInfo;
			unsigned long   m_dwBodyInfo;
		};

		unsigned char   m_cClass;       // Ŭ����
		unsigned char   m_cLevel;       // ����
	};

	//! ������ ����
	struct StallNode
	{
		enum
		{
			MAX_STALL_NAME = 32
		};

		unsigned long   m_dwCID;
		char            m_szStallName[MAX_STALL_NAME];
	};

	//! ��Ƽ, ���, ��, ���ϸ��� ���� ����
	struct CommunityInfo
	{
		unsigned long   m_dwGuild_ID;
		unsigned long   m_dwParty_ID;
		unsigned char	m_cTacticsFlag;

		unsigned long   m_dwFame;
		unsigned long   m_dwMileage;
	};

	//! ����Ʈ ó���� ����� �׷��̵�
	class CNetworkEquipGrade
	{
	public:

		CNetworkEquipGrade()
			:	m_cHead(0), m_cBody(0), m_cRightHand(0), m_cLeftHand(0)
		{ }

		CNetworkEquipGrade(unsigned char cHead, unsigned char cBody, unsigned char cRightHand, unsigned char cLeftHand)
			:	m_cHead(cHead), m_cBody(cBody), m_cRightHand(cRightHand), m_cLeftHand(cLeftHand)
		{ }

		inline bool operator != (const CNetworkEquipGrade& rhs)
		{
			return (m_cHead != rhs.m_cHead || m_cBody != rhs.m_cBody || 
				m_cRightHand != rhs.m_cRightHand || m_cLeftHand != rhs.m_cLeftHand);
		}

		inline const unsigned char GetHead(void)		{ return m_cHead;		}
		inline const unsigned char GetBody(void)		{ return m_cBody;		}
		inline const unsigned char GetRightHand(void)	{ return m_cRightHand;	}
		inline const unsigned char GetLeftHand(void)	{ return m_cLeftHand;	}

		static const unsigned char TransferNetworkEquipGrade(Item::CItem* lpItem);

	private:

		unsigned short	m_cHead			: 4;
		unsigned short	m_cBody			: 4;
		unsigned short	m_cRightHand	: 4;
		unsigned short	m_cLeftHand		: 4;
	};

	//! ����� ������ ���� �÷��� ����
	struct WarFlagInfo
	{
		unsigned char	m_cGuildWarFlag;
		unsigned char	m_cRealmWarFlag;
	};

	enum DataFlag
	{
		CHARDATA_CHANGED    = (1 << 0)        
	};


    class CAggresiveCreatureData
    {
    public:

        CAggresiveCreatureData();
        virtual ~CAggresiveCreatureData();

		const unsigned long  GetCID()         const { return m_dwCID; }
		const EnchantInfo&	 GetEnchantInfo() const	{ return m_EnchantInfo;		}
		const CNetworkPos    GetNetworkPos()  const	{ return m_NetworkPos;		}
		const unsigned short GetHP()          const { return m_usHP;			}

		void SetEnchantInfo(const EnchantInfo& enchantInfo);
		void SetEnchantInfo(unsigned char cIndex, unsigned long dwFlag)	{ m_EnchantInfo.m_dwStatusFlag[cIndex] = dwFlag;	}
        void SetNetworkPos(const CNetworkPos& networkPos)				{ m_NetworkPos = networkPos;						}
		void SetHP(unsigned short usHP)                                 { m_usHP = usHP; }

		void SetDataFlag(unsigned long dwDataFlag)   { m_dwDataFlag |= dwDataFlag; }
		bool IsSetDataFlag(unsigned long dwDataFlag) { return 0 != (m_dwDataFlag & dwDataFlag); }
		void ResetDataFlag(unsigned long dwDataFlag) { m_dwDataFlag &= ~dwDataFlag; }
		void ClearDataFlag() { m_dwDataFlag = 0; }

		unsigned long GetDataFlag() const { return m_dwDataFlag; }

		const unsigned long GetLastUpdatedPulse() const { return m_dwLastUpdatedPulse; }

		//! ���ۿ� ���� ���̸� ���ڷ� �޾Ƽ� �����͸� ������Ʈ�Ѵ�.
		//! ���Ͻÿ� ���� ���̿�, ����� ���� ���̸� �־� �ش�.
		//! ���ϰ��� ����/���� ���θ� �����Ѵ�.
		virtual bool UpdateData(unsigned long dwCID, unsigned long dwCurrentPulse, 
			const char* szData, int& nBufferSize_InOut) = 0;

        virtual void ClearData();

    protected:

        unsigned long   m_dwCID;
		unsigned long   m_dwLastUpdatedPulse;
		unsigned long   m_dwDataFlag;

		EnchantInfo		m_EnchantInfo;
		CNetworkPos     m_NetworkPos;
		unsigned short  m_usHP;
    };


    // ĳ���� ������ Ŭ����
    class CCharacterData : public CAggresiveCreatureData
    {
    public:

		enum 
		{
			MAX_NAME		= 16,
			HALF_EQUIP_POS	= 8
		};

		CCharacterData();
		virtual ~CCharacterData();

		const char* GetName()      const { return m_szName;      }
		const char* GetStallName() const { return m_szStallName; }

		void GetEquipmentsID(unsigned short* lpEquipmentsID_Out, unsigned char nIDNum) const;

		const unsigned long GetGuildID() const		{ return m_CommunityInfo.m_dwGuild_ID;		}
		const unsigned long GetPartyID() const      { return m_CommunityInfo.m_dwParty_ID;		}
		
		const unsigned long GetFame()    const      { return m_CommunityInfo.m_dwFame;			}
		const unsigned long GetMileage() const      { return m_CommunityInfo.m_dwMileage;		}		

		const unsigned long	GetBodyInfo() const     { return m_StatusInfo.m_dwBodyInfo;			}

		const CNetworkEquipGrade	GetNetworkEquipGrade() const	{ return m_NetworkEquipGrade;	}

		const unsigned char	GetGuildWarFlag() const	{ return m_WarFlagInfo.m_cGuildWarFlag;		}
		const unsigned char	GetRealmWarFlag() const	{ return m_WarFlagInfo.m_cRealmWarFlag;		}
		const unsigned char GetTacticsFlag() const	{ return m_CommunityInfo.m_cTacticsFlag;	}

		const unsigned char GetAdminLevel() const   { return m_StatusInfo.m_BodyInfo.m_cAdminLevel; 	}
		const unsigned char GetClassType()  const   { return m_StatusInfo.m_cClass;   	 				}
		const unsigned char GetLevel()  const       { return m_StatusInfo.m_cLevel;   	 				}
		const unsigned char GetRace()   const       { return m_StatusInfo.m_BodyInfo.m_cRace;			}
		const unsigned char GetNation() const       { return m_StatusInfo.m_BodyInfo.m_cNationality;	}
		const unsigned char GetFace()   const       { return m_StatusInfo.m_BodyInfo.m_cFace;       	}
		const unsigned char GetHair()   const       { return m_StatusInfo.m_BodyInfo.m_cHair;       	}        
		const unsigned char GetSex()    const       { return static_cast<unsigned char>(m_StatusInfo.m_BodyInfo.m_cSex) + 1; }

		const unsigned char GetHandPos()  const     { return m_StatusInfo.m_BodyInfo.m_cHandPos;    }
		const unsigned char GetRideFlag()  const     { return m_StatusInfo.m_BodyInfo.m_cRideFlag;    }
		const bool          IsPeaceMode() const     { return m_StatusInfo.m_BodyInfo.m_bPeaceMode;  }
		const bool          IsAdmin()     const     { return (0 != m_StatusInfo.m_BodyInfo.m_cAdminLevel); }

		const unsigned char GetRealmPoint() const	{ return m_cRealmPoint;								}

		void SetEquipmentsID(unsigned short* lpEquipmentsID_In, const unsigned char nIDNum);
		void SetEquipmentsID(unsigned long dwEquipmentsID_In, const unsigned char nIDIndex);
		void SetName(const char* szName, const size_t nNameLen);
		void SetStallName(const char* szStallName, const size_t nStallNameLen);

        void SetGuildID(unsigned long dwGID)		{ m_CommunityInfo.m_dwGuild_ID		= dwGID;    }
        void SetPartyID(unsigned long dwPID)        { m_CommunityInfo.m_dwParty_ID		= dwPID;    }
        void SetFame(unsigned long dwFame)          { m_CommunityInfo.m_dwFame			= dwFame;   }
        void SetMileage(unsigned long dwMileage)    { m_CommunityInfo.m_dwMileage		= dwMileage;}
				
		void SetBodyInfo(unsigned long dwBodyInfo)	{ m_StatusInfo.m_dwBodyInfo = dwBodyInfo;		}

		void SetNetworkEquipGrade(const CNetworkEquipGrade& equipGrade)	{ m_NetworkEquipGrade = equipGrade;	}

		void SetGuildWarFlag(unsigned char cFlag)	{ m_WarFlagInfo.m_cGuildWarFlag = cFlag;		}
		void SetRealmWarFlag(unsigned char cFlag)	{ m_WarFlagInfo.m_cRealmWarFlag = cFlag;		}
		void SetTacticsFlag(unsigned char cFlag)	{ m_CommunityInfo.m_cTacticsFlag = cFlag;		}

		void SetClass(unsigned char cClass)         { m_StatusInfo.m_cClass = cClass; 				}
		void SetLevel(unsigned char cLevel)         { m_StatusInfo.m_cLevel = cLevel; 				}

		void SetRealmPoint(unsigned char cRealmPoint)	{ m_cRealmPoint = cRealmPoint;				}

		void ClearData();

		bool UpdateData(unsigned long dwCID, unsigned long dwCurrentPulse, 
			const char* szData, int& nBufferSize_InOut);

		static int EstimateBufferSize(unsigned long dwUpdateFlag);

	protected:

		char            	m_szName[MAX_NAME];
		char            	m_szStallName[StallNode::MAX_STALL_NAME];
		unsigned long		m_dwDoubleEquipsID[HALF_EQUIP_POS];			// ��� ������ 2�� ������ ó���Ѵ�.

		CommunityInfo   	m_CommunityInfo;
		bcStatusInfo    	m_StatusInfo;
		CNetworkEquipGrade	m_NetworkEquipGrade;
		WarFlagInfo			m_WarFlagInfo;

		unsigned char		m_cRealmPoint;
	};

    class CMonsterData : public CAggresiveCreatureData
    {
    public:

        CMonsterData();
        virtual ~CMonsterData();

        void ClearData();

		bool UpdateData(unsigned long dwCID, unsigned long dwCurrentPulse, 
			const char* szData, int& nBufferSize_InOut);

		static int EstimateBufferSize(unsigned char cUpdateFlag);
        
    protected:

        unsigned long m_dwMasterCID; // ��ȯ�� ������ ���....
    };
}

#endif