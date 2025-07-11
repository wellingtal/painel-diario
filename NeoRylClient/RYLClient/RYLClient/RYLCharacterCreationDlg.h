//================================================================
//
//	Name : RYLCharacterCreation.h 
//	Desc : ĳ���� ���� â
//  Date : 2004. 9. 16
//
//================================================================
#ifndef __RYL_CHARACTERCREATION_H__
#define __RYL_CHARACTERCREATION_H__

#include "RYLDialog.h"

namespace RYLCHATACTERCREATION
{
//	WORK_LIST 1.2 ĳ���� ����â���� ���� �� ������
//	const DWORD		RACE_LABEL					= 0x00000000 ;
//	const DWORD		RACESELECTION_LABEL			= 0x00000001 ;
//	const DWORD		RACENEXT_BUTTON				= 0x00000002 ;
//	const DWORD		RACEPREV_BUTTON				= 0x00000003 ;

	const DWORD		NAME_LABEL					= 0x00000004 ;
	const DWORD		NAME_EDIT					= 0x00000005 ;

	const DWORD		CLASS_LABEL					= 0x00000006 ;
	const DWORD		CLASSSELECTION_LABEL		= 0x00000007 ;
	const DWORD		CLASSNEXT_BUTTON			= 0x00000008 ;
	const DWORD		CLASSPREV_BUTTON			= 0x00000009 ;

	const DWORD		SEX_LABEL					= 0x00000010 ;
	const DWORD		SEXSELECTION_LABEL			= 0x00000011 ;
	const DWORD		SEXNEXT_BUTTON				= 0x00000012 ;
	const DWORD		SEXPREV_BUTTON				= 0x00000013 ;

	const DWORD		HAIR_LABEL					= 0x00000014 ;
	const DWORD		HAIRSELECTION_LABEL			= 0x00000015 ;
	const DWORD		HAIRNEXT_BUTTON				= 0x00000016 ;
	const DWORD		HAIRPREV_BUTTON				= 0x00000017 ;

	const DWORD		FACE_LABEL					= 0x00000018 ;
	const DWORD		FACESELECTION_LABEL			= 0x00000019 ;
	const DWORD		FACENEXT_BUTTON				= 0x00000020 ;
	const DWORD		FACEPREV_BUTTON				= 0x00000021 ;

	const DWORD		SHIRT_LABEL					= 0x00000022 ;
	const DWORD		SHIRTSELECTION_LABEL		= 0x00000023 ;
	const DWORD		SHIRTNEXT_BUTTON			= 0x00000024 ;
	const DWORD		SHIRTPREV_BUTTON			= 0x00000025 ;

	const DWORD		STATUS_LABEL				= 0x00000026 ;

	const DWORD		STRPOINT_LABEL				= 0x00000027 ;
	const DWORD		STRPOINTVALUE_LABEL			= 0x00000028 ;
	const DWORD		STRPOINTINC_BUTTON			= 0x00000029 ;
	const DWORD		STRPOINTDEC_BUTTON			= 0x00000030 ;

	const DWORD		DEXPOINT_LABEL				= 0x00000031 ;
	const DWORD		DEXPOINTVALUE_LABEL			= 0x00000032 ;
	const DWORD		DEXPOINTINC_BUTTON			= 0x00000033 ;
	const DWORD		DEXPOINTDEC_BUTTON			= 0x00000034 ;

	const DWORD		CONPOINT_LABEL				= 0x00000035 ;
	const DWORD		CONPOINTVALUE_LABEL			= 0x00000036 ;
	const DWORD		CONPOINTINC_BUTTON			= 0x00000037 ;
	const DWORD		CONPOINTDEC_BUTTON			= 0x00000038 ;

	const DWORD		INTPOINT_LABEL				= 0x00000039 ;
	const DWORD		INTPOINTVALUE_LABEL			= 0x00000040 ;
	const DWORD		INTPOINTINC_BUTTON			= 0x00000041 ;
	const DWORD		INTPOINTDEC_BUTTON			= 0x00000042 ;

	const DWORD		WISPOINT_LABEL				= 0x00000043 ;
	const DWORD		WISPOINTVALUE_LABEL			= 0x00000044 ;
	const DWORD		WISPOINTINC_BUTTON			= 0x00000045 ;
	const DWORD		WISPOINTDEC_BUTTON			= 0x00000046 ;

