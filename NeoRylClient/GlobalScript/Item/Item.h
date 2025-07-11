#ifndef _CITEM_CLASS_H_
#define _CITEM_CLASS_H_

#include <limits>
#include <vector>
#include <algorithm>
#include <Network/Packet/PacketStruct/CharItemPacketStruct.h>

#include "ItemMgr.h"

#include "ItemStructure.h"

// ���� ����
class CCharacter;

#define DELITEMLOG(x)       (void*)0
//#define DELITEMLOG(x)     x

namespace Item
{
    class CItem
    {
    public:

        CItem();
        CItem(const ItemInfo& itemInfo);  

        // ��ũ��Ʈ�κ��� �ε��� ������ �����͸� ��� �´�.
        const ItemInfo& GetItemInfo() const { return m_ItemInfo; }

        // Desc : ������ ������ char���۷� ����Ѵ�.
        // In   : ����, ���� ũ��
        // Out  : (Return)���� ����, nBufferLength_InOut - ����� ũ��
        virtual bool SerializeOut(char* lpSerializeItem_Out, size_t& nBufferLength_InOut);

        // Desc : char���۷κ��� ������ ������ �о� �´�.
        // In   : ����, ���� ũ��
        // Out  : (Return)���� ����, nBufferLength_InOut - ����� ���� ũ��
        virtual bool SerializeIn(const char* lpSerializeItem_In, size_t& nBufferLength_InOut);

        inline void MoveItem(ItemPos itemPos);

        inline unsigned __int64 GetUID(void) const			{ return m_ItemData.m_dwUID;						}
        inline ItemPos          GetPos(void) const			{ return m_ItemData.m_ItemPos;						}
        inline ItemPos          GetRealPos(void) const		{ return m_itemPos_Real;							}
		inline void				SwapPos(void)				{ std::swap(m_ItemData.m_ItemPos, m_itemPos_Real);	}

		inline unsigned short   GetPrototypeID(void) const							{ return m_ItemData.m_usProtoTypeID;				}
        inline unsigned char    GetNumOrDurability(void) const						{ return m_ItemData.m_cNumOrDurability;				}
        inline unsigned char    GetMaxNumOrDurability(void) const					{ return m_cMaxNumOrDurability;						}
        inline void             SetNumOrDurability(unsigned char cNumOrDurability)	{ m_ItemData.m_cNumOrDurability = cNumOrDurability; }      
        inline void             SetMaxNumOrDurability(unsigned char cNumOrDurability){ m_cMaxNumOrDurability = cNumOrDurability;		}
		inline void				SetStallPrice(unsigned long dwStallPrice)			{ m_dwStallPrice = dwStallPrice;					}

        inline unsigned long    GetStallPrice(void) const		{ return m_dwStallPrice;										}
		inline unsigned long	GetBuyBlackPrice(void) const	{ return m_ItemInfo.m_DetailData.m_dwBlackPrice;				}	
		inline unsigned long	GetBuyMedalPrice(void) const	{ return m_ItemInfo.m_DetailData.m_dwMedalPrice;				}	
		
		virtual unsigned long	GetBuyPrice(void) const;			
        virtual unsigned long	GetSellPrice(void) const;	
        virtual unsigned long	GetRepairPrice(void) const;		
		virtual unsigned long	GetUpgradePrice(void) const;
		virtual unsigned long	GetGraftPrice(void) const;

		inline const char*		GetItemTypeName(void) const		{ return m_ItemInfo.m_StringData.m_szTypeName;		}

        inline bool IsSet(unsigned long dwDetailDataFlag) const { return dwDetailDataFlag == (m_ItemInfo.m_DetailData.m_dwFlags & dwDetailDataFlag); }

		inline bool		GetEnableRepair(void);
		inline bool 	GetEnableStack(void);
		inline bool 	GetSocketItem(void);
		inline bool		GetRuneItem(void);
		inline bool 	GetTwohandItem(void);
		inline bool 	GetEnableEquip(void);
                
		inline void				SetItemPos(unsigned short wItemPos)		{ m_ItemData.m_ItemPos.m_cPos = wItemPos;	}
		inline unsigned short	GetItemPos(void)						{ return m_ItemData.m_ItemPos.m_cPos;		}

        inline void		SetPos(unsigned char cXIndex, unsigned char cYIndex, unsigned char cZIndex) { m_ItemData.m_ItemPos.SetPos(cXIndex, cYIndex, cZIndex); }

		inline void		SetXIndex(unsigned char cXIndex)		{ m_ItemData.m_ItemPos.SetXIndex(cXIndex);			}
		inline void		SetYIndex(unsigned char cYIndex)		{ m_ItemData.m_ItemPos.SetYIndex(cYIndex);			}
		inline void		SetZIndex(unsigned char cZIndex)		{ m_ItemData.m_ItemPos.SetZIndex(cZIndex);			}

		inline void		SetPos(unsigned short wIndex)			{ m_ItemData.m_ItemPos.SetPos(wIndex);				}
	
        // SetUID�� �ƹ����� �Ժη� ������� ���ÿ�!
        inline void     SetUID(unsigned __int64 dwItemUID) { m_ItemData.m_dwUID = dwItemUID; }

