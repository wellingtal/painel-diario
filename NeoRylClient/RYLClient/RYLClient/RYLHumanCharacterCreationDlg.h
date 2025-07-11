//================================================================
//
//	Name : RYLHumanCharacterCreation.h 
//	Desc : Ä³¸¯ÅÍ »ý¼º Ã¢
//  Date : 2003. 11. 30
//
//================================================================
#ifndef __RYL_HUMAN_CHARACTERCREATION_H__
#define __RYL_HUMAN_CHARACTERCREATION_H__

#include "RYLDialog.h"

namespace RYLHUMANCHATACTERCREATION
{
	const DWORD		NAME_LABEL					= 0x00000000 ;
	const DWORD		NAME_EDIT					= 0x00000001 ;
	
	const DWORD		SEX_LABEL					= 0x00000002 ;
	const DWORD		MALE_BUTTON					= 0x00000003 ;
	const DWORD		FEMALE_BUTTON				= 0x00000004 ;
	
	const DWORD		HAIR_LABEL					= 0x00000005 ;
	const DWORD		HAIRNEXT_BUTTON				= 0x00000006 ;
	const DWORD		HAIRPREV_BUTTON				= 0x00000007 ;

	const DWORD		FACE_LABEL					= 0x00000008 ;
	const DWORD		FACENEXT_BUTTON				= 0x00000009 ;
	const DWORD		FACEPREV_BUTTON				= 0x00000010 ;
	
	const DWORD		BONUSPOINT_LABEL			= 0x00000011 ;
	const DWORD		BONUSPOINTVALUE_LABEL		= 0x00000012 ;
	
	const DWORD		STRPOINT_LABEL				= 0x00000013 ;
	const DWORD		STRPOINTVALUE_LABEL			= 0x00000014 ;
	const DWORD		STRPOINTINC_BUTTON			= 0x00000015 ;
	const DWORD		STRPOINTDEC_BUTTON			= 0x00000016 ;

	const DWORD		DEXPOINT_LABEL				= 0x00000017 ;
	const DWORD		DEXPOINTVALUE_LABEL			= 0x00000018 ;
	const DWORD		DEXPOINTINC_BUTTON			= 0x00000019 ;
	const DWORD		DEXPOINTDEC_BUTTON			= 0x00000020 ;

	const DWORD		CONPOINT_LABEL				= 0x00000021 ;
	const DWORD		CONPOINTVALUE_LABEL			= 0x00000022 ;
	const DWORD		CONPOINTINC_BUTTON			= 0x00000023 ;
	const DWORD		CONPOINTDEC_BUTTON			= 0x00000024 ;

	const DWORD		INTPOINT_LABEL				= 0x00000025 ;
	const DWORD		INTPOINTVALUE_LABEL			= 0x00000026 ;
	const DWORD		INTPOINTINC_BUTTON			= 0x00000027 ;
	const DWORD		INTPOINTDEC_BUTTON			= 0x00000028 ;

	const DWORD		WISPOINT_LABEL				= 0x00000029 ;
	const DWORD		WISPOINTVALUE_LABEL			= 0x00000030 ;
	const DWORD		WISPOINTINC_BUTTON			= 0x00000031 ;
	const DWORD		WISPOINTDEC_BUTTON			= 0x00000032 ;

	const DWORD		CLASSSELECTION_LABEL		= 0x00000033 ;
	const DWORD		CLASSNEXT_BUTTON			= 0x00000034 ;
	const DWORD		CLASSPREV_BUTTON			= 0x00000035 ;

	const DWORD		MAKE_BUTTON					= 0x00000036 ;
	const DWORD		CALCEL_BUTTON				= 0x00000037 ;

	const DWORD		SHIRTNEXT_BUTTON			= 0x00000038 ;
	const DWORD		SHIRTPREV_BUTTON			= 0x00000039 ;

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

class CRYLButton ;
class CRYLEdit ;
class CRYLLabel ;
class CRYLPicture ;


//===========================================================
// character creation
//===========================================================
class CRYLHumanCharacterCreationDlg : public CRYLDialog
{
private:
	// Ä³¸¯ÅÍ ÀÌ¸§
	CRYLLabel*			m_pCharacterCreateLabel ;
	CRYLLabel*			m_pNameLabel ;
	CRYLEdit*			m_pNameEdit ;
	// ¼ºº°
	CRYLLabel*			m_pSexLabel ;
	CRYLButton*			m_pMaleBtn ;
	CRYLButton*			m_pFemaleBtn ;
	// Çì¾î ½ºÅ¸ÀÏ ¹öÆ°
	CRYLLabel*			m_pHairLabel ;
	CRYLButton*			m_pHairNextBtn ;
	CRYLButton*			m_pHairPrevBtn ;
	// ¾ó±¼ ½ºÅ¸ÀÏ ¹öÆ°
	CRYLLabel*			m_pFaceLabel ;
	CRYLButton*			m_pFaceNextBtn ;
	CRYLButton*			m_pFacePrevBtn ;

