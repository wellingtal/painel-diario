//================================================================
//
//	Name : RYLJoystickConfigurePanel.h 
//	Date : 2006.4.18
//  Desc : 조이스틱 설정 페널
//
//================================================================
#ifndef __RYL_JOYSTICKCONFIGUREPANEL_H__
#define __RYL_JOYSTICKCONFIGUREPANEL_H__

#include "GMUICustomPanel.h"

class CRYLLabel;
class CRYLButton;
class CRYLCheckBox;
class CRYLStringGrid;

namespace RYLJOYSTICKCONFIGUREPANEL
{
	const DWORD	CHECK_BOX		= 0x00000000 ;
	const DWORD	KEYSETUP_BUTTON	= 0x00000001 ;
	const DWORD	KEYINIT_BUTTON	= 0x00000002 ;
	const DWORD	PREV_BUTTON		= 0x00000003 ;
	const DWORD	NEXT_BUTTON		= 0x00000004 ;
	const DWORD KEY_STRINGGRID	= 0x00000005 ;
	const DWORD KEYCANLE_BUTTON	= 0x00000006 ;
	const DWORD KEYINPUT_BUTTON	= 0x00000007 ;	
} ;

namespace KeyControl
{
	enum Const
	{
		START_KEY_BUTTON			= 12,
		JOY_INPUT_NONE				= 0,
		JOY_INPUT_PLAYMODE			= 1,
		JOY_INPUT_CHECKMODE			= 2
	};

	enum KeyColum
	{
		KEY_COLUM_FORWARD			= 0,
		KEY_COLUM_BACKWARD			= 1,
		KEY_COLUM_LEFT				= 2,
		KEY_COLUM_RIGHT				= 3,
		KEY_COLUM_DIRECTION_LEFT	= 4,
		KEY_COLUM_DIRECTION_RIGHT	= 5,		
		KEY_COLUM_DIRECTION_UP		= 6,
		KEY_COLUM_DIRECTION_DOWN	= 7,
		KEY_COLUM_CASTING			= 8,
		KEY_COLUM_ATTACK			= 9,
		KEY_COLUM_ZOOMIN			= 10,
		KEY_COLUM_ZOOMOUT			= 11,
		KEY_COLUM_PREV				= 12,
		KEY_COLUM_NEXT				= 13,
		KEY_COLUM_01				= 14,
		KEY_COLUM_02				= 15,
		KEY_COLUM_03				= 16,
		KEY_COLUM_04				= 17,
		KEY_COLUM_05				= 18,
		KEY_COLUM_06				= 19,
		KEY_COLUM_07				= 20,
		KEY_COLUM_08				= 21,
		KEY_COLUM_PARTY				= 22,
		KEY_COLUM_CHAR				= 23,
		KEY_COLUM_SKILL				= 24,
		KEY_COLUM_INVEN				= 25,
		KEY_COLUM_QUEST				= 26,
		KEY_COLUM_REST				= 27,
		KEY_COLUM_ITEM				= 28,
		KEY_COLUM_JUMP				= 29,
		KEY_COLUM_WEAPONCHANGE		= 30,
		KEY_COLUM_MAPONOFF			= 31,
		KEY_COLUM_SELFTARGET		= 32,
	};

	enum KeyConst
	{
		KEY_UP				= 0x00000001,
		KEY_DOWN			= 0x00000002,
		KEY_LEFT			= 0x00000004,
		KEY_RIGHT			= 0x00000008,

		KEY_POV_UP			= 0x00000010,
		KEY_POV_DOWN		= 0x00000020,
		KEY_POV_LEFT		= 0x00000040,
		KEY_POV_RIGHT		= 0x00000080,

		KEY_Z_UP			= 0x00000100,
		KEY_Z_DOWN			= 0x00000200,

		KEY_ZROT_UP			= 0x00000400,
		KEY_ZROT_DOWN		= 0x00000800,

		KEY_BTN_1			= 0x00001000,
		KEY_BTN_2			= 0x00002000,
		KEY_BTN_3			= 0x00004000,
		KEY_BTN_4			= 0x00008000,