		// S �׷��̵� ���Ѱ� ����(S �׷��̵� �̿ܿ��� ���� ������. ������ ��Ȯ���� �ʽ��ϴ�. - by hackermz)
		virtual void	RevisionLimit() { }
		virtual void	RevisionLimit(short* lpattrLimit) { }
		virtual void	RevisionLimit(Item::Attribute::Type eAttributeType, short& attrLImit) { }


	protected:

        virtual ~CItem();
        
        const ItemInfo& m_ItemInfo;             // ��ũ��Ʈ�κ��� �ε��� ������ ������
        ItemData        m_ItemData;             // DB�� ����Ǵ� ������ ������

        ItemPos         m_itemPos_Real;         // �ӽ� ������ �ƴ� ������ ��ġ
        unsigned char   m_cMaxNumOrDurability;  // �ִ� ������
		unsigned long	m_dwStallPrice;			// �����󿡼��� ����
        unsigned long   m_dwPrice;              // ������ ���� (�ɼǿ� ���� ���ŵ����� �������� ��������� ���� ��)

        friend class CItemFactory;
    };


    class CEquipment : public CItem
    {
    public:

        enum EQUIPMENT_ERROR
        {
            // ���� �ڱ�
            S_SUCCESS                       = 0,    // ����
	        E_SERVER_ERROR                  = 1,    // ���� ����
	        E_GET_EQUIP_FAILED              = 2,    // ��� ��� ����
	        E_GET_GEM_FAILED                = 3,    // �� ��� ����
	        E_NOT_EQUIP                     = 4,    // ���׷��̵� ������ ������ �ƴ�
	        E_NOT_GEM                       = 5,    // �� ������ �ƴ�
	        E_REMOVE_GEM_FAILED             = 6,    // �� ���� ����
	        E_SOCKET_IS_FULL                = 7,    // ���� ���� �ʰ�
	        E_OVER_MAX_SOCKET               = 8,    // �ִ� ���� �ʰ�
			E_SOCKET_NOT_MAX				= 9,	// ���� ������ 7���� �ƴϴ�.
			E_SOCKET_NOT_INST				= 10,	// ���Ͽ� �ν���� ������ ����.

			// �� ��ġ/����
			R_SUCCESS						= 0,	// �� ��ġ/���� ����.
			E_GET_RUNE_FAILED				= 9,	// �� ������ ������.
			E_NOT_RUNE						= 10,	// ���� ������.
			E_NOT_RUNE_SLOT					= 11,	// �鿡 �� ������ ������.
			E_NOT_RUNE_LEVEL_LIMIT			= 12,	// �鿡 �������Ѷ� ������ ��ġ�� �� ������.
			E_USED_RUNE_SKILL_POINT			= 13,	// �鿡 �߰� ��ų ����Ʈ�� ����ϰ� �־ ���� �� �� ������.
			E_NOT_RUNE_SCRIPT_INFO			= 14,	// �鿡 ��ũ��Ʈ ������ ���ü� ������.

            // ��ȭ 
            E_GET_MINERAL_FAILED            = 3,    // ���� ��� ����
            E_NOT_MINERAL                   = 5,    // ������ �ƴ�
            E_NOT_MATCH_MINERAL_TYPE        = 6,    // ���� Ÿ�� Ʋ��
            E_NOT_ENOUGH_MINERAL            = 7,    // ���� ����
            E_REMOVE_EQUIP_FAILED           = 8,    // ��� ���� ����
            E_CREATE_SCRAP_FAILED           = 9,    // ��ö ���� ����
            E_SET_SCRAP_FAILED              = 10,   // ��ö ���� ����
            E_REMOVE_MINERAL_FAILED         = 11,   // ���� ���� ����
            E_UPGRADE_LEVEL_IS_FULL         = 12,   // �� �̻� ���׷��̵带 �� �� ����         
            E_NOT_ENOUGH_MONEY              = 13,   // ���� �����ؼ� ���׷��̵� �� �� ����
            S_BROKEN_EQUIP                  = 14,	// ���������� ���� �μ��� (Ŭ���̾�Ʈ�� �������� �ʴ´�.)

			E_NOT_UPGRADE_LEVEL				= 15,	// �ɼ� �̽� ������ ���� ���׷��̵� ������ �ƴ�.
			
			E_NOT_UPGRADE_AS_OVER			= 16,	// ����� �� �̿� ȸ�� �ʰ�.
			E_NOT_UPGRADE_ENDURANCE			= 17,	// ������ ȸ�� ������ ��� �Ұ�.
			E_NOT_UPGRADE_MINENDURANCE		= 18,	// ���ð����� ������ 10�� ���� �ʽ��ϴ�.
			E_NOT_UPGRADE_AWAKEN			= 19,	// �̹� �����Ǿ���. (������ �ھ� ���)

			// �ɼ� �̽� 
			E_WRONG_ATTRIBUTE				= 2,	// �̽� �Ұ����� �ɼ�
			E_NOT_EQUAL_KIND				= 3,	// �ٸ� ������ ���
			E_HIGH_GRADE_ORIGINAL			= 4,	// ������ �׷��̵尡 ����
			E_GRAFE_FAILD					= 5,	// �ɼ��̽� ����

			// ���� �Ǹ�
			E_WRONG_TYPE					= 2,	// ���� �Ǹ� ����� �ƴ� 
			E_NEW_SEASON_EQUIP				= 3,	// ���� �����ų� �̹� ����� ���
			E_LOW_STATUS_EQUIP				= 4		// �ʹ� ���� ������ ���
        };

