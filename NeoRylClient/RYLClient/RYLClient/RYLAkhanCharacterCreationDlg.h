//================================================================
//
//	Name : RYLAkhanCharacterCreation.h 
//	Desc : ĳ���� ���� â
//  Date : 2003. 11. 30
//
//================================================================
#ifndef __RYL_AKHAN_CHARACTERCREATION_H__
#define __RYL_AKHAN_CHARACTERCREATION_H__

#include "RYLDialog.h"

namespace RYLAKHANCHATACTERCREATION
{
	const DWORD		NAME_EDIT					= 0x00000000 ;
	const DWORD		HAIRNEXT_BUTTON				= 0x00000001 ;
	const DWORD		HAIRPREV_BUTTON				= 0x00000002 ;
	const DWORD		FACENEXT_BUTTON				= 0x00000003 ;
	const DWORD		FACEPREV_BUTTON				= 0x00000004 ;
	const DWORD		CLASSNEXT_BUTTON			= 0x00000005 ;
	const DWORD		CLASSPREV_BUTTON			= 0x00000006 ;
	const DWORD		STRPOINTINC_BUTTON			= 0x00000007 ;
	const DWORD		STRPOINTDEC_BUTTON			= 0x00000008 ;
	const DWORD		DEXPOINTINC_BUTTON			= 0x00000009 ;
	const DWORD		DEXPOINTDEC_BUTTON			= 0x00000010 ;
	const DWORD		CONPOINTINC_BUTTON			= 0x00000011 ;
	const DWORD		CONPOINTDEC_BUTTON			= 0x00000012 ;
	const DWORD		INTPOINTINC_BUTTON			= 0x00000013 ;
	const DWORD		INTPOINTDEC_BUTTON			= 0x00000014 ;
	const DWORD		WISPOINTINC_BUTTON			= 0x00000015 ;
	const DWORD		WISPOINTDEC_BUTTON			= 0x00000016 ;
	const DWORD		COMBATANT_BUTTON			= 0x00000017 ;
	const DWORD		OFFICIATOR_BUTTON			= 0x00000018 ;
	const DWORD		MAKE_BUTTON					= 0x00000019 ;
	const DWORD		CALCEL_BUTTON				= 0x00000020 ;

