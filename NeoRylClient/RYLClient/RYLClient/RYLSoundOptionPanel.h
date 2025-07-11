//================================================================
//
//	Name : RYLSoundOptionPanel.h 
//	Desc : ���� �ɼ� �ǳ�
//  Date : 2004. 04. 13
//
//================================================================
#ifndef __RYL_SOUNDOPTIONPANEL_H__
#define __RYL_SOUNDOPTIONPANEL_H__

#include "GMUICustomPanel.h"
#include <list>

class CRYLLabel ;
class CRYLCheckBox ;
class CRYLButton ;
class CRYLProgressBar ;

namespace RYLSOUNDOPTIONPANEL
{
	const DWORD		MASTER_PULS_BUTTON			= 0x00000000 ; // ������ ���� ũ��
	const DWORD		MASTER_MINUS_BUTTON			= 0x00000001 ; // ������ ���� �۰�
	const DWORD		EFFECT_PULS_BUTTON			= 0x00000002 ; // ȿ���� ���� ũ��
	const DWORD		EFFECT_MINUS_BUTTON			= 0x00000003 ; // ȿ���� ���� �۰�
	const DWORD		BGM_PULS_BUTTON				= 0x00000004 ; // BGM ���� ũ��
	const DWORD		BGM_MINUS_BUTTON			= 0x00000005 ; // BGM ���� �۰�
	const DWORD		MASTER_ON_OFF_BUTTON		= 0x00000006 ; // ������ �ѱ�/����
	const DWORD		EFFECT_ON_OFF_BUTTON		= 0x00000007 ; // ȿ���� �ѱ�/����
	const DWORD		BGM_ON_OFF_BUTTON			= 0x00000008 ; // BGM �ѱ�/����
	const DWORD		OK_BUTTON					= 0x00000009 ;
} ;

class CRYLSoundOptionPanel : public CGMUICustomPanel
{
private :
	CRYLButton*		m_pOkButton ;				// Ȯ��

	CRYLLabel*			m_pMasterLabel ;			// Master
	CRYLLabel*			m_pBGMLabel ;				// BGM
	CRYLLabel*			m_pEffectLabel ;			// ȿ����

	CRYLButton*			m_pMasterPulsButton ;		// Master ũ��
	CRYLButton*			m_pMasterMinusButton ;		// Master �۰�
	CRYLButton*			m_pBGMPulsButton ;			// BGM ũ��
	CRYLButton*			m_pBGMMinusutton ;			// BGM �۰�
	CRYLButton*			m_pEffectPulsButton ;		// ȿ���� ũ��
	CRYLButton*			m_pEffectMinusButton ;		// ȿ���� �۰�
	
	CRYLCheckBox*		m_pMasterOnOffButton ;		// BGM �ѱ�/����
	CRYLCheckBox*		m_pBGMOnOffButton ;			// BGM �ѱ�/����
	CRYLCheckBox*		m_pEffectOnOffButton ;		// ȿ���� �ѱ�/����

	CRYLProgressBar*	m_pMasterProgressBar ;		// Master ���α׷�����
	CRYLProgressBar*	m_pBGMProgressBar ;			// BGM ���α׷�����
	CRYLProgressBar*	m_pEffectProgressBar ;		// ȿ���� ���α׷�����

	CRYLLabel*			m_pMasterVolumeLabel ;			// Master
	CRYLLabel*			m_pBGMVolumeLabel ;				// BGM
	CRYLLabel*			m_pEffectVolumeLabel ;			// ȿ����

	float				m_fMasterVolume; 
	float				m_fBgmVolume; 
	float				m_fAmbVolume; 

	float				m_fOldMasterVolume ;
	float				m_fOldBGMVolume ;
	float				m_fOldAmbVolume ;
	
public :
	CRYLSoundOptionPanel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLSoundOptionPanel() ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;
	
	HRESULT		Update() ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	static VOID DlgMouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;

	VOID	ReLoadOption() ;	

	VOID	SetMasterVolume(float fMasterVolume) { m_fOldMasterVolume = fMasterVolume; m_fMasterVolume = fMasterVolume; } 
	VOID	SetBgmVolume(float fBgmVolume) { m_fOldBGMVolume = fBgmVolume; m_fBgmVolume = fBgmVolume; } 
	VOID	SetAmbVolume(float fAmbVolume) { m_fOldAmbVolume = fAmbVolume; m_fAmbVolume = fAmbVolume; } 
} ;

#endif //__RYL_SOUNDOPTIONPANEL_H__