		enum PASSIVE_TYPE
		{
			PASSIVE_ALWAYS				=	0,		// ���⿡ ������� �׻� �߻��ϴ� �нú�
			PASSIVE_THIS_WEAPON			=	1,		// �ش� ���⿡ �߻��ϴ� �нú�
			NONE_PASSIVE_THIS_WEAPON	=	2		// �ش� ���⿡ �߻����� �ʴ� �нú�
		};

		enum RuneApplyType
		{
			RUNE_NONE	=	0,
			RUNE_INDEX	=	(1 << 0),				// �ϳ��� �Ӽ�.
			RUNE_ALL	=	(1 << 1),				// ��� �Ӽ�.
			RUNE_APPLY  =	(1 << 2),				// �Ӽ��� �߰�.
			RUNE_REMOVE =	(1 << 3)				// �Ӽ��� ����.
		};

        // �������� ���� Downcast�Ѵ�.
        inline static CEquipment* DowncastToEquipment(CItem* lpItem);

        // Desc : ������ ������ char���۷� ����Ѵ�.
        // In   : ����, ���� ũ��
        // Out  : (Return)���� ����, nBufferLength_InOut - ����� ũ��
        virtual bool SerializeOut(char* lpSerializeItem_Out, size_t& nBufferLength_InOut);

        // Desc : char���۷κ��� ������ ������ �о� �´�.
        // In   : ����, ���� ũ��
        // Out  : (Return)���� ����, nBufferLength_InOut - ����� ���� ũ��
        virtual bool SerializeIn(const char* lpSerializeItem_In, size_t& nBufferLength_InOut);

        // ������ �Ӽ� �߰� (�Ӽ� ���� �Ŀ��� �ݵ�� ����/�׷��̵� ����� �ٽ� ���ݽô�.)
        inline void AddAttribute(Item::Attribute::Type eAttributeType, short nAttributeValue);
		inline void SetAttribute(Item::Attribute::Type eAttributeType, short nAttributeValue);

		// 3�� �뷱�� ��ġ(����, ���׷��̵� �� ����)0
		inline void RemoveUpgradeAttribute(Item::Attribute::Type eAttributeType);
		inline void RemoveGemAttribute(Item::Attribute::Type eAttributeType);

        // ������ �Ӽ��� ��� �´�.
        inline short GetAttribute(Item::Attribute::Type eAttributeType);
        inline void GetAttribute(short* lpAttributeArray_Out, unsigned short nArraySize);

		// �ɼǺ� �Ѱ谪�� �����ߴ��� üũ�Ѵ�.
		bool CheckAttributeLimit(Item::Attribute::Type eAttributeType, unsigned char cType = 0);

		inline bool AddSocket(unsigned char cSocket);
		inline void GetSocket(unsigned char* cSockets, unsigned char cArraySize);
		unsigned char GetSocket(unsigned char cIndex)		{ return m_cSocket[cIndex];	}

		const Grade::GradeInfo& GetItemGrade(void) const	{ return m_GradeInfo;		}

		inline void InitializeAttribute(void);         // ������ ������ ���������� �ʱ�ȭ
		BOOL AddRandomOption(Item::EquipType::Grade eGrade, unsigned char cBaseNum, int iMagicChance);

		// ���ϼ���
		BOOL AddRandomSocket();

		// �ɼ� �̽�
		EQUIPMENT_ERROR OptionGraft(CEquipment* lpSacrifice, bool bUpgradeLevelLimit, unsigned char cAttributeType, 
			unsigned long dwCurrentGold, unsigned long& dwUsedGold);

		bool CanOptionGraft(Item::CItemType::ArrayType eItemType, Item::Attribute::Type eAttributeType);

		// ���� �Ǹ�
		EQUIPMENT_ERROR Compensation(unsigned char cClass, unsigned char cLevel, 
			CharacterStatus status, CEquipment** lppResultEquip, long& lCompensationGold);

		unsigned char  	GetUpgradeLevel(void)     const { return m_cUpgradeLevel; 	}
		unsigned char  	GetCoreLevel(void)     const { return m_cCoreLevel; 	}
		
		unsigned char  	GetCurrentSocketNum(void) const { return m_cSocketNum;    	}
		unsigned char  	GetMaxSocketNum(void)     const { return m_cMaxSocket;    	}

		// ���뷱�� ���� �����Ǵ� ���� �� �Լ��� ȣ�����ش�.
		void			SetNewEquip(char cStrong = 1)		
		{  
			if(m_cSeasonRecord != cStrong)
				m_cCoreLevel = 0;

			m_cSeasonRecord = cStrong;	// 1 �⺻, 2 ���, 3 ����
		}
		unsigned char	GetSeasonRecord(void)			{ return m_cSeasonRecord;	}

		unsigned long	GetBuyPrice(void) const;			
		unsigned long	GetSellPrice(void) const;	
		unsigned long	GetRepairPrice(void) const;		
		unsigned long	GetUpgradePrice(void) const;
		unsigned long	GetGraftPrice(void) const;

