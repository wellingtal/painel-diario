#ifndef _CHAR_ITEM_PACKET_STRUCT_H_
#define _CHAR_ITEM_PACKET_STRUCT_H_

#include <DB/DBDefine.h>
#include <Item/ItemStructure.h>

#pragma pack(1)

// �ʵ� ������Ʈ ����ü
struct FieldObject
{
	enum CheckBit	{ Gold = 0x80000000 };

	unsigned __int64	m_nOID;			//  ������Ʈ ���̵� (���� Ȥ�� Object)
	unsigned long		m_dwTypeID;     //  Ÿ�� ���̵� (���� 1��Ʈ ���̸� ��)

    unsigned short		m_usXPos;       //  ������ X��ġ
    unsigned short		m_usYPos;       //  ������ Y��ġ
    unsigned short		m_usZPos;       //  ������ Z��ġ
    unsigned char		m_cNum;         //  ������ ��    
};

struct TakeType 
{
	enum TakeSource 
	{ 
        TS_NONE			= 0,	TS_EQUIP		= 1, 
        TS_INVEN		= 2,	TS_QSLOT		= 3,
        TS_SSLOT		= 4,	TS_TEMP			= 6, 
        TS_EXTRA		= 7,	TS_EXCHANGE		= 8,
        TS_DEPOSIT		= 9,	TS_STALL		= 10,
		TS_TEMPINVEN	= 11,	TS_CAMPSHOP		= 12, 

		TS_ADMIN		= 13,	TS_MAXTYPE		= 14
	};

    Item::ItemPos   m_srcPos;
    Item::ItemPos   m_dstPos;
    unsigned char   m_cNum;

	TakeType() : m_cNum(0) { }
	TakeType(Item::ItemPos srcPos, Item::ItemPos dstPos, unsigned char cNum)
        : m_srcPos(srcPos), m_dstPos(dstPos), m_cNum(cNum) { }
};

// ĳ��Ʈ ������Ʈ ����ü (Cast Object)
struct CastObject
{
	POS				m_DstPos;           // ���� ��ġ
	unsigned long	m_dwTargetID;       // Ÿ�� ���̵�

	unsigned short	m_wTypeID;          // ���̵�	( ���ϰ�� �ݾ� )
	unsigned char	m_cObjectType;      // ����	( 0 = ĳ��Ʈ, ���� ��, 2 = �� )
	unsigned char	m_cObjectLevel;     // ����
};

#pragma pack()

#endif