	const DWORD		BONUSPOINT_LABEL			= 0x00000047 ;
	const DWORD		BONUSPOINTVALUE_LABEL		= 0x00000048 ;

	const DWORD		MAKE_BUTTON					= 0x00000049 ;
	const DWORD		CALCEL_BUTTON				= 0x00000050 ;

	const DWORD		CAMERA_LABEL				= 0x00000051 ;
	const DWORD		CAMERASELECTION_LABEL		= 0x00000052 ;
	const DWORD		CAMERARIGHT_BUTTON			= 0x00000053 ;
	const DWORD		CAMERALEFT_BUTTON			= 0x00000054 ;

	static const unsigned short ALLOW_HAN_NUM = 39; // was 39 
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

	static const unsigned short HUMAN_CLASS_NUM = 4;
	static const unsigned short AKHAN_CLASS_NUM = 2;

	static const unsigned short HUMAN_HAIR_MAN_NUM = 5;
	static const unsigned short HUMAN_HAIR_WOMEN_NUM = 7;
	static const unsigned short AKHAN_HAIR_NUM = 7;

	static const unsigned short HUMAN_FACE_NUM = 7;
	static const unsigned short AKHAN_FACE_NUM = 7;

	static const unsigned short HUMAN_SHIRT_NUM = 4;
} ;

class CRYLButton ;
class CRYLEdit ;
class CRYLLabel ;
class CRYLPicture ;


//===========================================================
// character creation
//===========================================================
class CRYLCharacterCreationDlg : public CRYLDialog
{
private:
	CRYLLabel*			m_pCharacterCreateLabel ;

//	WORK_LIST 1.2 ĳ���� ����â���� ���� �� ������
	// ����
//	CRYLLabel*			m_pRaceLabel ;
//	CRYLLabel*			m_pRaceSelectLabel ;
//	CRYLButton*			m_pRaceNextBtn ;
//	CRYLButton*			m_pRacePrevBtn ;
	
	// ĳ���� �̸�
	CRYLLabel*			m_pNameLabel ;
	CRYLEdit*			m_pNameEdit ;

	// Ŭ���� ����
	CRYLLabel*			m_pClassLabel ;
	CRYLLabel*			m_pClassSelectLabel ;
	CRYLButton*			m_pClassNextBtn ;
	CRYLButton*			m_pClassPrevBtn ;

	// ����
	CRYLLabel*			m_pSexLabel ;
	CRYLLabel*			m_pSexSelectLabel ;
	CRYLButton*			m_pSexNextBtn ;
	CRYLButton*			m_pSexPrevBtn ;

	// ��� ��Ÿ�� ��ư
	CRYLLabel*			m_pHairLabel ;
	CRYLLabel*			m_pHairSelectLabel ;
	CRYLButton*			m_pHairNextBtn ;
	CRYLButton*			m_pHairPrevBtn ;

	// �� ��Ÿ�� ��ư
	CRYLLabel*			m_pFaceLabel ;
	CRYLLabel*			m_pFaceSelectLabel ;
	CRYLButton*			m_pFaceNextBtn ;
	CRYLButton*			m_pFacePrevBtn ;

	// ����
	CRYLLabel*			m_pShirtLabel ;
	CRYLLabel*			m_pShirtSelectLabel ;
	CRYLButton*			m_pShirtNextBtn ;
	CRYLButton*			m_pShirtPrevBtn ;

	// ĳ���� ȸ��
	CRYLLabel*			m_pCameraLabel ;
	CRYLLabel*			m_pCameraSelectLabel ;
	CRYLButton*			m_pCameraRightBtn ;
	CRYLButton*			m_pCameraLeftBtn ;

	// �������ͽ� �й�
	CRYLLabel*			m_pStatusDivideLabel ;

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

	// ���ʽ� ����Ʈ
	CRYLLabel*			m_pBonusPointLabel ;
	CRYLLabel*			m_pBonusPointValueLabel ;

	// �����
	CRYLButton*			m_pMakeBtn ;

	// ���
	CRYLButton*			m_pCalcelBtn ;