		unsigned char	GetUpgradeCnt(void)							{ return m_cUpgradeCnt;			}
		void			SetUpgradeCnt(unsigned char cUpgradeCnt)	{ m_cUpgradeCnt = cUpgradeCnt;	}

        // ���׷��̵� ���� ���� ����
        bool IsUpgradable(void) const;

		// ���������ۿ� �� ���� ���� üũ
		inline unsigned char	GetMaxRuneSocket();

		// ���� �����ۿ� ���� �������� üũ
		inline unsigned char	GetRuneEquipable();

		// ���� �����ۿ� �������ѿ� ������ ���´�.		
		inline unsigned short	GetRuneEquipLimitLevel(void) const;

		// �� �������� ����(�Ӽ��� ���� ����)
		inline bool				SubRuneAttribute(void);

		// ��ġ �Ǿ� �ִ� �� ���ϰ��� �Ѱ��ش�.
		inline unsigned short	GetRuneSocket(unsigned char cIndex) 
		{ 
			if (cIndex >= EquipmentInfo::MAX_RUNE_SOCKET_NUM) 
			{
				return 0; 
			}
			
			return m_usRuneSocket[cIndex]; 
		}

		// �� �Ӽ��� ������ ������ �Ӽ� ���纻�� �Ѱ��ش�.
		inline void				GetSubRuneAttribute(short* psAttribute);
		inline void				GetSubRuneAttribute(Item::Attribute::Type eAttributeType, short& sAttribute);

		// �ܺο��� ����� �� �Ӽ� ����/�߰�
		inline void				SetRuneAttribute(RuneApplyType enRuneApplyType, unsigned char ucIndex);

		// ���� �����ۿ� �ִ� ��� �� �����ۿ� �Ӽ��� ��ų����Ʈ �������ش�.
		unsigned short			GetAllRuneSkillPointCheck();										

		// �� �������� ���� �Ǿ� ������ ��ų ����Ʈ üũ
		bool					GetRuneCheckSkill(CCharacter* lpCharacter, Item::ItemPos itemPos, bool bChk = true);

		// ------------------------------------------------------------------------------------------------------------
		// ConvertTool������ ���Ǵ� �Լ��� - RebalanceOverItem.cpp ���� ���
		inline void				Repair();
		inline void				InitEquip();
		inline void				SetEquip();

		// ------------------------------------------------------------------------------------------------------------
		// ���������� ���Ǵ� �Լ��� - ItemServerFunc.cpp ���� ���� ����

		EQUIPMENT_ERROR InstallSocket(CItem& Gem);
        EQUIPMENT_ERROR UpgradeItem(CItem& Mineral_InOut, unsigned long dwCurrentGold_In, unsigned long& dwUsedGold_Out);

		EQUIPMENT_ERROR InstallRuneSocket(CItem& Rune, bool bEquip, unsigned char cLevel);									// ���� �����ۿ� �� ���� ��ġ.

		EQUIPMENT_ERROR UnInstallRuneSocket(CItem& Rune, bool bEquip, unsigned char cLevel,	unsigned short usSkillPoint);	// ���� �����ۿ� �� ���� ����.

		// ------------------------------------------------------------------------------------------------------------
		// 3�� �뷱�� ��ġ ���� �Լ�

		// S �׷��̵� ���Ѱ� ���ϱ�
		bool			GetLimitValue(short* lpattrLimit);
		bool			GetLimitValue(Item::Attribute::Type eAttributeType, short& attrLimit);

		// S �׷��̵� ���Ѱ� ����(S �׷��̵� �̿ܿ��� ���� ������. ������ ��Ȯ���� �ʽ��ϴ�. - by hackermz)
		void			RevisionLimit();
		void			RevisionLimit(short* lpattrLimit);
		void			RevisionLimit(Item::Attribute::Type eAttributeType, short& attrLImit);

		// �׷��̵� å��
		void			CalculateItemGrade(void);					
		
												
    protected:

        enum ApplyType
        {
            APPLY   = 1,    // �� �ٲ��� �� ��
            REMOVE  = -1    // �� �ٲ��� �� ��
        };

        CEquipment(const ItemInfo& itemInfo);        
        virtual ~CEquipment();

        inline void InitializeGemAttribute(void);      // ���� ������ ���� �Ӽ��� �ʱ�ȭ
        inline void InitializeUpgradeAttribute(void);  // ���׷��̵� �ܰ�� ���׷��̵� �Ӽ��� �ʱ�ȭ

        inline void ApplyGemAttribute(ApplyType eApplyType);        // ���� �Ӽ��� ����
        inline void ApplyUpgradeAttribute(ApplyType eApplyType);    // ���׷��̵� �Ӽ��� ����
		inline void ApplyRuneAttribute(ApplyType eApplyType);		// �� ������ �Ӽ��� ����

		void CalculateItemPrice(void);					// ���� å��
	       
        unsigned char       m_cSocket[EquipmentInfo::MAX_SOCKET_NUM];                   // ������ ���� ����
        ItemAttribute       m_SocketAttribute[EquipmentInfo::MAX_SOCKET_ATTRIBUTE_NUM]; // ������ �������� ���� �Ӽ�
        ItemAttribute       m_UpgradeAttribute[EquipmentInfo::MAX_UPGRADE_ATTRIBUTE];   // ������ ���׷��̵�� �ٴ� �Ӽ�
		unsigned short		m_usRuneSocket[EquipmentInfo::MAX_RUNE_SOCKET_NUM];			// �� ����

		short				m_wAttribute[Attribute::MAX_ATTRIBUTE_NUM];		// ���������� ���� ������ �Ӽ� (��ũ��Ʈ + DB + ...)	

		unsigned char		m_cUpgradeLevel;	// ���׷��̵� �ܰ�
		unsigned char       m_cSocketNum;       // ���� ���� ����
        unsigned char       m_cMaxSocket;       // �ִ� ���� ����      
        unsigned char       m_cMaxAttribute;    // �ִ� �Ӽ� ����

		unsigned char		m_cSeasonRecord;	// ������ ���� �ñ�

		Grade::GradeInfo	m_GradeInfo;		// �׷��̵� ����

		unsigned char		m_cUpgradeCnt;
		unsigned char		m_cCoreLevel;

        friend class CItemFactory;
		friend class CItemType;
    };

    class CUseItem : public CItem
    {
    public:

        inline static CUseItem* DowncastToUseItem(CItem* lpItem);
	
		// ------------------------------------------------------------------------------------------------------------
		// ���������� ���Ǵ� �Լ��� - ItemServerFunc.cpp ���� ���� ����

		bool Use(CCharacter* lpSender, CCharacter* lpRecver, unsigned short& wError);

    protected:

        CUseItem(const ItemInfo& itemInfo);
        virtual ~CUseItem();
        
        bool UsePotion(CCharacter* lpSender, CCharacter* lpRecver);

        friend class CItemFactory;
    };
};


inline void Item::CItem::MoveItem(ItemPos itemPos) 
{
	if (TakeType::TS_STALL == itemPos.m_cPos)
	{
        m_itemPos_Real = itemPos;
	}
	else
	{
		m_ItemData.m_ItemPos = itemPos;
		if (TakeType::TS_TEMP != itemPos.m_cPos)
		{
			m_itemPos_Real = itemPos;
		}
	}
}

inline Item::CEquipment* Item::CEquipment::DowncastToEquipment(Item::CItem* lpItem)
{
    if (NULL == lpItem) { return NULL; }
    return lpItem->IsSet(DetailData::EQUIP) ?
        static_cast<CEquipment*>(lpItem) : NULL;
}

inline void Item::CEquipment::AddAttribute(Item::Attribute::Type eAttributeType, short nAttributeValue)
{ 
/*
	// ������ۿ� �Ӽ� �߰�
	if (eAttributeType == Attribute::RUNE && 0 != nAttributeValue)
	{
		unsigned short usRuneID = nAttributeValue + EtcItemID::RUNE_START_ID;

		const ItemInfo* lpRuneInfo = CItemMgr::GetInstance().GetItemInfo(usRuneID);
		if (NULL != lpRuneInfo)
		{
			for (unsigned char cIndex = 0; cIndex < EquipmentInfo::MAX_RUNE_SOCKET_NUM; ++cIndex)
			{
				if (0 != m_usRuneSocket[cIndex])
				{
					m_usRuneSocket[cIndex] = usRuneID;

					for (int nIndex = 0; nIndex < Item::Attribute::MAX_ATTRIBUTE_NUM; ++nIndex)
					{
						if (Item::EquipType::S_GRADE != GetItemGrade().m_aryAttributeGrade[nIndex])
						{
							m_wAttribute[nIndex] += lpRuneInfo->m_EquipAttribute.m_usAttributeValue[nIndex];
						}
					}

					return;
				}
			}
		}
	}
	else
*/	{
		if (Item::EquipType::S_GRADE != GetItemGrade().m_aryAttributeGrade[eAttributeType])
		{
			m_wAttribute[eAttributeType] += nAttributeValue; 
		}

/*
		// edith 2009.03.15 min �������� max���� ���Ƶ� �ɼ��̽� ���� �߰��۾��Ȱ� 
		if(eAttributeType == Attribute::MIN_DAMAGE)
		{
			if(m_wAttribute[Attribute::MIN_DAMAGE] > m_wAttribute[Attribute::MAX_DAMAGE])
			{
				// �ΰ� ��ȯ
				short t = m_wAttribute[Attribute::MIN_DAMAGE];
				m_wAttribute[Attribute::MIN_DAMAGE] = m_wAttribute[Attribute::MAX_DAMAGE];
				m_wAttribute[Attribute::MAX_DAMAGE] = t;
			}
		}
//*/
	}
}

inline void Item::CEquipment::SetAttribute(Item::Attribute::Type eAttributeType, short nAttributeValue)
{ 
/*	// ������ۿ� �Ӽ� �߰�
	if (eAttributeType == Attribute::RUNE && 0 != nAttributeValue)
	{
		unsigned short usRuneID = nAttributeValue + EtcItemID::RUNE_START_ID;

		const ItemInfo* lpRuneInfo = CItemMgr::GetInstance().GetItemInfo(usRuneID);
		if (NULL != lpRuneInfo)
		{
			for (unsigned char cIndex = 0; cIndex < EquipmentInfo::MAX_RUNE_SOCKET_NUM; ++cIndex)
			{
				if (0 != m_usRuneSocket[cIndex])
				{
					m_usRuneSocket[cIndex] = usRuneID;

					for (int nIndex = 0; nIndex < Item::Attribute::MAX_ATTRIBUTE_NUM; ++nIndex)
					{
						m_wAttribute[nIndex] = lpRuneInfo->m_EquipAttribute.m_usAttributeValue[nIndex];
					}

					return;
				}
			}
		}
	}
	else
*/	{
		m_wAttribute[eAttributeType] = nAttributeValue; 
	}
}