	// ¼ÅÃ÷
	CRYLLabel*			m_pShirtLabel ;
	CRYLButton*			m_pShirtNextBtn ;
	CRYLButton*			m_pShirtPrevBtn ;

	
	// Å¬·¹½º ¼±ÅÃ
	CRYLLabel*			m_pClassSelectLabel ;
	CRYLLabel*			m_pClassLabel ;
	CRYLButton*			m_pClassNextBtn ;
	CRYLButton*			m_pClassPrevBtn ;

	// ½ºÅ×ÀÌÅÍ½º ºÐ¹è
	CRYLLabel*			m_pStatusDivideLabel ;
	
	// º¸³Ê½º Æ÷ÀÎÆ®
	CRYLLabel*			m_pBonusPointLabel ;
	CRYLLabel*			m_pBonusPointValueLabel ;
	// STR Æ÷ÀÎÆ® Áõ°¡,°¨¼Ò ¹öÆ°
	CRYLLabel*			m_pStrLabel ;
	CRYLLabel*			m_pStrValueLabel ;
	CRYLButton*			m_pStrPointIncBtn ;
	CRYLButton*			m_pStrPointDecBtn ;
	// DEX Æ÷ÀÎÆ® Áõ°¡,°¨¼Ò ¹öÆ°
	CRYLLabel*			m_pDexLabel ;
	CRYLLabel*			m_pDexValueLabel ;
	CRYLButton*			m_pDexPointIncBtn ;
	CRYLButton*			m_pDexPointDecBtn ;
	// CON Æ÷ÀÎÆ® Áõ°¡,°¨¼Ò ¹öÆ°
	CRYLLabel*			m_pConLabel ;
	CRYLLabel*			m_pConValueLabel ;
	CRYLButton*			m_pConPointIncBtn ;
	CRYLButton*			m_pConPointDecBtn ;
	// INT Æ÷ÀÎÆ® Áõ°¡,°¨¼Ò ¹öÆ°
	CRYLLabel*			m_pIntLabel ;
	CRYLLabel*			m_pIntValueLabel ;
	CRYLButton*			m_pIntPointIncBtn ;
	CRYLButton*			m_pIntPointDecBtn ;
	// WIS Æ÷ÀÎÆ® Áõ°¡,°¨¼Ò ¹öÆ°
	CRYLLabel*			m_pWisLabel ;
	CRYLLabel*			m_pWisValueLabel ;
	CRYLButton*			m_pWisPointIncBtn ;
	CRYLButton*			m_pWisPointDecBtn ;

	// ¸¸µé±â
	CRYLButton*			m_pMakeBtn ;
	// Ãë¼Ò
	CRYLButton*			m_pCalcelBtn ;

	CRYLPicture*		m_pBonusPicture1 ;
	CRYLPicture*		m_pBonusPicture2 ;

	DWORD				m_dwClass ;
	DWORD				m_dwSex	;

	DWORD				m_dwIP ;
	DWORD				m_dwSTR ;
	DWORD				m_dwDEX ;
	DWORD				m_dwCON ;
	DWORD				m_dwINT ;
	DWORD				m_dwWIS ;
	DWORD				m_dwFace ;
	DWORD				m_dwHair ;
	DWORD				m_dwShirt ;

	DWORD				m_dwEndTimer ;

	CHAR				m_szClassName[ 4 ][ MAX_PATH ] ;
	INT					m_iCurrentClass ;
	
public :
	CRYLHumanCharacterCreationDlg() ;
	virtual ~CRYLHumanCharacterCreationDlg() ;

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

	VOID	InitValue() ;
	VOID	ClassNamePrev()  ;
	VOID	ClassNameNext() ;
	VOID	DrawBonusPicture() ;
	VOID	CreateCancel() ;

	inline bool CheckCharCreateName(char *Name_In)
	{
		const unsigned short MIN_CHAR_NAME	= 4;
		const unsigned short MAX_CHAR_NAME	= 15;

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
				for(ACount = 0; ACount < RYLHUMANCHATACTERCREATION::ALLOW_HAN_NUM; ACount++)
				{
					if(MAKEWORD(CheckName[LCount + 1], CheckName[LCount])  == RYLHUMANCHATACTERCREATION::AllowHans[ACount])
						break;
				}

				if(ACount != RYLHUMANCHATACTERCREATION::ALLOW_HAN_NUM)
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
				for(ACount = 0; ACount < RYLHUMANCHATACTERCREATION::ALLOW_LETTER_NUM; ACount++)
				{
					if(CheckName[LCount] == RYLHUMANCHATACTERCREATION::AllowLetters[ACount])
						break;
				}

				if(ACount == RYLHUMANCHATACTERCREATION::ALLOW_LETTER_NUM)
					return false;
			}
		}

		return true;
	}
} ;


#endif //__RYL_HUMAN_CHARACTERCREATION_H__