		KEY_BTN_5			= 0x00010000,
		KEY_BTN_6			= 0x00020000,
		KEY_BTN_7			= 0x00040000,
		KEY_BTN_8			= 0x00080000,

		KEY_BTN_9			= 0x00100000,
		KEY_BTN_10			= 0x00200000,
		KEY_BTN_11			= 0x00400000,
		KEY_BTN_12			= 0x00800000,
	};

	void Process(void);

	bool GetCheckInputJoystick(void);

	bool IsKeyState(unsigned char cKeyColumn);
	bool IsKeyPress(unsigned char cKeyColumn);

	unsigned long CheckKeyState(unsigned char & cCount);

	void StartForceFeedback(long lForceX, long lForceY, unsigned long dwTick);
};

class CRYLJoystickConfigurePanel : public CGMUICustomPanel
{
public :

	enum Const
	{
		MAX_JOY_KEY_COUNT	= 24,
		MAX_GIRD_COUNT		= 10,
		MAX_KEY_COUNT		= 33,
		MAX_KEY_PAGE		= 3,
	};	

	CRYLJoystickConfigurePanel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLJoystickConfigurePanel() ;

	VOID				InitCtrl() ;
	VOID				FinalCtrl() ;	
	
	HRESULT				Update(BOOL &bClick, BOOL &bEdge);
	HRESULT				Render();
	HRESULT				FinalRender();

	VOID				UpdateKeyPage();

	VOID				UpdateNextKeyPage();
	VOID				UpdatePrevKeyPage();

	VOID				ChangeForceFeedback();

	VOID				SetKeyIndex();

	VOID				AllInitialize();

	unsigned long		GetJoyKey(unsigned char cKeyColumn);

	static VOID			DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;	

	unsigned char		GetForcefeedback(void);	

	VOID				SetInput(unsigned char cInputMode);

	unsigned long		GetTempKey(void) { return m_dwTempKey; }

	unsigned char		GetCount(void) { return m_cCount; }

	VOID				SetCount(unsigned char cCount);

	VOID				SetTempKey(unsigned long dwKey);

	unsigned char		GetInputMode(void) { return m_cInputMode; }

	VOID				SetInitialize(void);

	VOID				SetJoyKey(void);

	VOID				SetForceFeedback(unsigned char cForceFeedBack);

	unsigned long*		GetSettingKey(void) { return m_dwSettingKey; }

	void				SetInitializeKey(void);

	void				SetAllocKey(unsigned char cIndex);

	unsigned char		GetAllocKey(unsigned char cIndex) { return m_cAllocKey[cIndex]; }

	void				SetPressKey(unsigned char cIndex, bool bPressKey);

	bool				GetPressKey(unsigned char cIndex) { return m_bPressKey[cIndex]; }

private:

	CRYLCheckBox*		m_pForceFeedBack;
	CRYLLabel*			m_pForceFeedBackLable;

	CRYLButton*			m_pKeySetup;
	CRYLButton*			m_pKeyInit;

	CRYLButton*			m_pKeyCancel;
	CRYLButton*			m_pKeyIuput;

	CRYLButton*			m_pPrevButton;
	CRYLButton*			m_pNextButton;

	CRYLLabel*			m_pKeyLabel;
	CRYLLabel*			m_pDefaultKeyLabel1;
	CRYLLabel*			m_pDefaultKeyLabel2;
	CRYLLabel*			m_pSetupKeyLabel;

	CRYLStringGrid*		m_pKeyStringGrid;

	CRYLLabel*			m_pKeyDescLable;
	CRYLLabel*			m_pKeyDefaultDescLable;
	CRYLLabel*			m_pKeySetupDescLabel;

	unsigned char		m_cPage;

	int					m_nKeyIndex;

	unsigned long		m_dwDefaultKey[MAX_KEY_COUNT];
	unsigned long		m_dwSettingKey[MAX_KEY_COUNT];
	bool				m_bPressKey[MAX_KEY_COUNT];

	unsigned char		m_cAllocKey[MAX_KEY_COUNT];

	unsigned char		m_cForceFeedback;

	unsigned long		m_dwTempKey;
	unsigned char		m_cCount;

	unsigned char		m_cInputMode;
} ;

#endif