inline void Item::CEquipment::RemoveUpgradeAttribute(Item::Attribute::Type eAttributeType)
{
	ItemAttribute* first  = m_UpgradeAttribute;
	ItemAttribute* last   = m_UpgradeAttribute + EquipmentInfo::MAX_UPGRADE_ATTRIBUTE;

	for (;first != last; ++first) 
	{
		if(eAttributeType == static_cast<Item::Attribute::Type>(first->m_cType))
		{
			first->m_usValue = 0;
		}
	}
}

inline void Item::CEquipment::RemoveGemAttribute(Item::Attribute::Type eAttributeType)
{
	ItemAttribute* first  = m_SocketAttribute;
	ItemAttribute* last   = m_SocketAttribute + EquipmentInfo::MAX_UPGRADE_ATTRIBUTE;

	for (;first != last; ++first) 
	{
		if(eAttributeType == static_cast<Item::Attribute::Type>(first->m_cType))
		{
			first->m_usValue = 0;
		}
	}
}

inline bool Item::CEquipment::SubRuneAttribute(void)
{
	// �� ������ ����
	for (unsigned char cRune = 0; cRune <= EquipmentInfo::MAX_RUNE_SOCKET_NUM; ++cRune)
	{
		if (m_usRuneSocket[cRune])
		{
			unsigned short usItemId = m_usRuneSocket[cRune];

			const ItemInfo* itemInfo = CItemMgr::GetInstance().GetItemInfo(usItemId);

			if (itemInfo)
			{
				for (unsigned char cIndex = 0; cIndex < Item::Attribute::MAX_ATTRIBUTE_NUM; ++cIndex)
				{
					m_wAttribute[cIndex] -= itemInfo->m_EquipAttribute.m_usAttributeValue[cIndex];
				}
			}

			m_usRuneSocket[cRune] = 0;	
		}		
	}

	return true;
}


inline short Item::CEquipment::GetAttribute(Item::Attribute::Type eAttributeType) 
{ 
	return m_wAttribute[eAttributeType];
}

inline void Item::CEquipment::GetAttribute(short* lpAttributeArray_Out, unsigned short nArraySize) 
{
    unsigned short nCopyNum = nArraySize < Item::Attribute::MAX_ATTRIBUTE_NUM
        ? nArraySize : Item::Attribute::MAX_ATTRIBUTE_NUM;

	for (int nIndex = 0; nIndex < nCopyNum; ++nIndex)
	{
		*(lpAttributeArray_Out + nIndex) = GetAttribute(static_cast<Item::Attribute::Type>(nIndex));
	}
}

inline bool Item::CEquipment::AddSocket(unsigned char cSocket)
{
    if (m_cSocketNum < m_cMaxSocket && 0 == m_cSocket[m_cSocketNum])
    {
        m_cSocket[m_cSocketNum] = cSocket;
        ++m_cSocketNum;

		return true;
    }

    return false;
}

inline void Item::CEquipment::GetSocket(unsigned char* cSockets, unsigned char cArraySize)
{
    unsigned short nCopyNum = cArraySize < Item::EquipmentInfo::MAX_SOCKET_NUM
        ? cArraySize : Item::EquipmentInfo::MAX_SOCKET_NUM;
    unsigned char* lpSocketPastEnd = m_cSocket + nCopyNum;
    std::copy(m_cSocket, lpSocketPastEnd , cSockets);
}

inline void Item::CEquipment::InitializeAttribute(void)
{
    for (unsigned int nIndex = 0; nIndex < Item::Attribute::MAX_ATTRIBUTE_NUM; ++nIndex)
    {
		m_wAttribute[nIndex] = m_ItemInfo.m_EquipAttribute.m_usAttributeValue[nIndex];
    }    
}

inline void Item::CEquipment::InitializeGemAttribute(void)
{
    std::fill_n(m_SocketAttribute, int(EquipmentInfo::MAX_SOCKET_ATTRIBUTE_NUM), ItemAttribute());

    // ��ġ�� ������ ������ �Ӽ��� ���� ����
    CItemType::SetInstallGemAttribute(this);
}

inline void Item::CEquipment::InitializeUpgradeAttribute(void)
{
    std::fill_n(m_UpgradeAttribute, int(EquipmentInfo::MAX_UPGRADE_ATTRIBUTE), ItemAttribute());

    // ������ ���׷��̵� �ܰ谪�� ������ �Ӽ��� ���� ����
	CItemType::SetUpgradeItemAttribute(this);
}

inline void Item::CEquipment::ApplyGemAttribute(ApplyType eApplyType)
{   
    ItemAttribute* first  = m_SocketAttribute;
    ItemAttribute* last   = m_SocketAttribute + EquipmentInfo::MAX_SOCKET_ATTRIBUTE_NUM;

    for (;first != last; ++first) 
	{ 
		AddAttribute(static_cast<Item::Attribute::Type>(first->m_cType), first->m_usValue * eApplyType);
	}
}