	static const unsigned short ALLOW_HAN_NUM = 39;
	static unsigned short AllowHans[ALLOW_HAN_NUM] =
	{
		'��', '��',	'��', '��', '��',
		'��', '��', '��', '��', '��', 
		'��', '��', '��', '��', '��',
		'��', '��', '��', '��', '��',
		'��', '��', '��', '��', '��',
		'��', '��', '��', '��', '��', 
		'¨', '��', '��', 'ǧ', 'Ǩ',
		'ǵ', '��', 'ȧ', '��'
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
class CRYLAkhanCharacterCreationDlg : public CRYLDialog
{
private:
	CRYLLabel*			m_pCharacterCreateLabel ;
	// ĳ���� �̸�
	CRYLLabel*			m_pNameLabel ;
	CRYLEdit*			m_pNameEdit ;

	// ��� ��Ÿ�� ��ư
	CRYLLabel*			m_pHairLabel ;
	CRYLButton*			m_pHairNextBtn ;
	CRYLButton*			m_pHairPrevBtn ;
	
	// �� ��Ÿ�� ��ư
	CRYLLabel*			m_pFaceLabel ;
	CRYLButton*			m_pFaceNextBtn ;
	CRYLButton*			m_pFacePrevBtn ;

	// Ŭ���� ����
	CRYLLabel*			m_pClassSelectLabel ;
	CRYLLabel*			m_pClassLabel ;
	CRYLButton*			m_pClassNextBtn ;
	CRYLButton*			m_pClassPrevBtn ;

	// �������ͽ� �й�
	CRYLLabel*			m_pStatusDivideLabel ;
	
	// ���ʽ� ����Ʈ
	CRYLLabel*			m_pBonusPointLabel ;
	CRYLLabel*			m_pBonusPointValueLabel ;
	// STR ����Ʈ ����,���� ��ư
	CRYLLabel*			m_pStrLabel ;
	CRYLLabel*			m_pStrValueLabel ;
	CRYLButton*			m_pStrPointIncBtn ;
	CRYLButton*			m_pStrPointDecBtn ;
	// DEX ����Ʈ ����,���� ��ư
	CRYLLabel*			m_pDexLabel ;
	CRYLLabel*			m_pDexValueLabel ;
	CRYLButton*			m_pDexPointIncBtn ;
	CRYLButton*			m_pDexPointDecBtn ;
	// CON ����Ʈ ����,���� ��ư
	CRYLLabel*			m_pConLabel ;
	CRYLLabel*			m_pConValueLabel ;
	CRYLButton*			m_pConPointIncBtn ;
	CRYLButton*			m_pConPointDecBtn ;
	// INT ����Ʈ ����,���� ��ư
	CRYLLabel*			m_pIntLabel ;
	CRYLLabel*			m_pIntValueLabel ;
	CRYLButton*			m_pIntPointIncBtn ;
	CRYLButton*			m_pIntPointDecBtn ;
	// WIS ����Ʈ ����,���� ��ư
	CRYLLabel*			m_pWisLabel ;
	CRYLLabel*			m_pWisValueLabel ;
	CRYLButton*			m_pWisPointIncBtn ;
	CRYLButton*			m_pWisPointDecBtn ;

	
	CRYLLabel*			m_pClassSelectionLabel ;

	// �����
	CRYLButton*			m_pMakeBtn ;
	// ���
	CRYLButton*			m_pCalcelBtn ;

	CRYLPicture*		m_pBonusDEXPicture ;
	CRYLPicture*		m_pBonusCONPicture ;

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

	CHAR				m_szClassName[ 2 ][ MAX_PATH ] ;
	INT					m_iCurrentClass ;
	
public :
	CRYLAkhanCharacterCreationDlg() ;
	virtual ~CRYLAkhanCharacterCreationDlg() ;

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

public :
	VOID		CreateCancel() ;

	inline bool CheckCharCreateName(char *Name_In)
	{
		const unsigned short MIN_CHAR_NAME	= 4;
		const unsigned short MAX_CHAR_NAME	= 15;

		if(Name_In == NULL)
			return false;

		// ���� ����
		size_t Len = strlen(Name_In);
		if(Len < MIN_CHAR_NAME || Len > MAX_CHAR_NAME)
			return false;

		LPBYTE CheckName = (LPBYTE)Name_In;

		// ���� ���� �˻�
		int ACount = 0;
		for(unsigned short LCount = 0; LCount < Len; LCount++)
		{
			if((CheckName[LCount] & 0x80) == 0x80)
			{
				// 2Byte ���� üũ
				if(CheckName[LCount + 1] == NULL)
					return false;
				
				// ��� ���� üũ (�ѱ�)
				if(CheckName[LCount] < 0xB0 || CheckName[LCount] > 0xC9)
					return false;
					
				if(CheckName[LCount + 1] < 0xA1 || CheckName[LCount + 1] > 0xFE)
					return false;

				// �ѱ� �κ� �� ���
				for(ACount = 0; ACount < RYLAKHANCHATACTERCREATION::ALLOW_HAN_NUM; ACount++)
				{
					if(MAKEWORD(CheckName[LCount + 1], CheckName[LCount])  == RYLAKHANCHATACTERCREATION::AllowHans[ACount])
						break;
				}

				if(ACount != RYLAKHANCHATACTERCREATION::ALLOW_HAN_NUM)
					return false;

				LCount += 1;
				continue;
			}
			else
			{
				// ������ ���	
				if((CheckName[LCount] >= 'A' && CheckName[LCount] <= 'Z') || (CheckName[LCount] >= 'a' && CheckName[LCount] <= 'z'))
					continue;

				// ���� ���
				if(CheckName[LCount] >= '0' && CheckName[LCount] <= '9')
					continue;

				// Ư�� ��ȣ �κ� ���
				for(ACount = 0; ACount < RYLAKHANCHATACTERCREATION::ALLOW_LETTER_NUM; ACount++)
				{
					if(CheckName[LCount] == RYLAKHANCHATACTERCREATION::AllowLetters[ACount])
						break;
				}

				if(ACount == RYLAKHANCHATACTERCREATION::ALLOW_LETTER_NUM)
					return false;
			}
		}

		return true;
	}

	VOID		ClassNamePrev() ;
	VOID		ClassNameNext() ;
	VOID		ChangeClassModel() ;
} ;


#endif //__RYL_AKHAN_CHARACTERCREATION_H__