	CRYLPicture*		m_pBonusPicture1 ;
	CRYLPicture*		m_pBonusPicture2 ;

	DWORD				m_dwRace ;
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

	CHAR				m_szRaceName[ 2 ][ MAX_PATH ] ;
	CHAR				m_szHumanClassName[ 4 ][ MAX_PATH ] ;
	CHAR				m_szAkhanClassName[ 2 ][ MAX_PATH ] ;
	CHAR				m_szSexName[ 2 ][ MAX_PATH ] ;
	CHAR				m_szHairName[ 7 ][ MAX_PATH ] ;
	CHAR				m_szFaceName[ 7 ][ MAX_PATH ] ;
	CHAR				m_szShirtName[ 6 ][ MAX_PATH ] ;

//	WORK_LIST 1.2 ĳ���� ����â���� ���� �� ������
//	INT					m_iCurrentRace ;
	INT					m_iCurrentClass ;
	INT					m_iCurrentSex ;
	INT					m_iCurrentHair ;
	INT					m_iCurrentFace ;
	INT					m_iCurrentShirt ;

public :
//	WORK_LIST 1.2 ĳ���� ����â���� ���� �� ������
//	CRYLCharacterCreationDlg() ;
	CRYLCharacterCreationDlg( unsigned long dwRace ) ;
	virtual ~CRYLCharacterCreationDlg() ;

	enum StatusType
	{
		TYPE_STR	= 0,
		TYPE_DEX	= 1,
		TYPE_CON	= 2,
		TYPE_INT	= 3,
		TYPE_WIS	= 4,

		MAX_TYPE_NUM = 5
	};

	VOID InitCtrl() ;
	VOID FinalCtrl() ;

	virtual VOID Show( CGMUIObject* Sender ) ;
	virtual VOID Hide( CGMUIObject* Sender ) ;

	virtual VOID GMUIPaint( INT xx, INT yy ) ;

	virtual	HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )			{ return S_OK ; }
	virtual HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice )	{ return S_OK ; }
	virtual HRESULT		Update( BOOL &bClick, BOOL &bEdge )  ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	static VOID DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift ) ; 

	VOID	InitValue() ;
	VOID	DrawBonusPicture() ;
	
	VOID	ChangeModelByRace() ;
	VOID	ChangeModelByClass() ;
	VOID	ChangeModelBySex() ;

//	WORK_LIST 1.2 ĳ���� ����â���� ���� �� ������
//	VOID	RaceNameNext() ;
//	VOID	RaceNamePrev() ;
	VOID	ClassNameNext() ;
	VOID	ClassNamePrev() ;
	VOID	SexNameNext() ;
	VOID	SexNamePrev() ;
	VOID	HairNameNext() ;
	VOID	HairNamePrev() ;
	VOID	FaceNameNext() ;
	VOID	FaceNamePrev() ;
	VOID	ShirtNameNext() ;
	VOID	ShirtNamePrev() ;
	VOID	PointInc(char cType) ;
	VOID	PointDec(char cType) ;
	VOID	CharRightRotate() ;
	VOID	CharLeftRotate() ;
	VOID	MakeButton() ;
	VOID	CancelButton() ;

	inline bool CheckCharCreateName(char *Name_In)
	{
		const unsigned short MIN_CHAR_NAME	= 4;
		const unsigned short MAX_CHAR_NAME	= 20;

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
				for(ACount = 0; ACount < RYLCHATACTERCREATION::ALLOW_HAN_NUM; ACount++)
				{
					if(MAKEWORD(CheckName[LCount + 1], CheckName[LCount])  == RYLCHATACTERCREATION::AllowHans[ACount])
						break;
				}

				if(ACount != RYLCHATACTERCREATION::ALLOW_HAN_NUM)
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
				for(ACount = 0; ACount < RYLCHATACTERCREATION::ALLOW_LETTER_NUM; ACount++)
				{
					if(CheckName[LCount] == RYLCHATACTERCREATION::AllowLetters[ACount])
						break;
				}

				if(ACount == RYLCHATACTERCREATION::ALLOW_LETTER_NUM)
					return false;
			}
		}

		return true;
	}
} ;


#endif //__RYL_CHARACTERCREATION_H__