inline unsigned char Item::CEquipment::GetMaxRuneSocket()
{
	// ���������ۿ� �� ���� ���� üũ
	return Item::CItemMgr::GetInstance().GetMaxRuneSocket(static_cast<ItemType::Type>(GetItemInfo().m_DetailData.m_cItemType));
}

inline unsigned char Item::CEquipment::GetRuneEquipable()
{
	// ���������ۿ� �� ���� ����� üũ
	unsigned char cRuneSocketCnt = Item::CItemMgr::GetInstance().GetMaxRuneSocket(static_cast<ItemType::Type>(GetItemInfo().m_DetailData.m_cItemType));

	if (!cRuneSocketCnt)
	{
		return 0;
	}

	if (!m_usRuneSocket[0])
	{
		return 1;
	}

	if (!m_usRuneSocket[1] && cRuneSocketCnt == EquipmentInfo::MAX_RUNE_SOCKET_NUM)
	{
		return 2;
	}

	return 0;
}

inline void Item::CEquipment::ApplyUpgradeAttribute(ApplyType eApplyType)
{
    ItemAttribute* first  = m_UpgradeAttribute;
    ItemAttribute* last   = m_UpgradeAttribute + EquipmentInfo::MAX_UPGRADE_ATTRIBUTE;

	for (;first != last; ++first) 
	{
		AddAttribute(static_cast<Item::Attribute::Type>(first->m_cType), first->m_usValue * eApplyType);
	}
}

inline void Item::CEquipment::ApplyRuneAttribute(ApplyType eApplyType)
{
	// ������ۿ� �Ӽ� �߰�/����
	for (unsigned char cIndex = 0; cIndex < EquipmentInfo::MAX_RUNE_SOCKET_NUM; ++cIndex)
	{
		if (m_usRuneSocket[cIndex])
		{
			unsigned short usItemId = m_usRuneSocket[cIndex];

			const ItemInfo* itemInfo = CItemMgr::GetInstance().GetItemInfo(usItemId);

			if (itemInfo)
			{
				for (unsigned char cAttribute = 0; cAttribute < Item::Attribute::MAX_ATTRIBUTE_NUM; ++cAttribute)
				{
					m_wAttribute[cAttribute] += itemInfo->m_EquipAttribute.m_usAttributeValue[cAttribute]*eApplyType;
				}
			}			
		}		
	}
}

inline void Item::CEquipment::GetSubRuneAttribute(short* psAttribute)
{
	// �� �Ӽ��� ������ ������ �Ӽ� ���纻�� �Ѱ��ش�.
	memcpy(psAttribute, m_wAttribute, sizeof(m_wAttribute));

	for (unsigned char cIndex = 0; cIndex < EquipmentInfo::MAX_RUNE_SOCKET_NUM; ++cIndex)
	{
		if (m_usRuneSocket[cIndex])
		{
			const ItemInfo* itemInfo = CItemMgr::GetInstance().GetItemInfo(m_usRuneSocket[cIndex]);

			if (itemInfo)
			{
				for (unsigned char cAttribute = 0; cAttribute < Item::Attribute::MAX_ATTRIBUTE_NUM; ++cAttribute)
				{
					psAttribute[cAttribute] -= itemInfo->m_EquipAttribute.m_usAttributeValue[cAttribute];
				}
			}			
		}		
	}
}

inline void Item::CEquipment::GetSubRuneAttribute(Item::Attribute::Type eAttributeType, short& sAttribute)
{
	sAttribute = m_wAttribute[eAttributeType];

	for (unsigned char cIndex = 0; cIndex < EquipmentInfo::MAX_RUNE_SOCKET_NUM; ++cIndex)
	{
		if (m_usRuneSocket[cIndex])
		{
			const ItemInfo* itemInfo = CItemMgr::GetInstance().GetItemInfo(m_usRuneSocket[cIndex]);

			if (itemInfo)
			{
				sAttribute -= itemInfo->m_EquipAttribute.m_usAttributeValue[eAttributeType];
			}			
		}		
	}
}

inline void Item::CEquipment::SetRuneAttribute(RuneApplyType enRuneApplyType, unsigned char ucIndex)
{
	// �ܺο��� ����� �� �Ӽ� ����/�߰�.

	if (enRuneApplyType&RuneApplyType::RUNE_INDEX && enRuneApplyType&RuneApplyType::RUNE_REMOVE)
	{		
		for (unsigned char ucRuneIndex = 0; ucRuneIndex < EquipmentInfo::MAX_RUNE_SOCKET_NUM; ucRuneIndex++)
		{
			if (m_usRuneSocket[ucRuneIndex])
			{
				const ItemInfo* itemInfo = CItemMgr::GetInstance().GetItemInfo(m_usRuneSocket[ucRuneIndex]);

				if (itemInfo)
				{				
					m_wAttribute[ucIndex] -= itemInfo->m_EquipAttribute.m_usAttributeValue[ucIndex];
				}
			}
		}		
	}
	else if (enRuneApplyType&RuneApplyType::RUNE_ALL && enRuneApplyType&RuneApplyType::RUNE_REMOVE)
	{ 
		ApplyRuneAttribute(REMOVE);
	}
	else if (enRuneApplyType&RuneApplyType::RUNE_INDEX && enRuneApplyType&RuneApplyType::RUNE_APPLY)
	{
		for (unsigned char ucRuneIndex = 0; ucRuneIndex < EquipmentInfo::MAX_RUNE_SOCKET_NUM; ucRuneIndex++)
		{
			if (m_usRuneSocket[ucRuneIndex])
			{
				const ItemInfo* itemInfo = CItemMgr::GetInstance().GetItemInfo(m_usRuneSocket[ucRuneIndex]);

				if (itemInfo)
				{				
					m_wAttribute[ucIndex] += itemInfo->m_EquipAttribute.m_usAttributeValue[ucIndex];
				}
			}
		}
	}
	else if (enRuneApplyType&RuneApplyType::RUNE_ALL && enRuneApplyType&RuneApplyType::RUNE_APPLY)
	{ 
		ApplyRuneAttribute(APPLY);
	}
}

