#ifndef _CHAR_CONFIG_PACKET_STRUCT_H_
#define _CHAR_CONFIG_PACKET_STRUCT_H_

#pragma pack(1)

#include <winsock2.h>
#include <windows.h>

// ����/���� ��� ���� (���� : ����ü ũ�Ⱑ ����Ǹ� �ַλ��� ����)
typedef struct PeaceModeInfo* LPPeaceModeInfo;
struct PeaceModeInfo
{
	bool		m_bPeace;
	FILETIME	m_FileTime;

	PeaceModeInfo() : m_bPeace(false)
	{
		m_FileTime.dwHighDateTime = 0;
		m_FileTime.dwLowDateTime = 0;
	}
};

// �ź� �ɼ� (���� : ����ü ũ�Ⱑ ����Ǹ� �ַλ��� ����)
typedef struct RejectOption* LPRejectOption;
struct RejectOption
{
	union 
	{
		struct RejectStruct
		{
			unsigned short	m_Party		: 1;    // ��Ƽ
			unsigned short	m_Exchange	: 1;    // ��ȯ �ź�
			unsigned short	m_Duel		: 1;    // �ϴ��� �ź�
			unsigned short	m_Whisper	: 1;    // �ӼӸ� �ź�
			unsigned short  m_Friend    : 1;    // ģ�� �ź�
			unsigned short	m_Shout		: 1;    // ��ġ�� �ź�            
			unsigned short	m_Recall	: 1;    // ���� �ź�            
			unsigned short	m_Remainder	: 9;
		};

		RejectStruct    Reject;
		unsigned short	m_wReject;
	};

	RejectOption() : m_wReject(0) { }
	RejectOption(unsigned short wReject) : m_wReject(wReject) { }
};

// ������� �ɼ�
typedef struct LanguageOption* LPLanguageOption;
struct LanguageOption
{
	union 
	{
		struct LangStruct
		{
			unsigned short	m_ENG		: 1;
			unsigned short	m_CHS		: 1;
			unsigned short	m_JPN		: 1;
			unsigned short	m_KOR		: 1;
			unsigned short	m_PRT		: 1;
			unsigned short	m_SPN		: 1;
			unsigned short	m_POL		: 1;
			unsigned short	m_Remainder	: 9;
		};

		LangStruct	    Lang;
		unsigned short	m_usLang;
	};

	LanguageOption() : m_usLang(0) { }
	LanguageOption(unsigned short usLang) : m_usLang(usLang) { }
};


#pragma pack()

#endif