#ifndef _ITEMNODE_H
#define _ITEMNODE_H

#include <windows.h>

// item kind structure
typedef union _ItemKind {
	struct {
		unsigned char	m_cSort;	// sort of item
		unsigned char	m_cClass;	// class of item
	} IClass;
	unsigned short		m_wIKind;

	operator unsigned short()
	{
		return m_wIKind;
	}
	operator=(unsigned short In)
	{
		m_wIKind=In;		
	}
	bool operator<(const _ItemKind& rOp) const
	{
		return (m_wIKind < rOp.m_wIKind); 
	}

} ItemKind, *LPItemKind;

const unsigned int		ItemKindSize		= sizeof(ItemKind);


typedef struct _IEnchant {
//	unsigned short	m_wIEnchantFlag;
//	unsigned short	m_wIEnchantValue;

	unsigned char	m_cIEnchant[4];
} IEnchant, *LPIEnchant;

const unsigned int		IEnchantSize		= sizeof(IEnchant);



// item id structure
typedef union _ItemID {
	struct {
		union {
			struct {
				unsigned char	m_cCurDur;		// durability or number of item
				unsigned char	m_cMaxDur;		// durability or number of item
			} ItemDur;
			unsigned short		m_wSkill;
		} DurOrSkill;

		IEnchant		m_Enchant;		// use for upgrade

		ItemKind		m_Kind;			// kind of item
	} IDSlip;

	__int64		m_dlItemID;

	operator __int64()
	{
		return m_dlItemID;
	}
	operator=(__int64 in)
	{
		m_dlItemID=in;
	}
} ItemID, *LPItemID;

const int			ItemIDSize	= sizeof(ItemID);



// item size
typedef struct _ItemSize	ItemSize, *LPItemSize;
struct _ItemSize {
	unsigned char	SizeX : 4;	// use for upgrade
	unsigned char	SizeY : 4;	// remained for last user

};

const int			ItemSizeSize	= sizeof(ItemSize);


typedef enum _Race		{ Race_NONE, Race_HUMAN } Race;
typedef enum _Feature	{ Feature_NORMAL, Feature_MAGIC, Feature_SPECIAL } Feature;


/*//
typedef enum _AbSort	{ AS_NONE, AS_STR, AS_DEX, AS_CON, AS_INT, AS_LEV,
						  AS_SK, AS_CLS, AS_BR, AS_HP, AS_MP } AbSort;
typedef struct _AbValue		AbValue, *LPAbValue;
struct _AbValue {
	unsigned short	m_wAb;
	unsigned short	m_wValue;

};

const int			AbValueSize	= sizeof(AbValue);


typedef struct _AbModify {
	AbValue			m_ReqAb[4];	// required ability
	AbValue			m_AddAb[4];	// added ability
	AbValue			m_ENAb;		// elemental nature

} AbModify, *LPAbModify;
//*/

typedef struct _ItemNode2	ItemNode2, *LPItemNode2;
struct _ItemNode2 {
	char			*m_pItem;			// pointer of start line

	__int64			m_dlID;				// id of item (8 byte)

	char			m_Name[32];			// name of this item

	DWORD			m_dwValue;			// value of item

	ItemSize		m_ItemSize;

	int				m_nRestrictStr;		// restrict ability
	int				m_nRestrictDex;
	int				m_nRestrictCon;
	int				m_nRestrictInt;
	int				m_nRestrictWis;

	unsigned short	m_wRestrictS;		// restrict skill
	unsigned short	m_wRestrictSLev;	// restrict skill level

	unsigned short	m_wRestrictC;		// restrict class
	unsigned short	m_wRestrictCLev;	// restrict character level

	unsigned short	m_wAssignedS;		// assigned skill
	unsigned short	m_wAssignedSLev;	// assigned skill level

	Race			m_Race;				// race limit of item user
	Feature			m_Feature;			// feature of item

//	float			m_fDamage;
//	float			m_fDamageLimit;		// damage limit

//	float			m_fDefense;
//	float			m_fDefenseLimit;

		

	float			m_fDamage;			// damage
	float			m_fDamageL;			// damage limit

	float			m_fDefense;			// defense
	float			m_fDefenseL;		// defense limit
	float			m_fBR;
	/*
	int				m_nAPL;
	int				m_nAPH;

	int				m_nBR;				// item blocking rate
	float			m_fDf;
	*/

	char			m_pObject[32];		// pointer of object

};
const int			ItemNode2Size	= sizeof(ItemNode2);


#endif