inline unsigned short Item::CEquipment::GetRuneEquipLimitLevel(void) const
{
	// ���� �����ۿ� �������ѿ� ������ ���´�.
	unsigned short usResult	= 0;

	for (unsigned char cIndex = 0; cIndex < EquipmentInfo::MAX_RUNE_SOCKET_NUM; ++cIndex)
	{
		if (m_usRuneSocket[cIndex])
		{
			unsigned short usItemId = m_usRuneSocket[cIndex];

			const ItemInfo* itemInfo = CItemMgr::GetInstance().GetItemInfo(usItemId);

			if (itemInfo)
			{
				if (itemInfo->m_UseLimit.m_cLimitStatus==Item::StatusLimit::LEVEL)
				{
					if (usResult<itemInfo->m_UseLimit.m_wLimitValue)
					{
						usResult = itemInfo->m_UseLimit.m_wLimitValue;
					}
				}				
			}
		}
	}

	return usResult;	
}

inline Item::CUseItem* Item::CUseItem::DowncastToUseItem(CItem* lpItem)
{
    if (NULL == lpItem) { return NULL; }
    return lpItem->IsSet(DetailData::USE_ITEM) ? 
        static_cast<CUseItem*>(lpItem) : NULL;
}

inline bool Item::CItem::GetEnableRepair(void)
{
	unsigned long dwFlags = GetItemInfo().m_DetailData.m_dwFlags;
	return ((dwFlags & Item::DetailData::EQUIP) && !(dwFlags & Item::DetailData::STACKABLE));
}

inline bool Item::CItem::GetEnableStack(void)
{
	return (GetItemInfo().m_DetailData.m_dwFlags & Item::DetailData::STACKABLE) ? true : false;
}

inline bool Item::CItem::GetEnableEquip(void)
{
	return (GetItemInfo().m_DetailData.m_dwFlags & Item::DetailData::EQUIP);
}

inline bool Item::CItem::GetSocketItem(void)
{
	return (GetItemInfo().m_DetailData.m_cItemType == Item::ItemType::GEM);
}

inline bool	Item::CItem::GetRuneItem(void)
{
	return (GetItemInfo().m_DetailData.m_cItemType == Item::ItemType::RUNE);
}

inline bool Item::CItem::GetTwohandItem(void)
{
	unsigned char cItemType = GetItemInfo().m_DetailData.m_cItemType;
	return (cItemType == Item::ItemType::BOW || 
		cItemType == Item::ItemType::CROSSBOW ||
		cItemType == Item::ItemType::STAFF || 
		cItemType == Item::ItemType::TWOHANDED_AXE ||
		cItemType == Item::ItemType::TWOHANDED_BLUNT || 
		cItemType == Item::ItemType::TWOHANDED_SWORD ||
		cItemType == Item::ItemType::OPP_SYTHE);
}

// ------------------------------------------------------------------------------------------------------------
// ConvertTool������ ���Ǵ� �Լ��� - RebalanceOverItem.cpp ���� ���
inline void Item::CEquipment::Repair()
{
	SetNumOrDurability(GetMaxNumOrDurability());
	CalculateItemPrice();
}

inline void Item::CEquipment::InitEquip()
{
	// �ɼ� �̽��� �ϱ����� �� �߰� �ɼ��� �����Ѵ�.
	RuneApplyType	enRuneApplyType	= static_cast<RuneApplyType>(RUNE_ALL | RUNE_REMOVE);

	SetRuneAttribute(enRuneApplyType, 0);

	// ���� �ܰ�� �ʱ�ȭ�ȴ�. (�ɼ��� ���´�.)
	m_cUpgradeLevel = 0;
	m_cCoreLevel = 0;
	InitializeUpgradeAttribute();   

	// ���ϵ� �ʱ�ȭ�ȴ�. (������ ��� ���ŵ����� �ɼ��� ���´�.)
	m_cSocketNum = 0;
	std::fill_n(m_cSocket, unsigned char(Item::EquipmentInfo::MAX_SOCKET_NUM), 0);
	InitializeGemAttribute();
	m_cMaxSocket = m_ItemInfo.m_DetailData.m_cMaxSocketNum;
}

inline void Item::CEquipment::SetEquip()
{
	// �� �Ӽ��� �ٽ� �߰��Ѵ�.
	RuneApplyType enRuneApplyType = static_cast<RuneApplyType>(RUNE_ALL | RUNE_APPLY);

	SetRuneAttribute(enRuneApplyType, 0);
}

#endif