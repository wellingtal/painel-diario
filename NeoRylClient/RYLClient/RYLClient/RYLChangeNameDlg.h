//================================================================
//
//	Name : RYLChangeNameDlg.h 
//	Desc : Ä³¸¯ÅÍ ÀÌ¸§ º¯°æÃ¢
//  Date : 2003. 11. 14
//
//================================================================
#ifndef __RYL_CHANGENAMEDLG_H__
#define __RYL_CHANGENAMEDLG_H__

#include "RYLDialog.h"

namespace RYLCHANGENAMEDLG
{
	const DWORD		CHANGE_BUTTON			= 0x00000000 ;
	const DWORD		CANCEL_BUTTON			= 0x00000001 ;
	const DWORD		NEWNAME_EDITBOX			= 0x00000002 ;
/*	const DWORD		ID_EDITBOX				= 0x00000002 ;
	const DWORD		PASSWORD_EDITBOX		= 0x00000003 ;
	const DWORD		IDSAVE_CHECKBOX			= 0x00000004 ;
	const DWORD		ID_LABEL				= 0x00000005 ;
	const DWORD		PASSWORD_LABEL			= 0x00000006 ;
	const DWORD		IDSAVE_LABEL			= 0x00000007 ;*/

	static const unsigned short ALLOW_HAN_NUM = 39;
	static unsigned short AllowHans[ALLOW_HAN_NUM] =
	{
		'°¦', '°§',	'°Ê', '°þ', '³ê',
		'´Ó', '·×', '·Ü', '·Ý', '·Þ', 
		'·à', '·ñ', '·õ', '·ö', '·÷',
		'·ø', '·þ', '¸¡', '¸«', '¸¬',
		'¸×', '¸õ', '¹¾', '¹Å', '¹Ü',
		'¹õ', '»Î', '¾·', 'Áâ', 'Áé', 
		'Â¨', 'Ãñ', 'Äú', 'Ç§', 'Ç¨',
		'Çµ', 'ÇÓ', 'È§', 'Èè'
	};

	static const unsigned short ALLOW_LETTER_NUM	= 2;
	static char AllowLetters[ALLOW_LETTER_NUM] =
	{
		'-', '_'
	};
} ;

class CRYLButton;
class CRYLEdit;
class CRYLLabel;

class CRYLChangeNameDlg : public CRYLDialog
{
public:
	CRYLButton*			m_pChangeBtn ;
	CRYLButton*			m_pCancelBtn ;
	CRYLEdit*			m_pNewNameEdit ;

	CRYLLabel*			m_pTitleLabel ;
	CRYLLabel*			m_pPrevNameTitleLabel ;
	CRYLLabel*			m_pPrevNameLabel ;
	CRYLLabel*			m_pNewNameLabel ;
//	CRYLLabel*			m_pDescLabel ;

public:
	CRYLChangeNameDlg();
	virtual ~CRYLChangeNameDlg();

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK ; } ;
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	static VOID DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift ) ; 

public:
	VOID	ChangeButtonDown() ;

	inline bool CheckCharCreateName(char *Name_In)
	{
		const unsigned short MIN_CHAR_NAME	= 4;
		const unsigned short MAX_CHAR_NAME	= 20;

		if(Name_In == NULL)
			return false;

		// ±æÀÌ Á¦ÇÑ
		size_t Len = strlen(Name_In);
		if(Len < MIN_CHAR_NAME || Len > MAX_CHAR_NAME)
			return false;

		LPBYTE CheckName = (LPBYTE)Name_In;

		// Á¦ÇÑ ¹®ÀÚ °Ë»ç
		int ACount = 0;
		for(unsigned short LCount = 0; LCount < Len; LCount++)
		{
			if((CheckName[LCount] & 0x80) == 0x80)
			{
				// 2Byte ¹®ÀÚ Ã¼Å©
				if(CheckName[LCount + 1] == NULL)
					return false;
				
				// Çã¿ë ¹üÀ§ Ã¼Å© (ÇÑ±Û)
				if(CheckName[LCount] < 0xB0 || CheckName[LCount] > 0xC9)
					return false;
					
				if(CheckName[LCount + 1] < 0xA1 || CheckName[LCount + 1] > 0xFE)
					return false;

				// ÇÑ±Û ºÎºÐ ºÒ Çã¿ë
				for(ACount = 0; ACount < RYLCHANGENAMEDLG::ALLOW_HAN_NUM; ACount++)
				{
					if(MAKEWORD(CheckName[LCount + 1], CheckName[LCount])  == RYLCHANGENAMEDLG::AllowHans[ACount])
						break;
				}

				if(ACount != RYLCHANGENAMEDLG::ALLOW_HAN_NUM)
					return false;

				LCount += 1;
				continue;
			}
			else
			{
				// ¿µ¹®ÀÚ Çã¿ë	
				if((CheckName[LCount] >= 'A' && CheckName[LCount] <= 'Z') || (CheckName[LCount] >= 'a' && CheckName[LCount] <= 'z'))
					continue;

				// ¼ýÀÚ Çã¿ë
				if(CheckName[LCount] >= '0' && CheckName[LCount] <= '9')
					continue;

				// Æ¯¼ö ±âÈ£ ºÎºÐ Çã¿ë
				for(ACount = 0; ACount < RYLCHANGENAMEDLG::ALLOW_LETTER_NUM; ACount++)
				{
					if(CheckName[LCount] == RYLCHANGENAMEDLG::AllowLetters[ACount])
						break;
				}

				if(ACount == RYLCHANGENAMEDLG::ALLOW_LETTER_NUM)
					return false;
			}
		}

		return true;
	}
};

#endif //__RYL_CHANGENAMEDLG_H__