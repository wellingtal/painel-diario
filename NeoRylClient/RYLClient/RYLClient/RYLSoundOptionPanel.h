//================================================================
//
//	Name : RYLSoundOptionPanel.h 
//	Desc : 사운드 옵션 판넬
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
	const DWORD		MASTER_PULS_BUTTON			= 0x00000000 ; // 마스터 볼륨 크게
	const DWORD		MASTER_MINUS_BUTTON			= 0x00000001 ; // 마스터 볼륨 작게
	const DWORD		EFFECT_PULS_BUTTON			= 0x00000002 ; // 효과음 볼륨 크게
	const DWORD		EFFECT_MINUS_BUTTON			= 0x00000003 ; // 효과음 볼륨 작게
	const DWORD		BGM_PULS_BUTTON				= 0x00000004 ; // BGM 볼륨 크게
	const DWORD		BGM_MINUS_BUTTON			= 0x00000005 ; // BGM 볼륨 작게
	const DWORD		MASTER_ON_OFF_BUTTON		= 0x00000006 ; // 마스터 켜기/끄기
	const DWORD		EFFECT_ON_OFF_BUTTON		= 0x00000007 ; // 효과음 켜기/끄기
	const DWORD		BGM_ON_OFF_BUTTON			= 0x00000008 ; // BGM 켜기/끄기
	const DWORD		OK_BUTTON					= 0x00000009 ;
} ;

class CRYLSoundOptionPanel : public CGMUICustomPanel
{
private :
	CRYLButton*		m_pOkButton ;				// 확인

	CRYLLabel*			m_pMasterLabel ;			// Master
	CRYLLabel*			m_pBGMLabel ;				// BGM
	CRYLLabel*			m_pEffectLabel ;			// 효과음

	CRYLButton*			m_pMasterPulsButton ;		// Master 크게
	CRYLButton*			m_pMasterMinusButton ;		// Master 작게
	CRYLButton*			m_pBGMPulsButton ;			// BGM 크게
	CRYLButton*			m_pBGMMinusutton ;			// BGM 작게
	CRYLButton*			m_pEffectPulsButton ;		// 효과음 크게
	CRYLButton*			m_pEffectMinusButton ;		// 효과음 작게
	
	CRYLCheckBox*		m_pMasterOnOffButton ;		// BGM 켜기/끄기
	CRYLCheckBox*		m_pBGMOnOffButton ;			// BGM 켜기/끄기
	CRYLCheckBox*		m_pEffectOnOffButton ;		// 효과음 켜기/끄기

	CRYLProgressBar*	m_pMasterProgressBar ;		// Master 프로그레스바
	CRYLProgressBar*	m_pBGMProgressBar ;			// BGM 프로그레스바
	CRYLProgressBar*	m_pEffectProgressBar ;		// 효과음 프로그레스바

	CRYLLabel*			m_pMasterVolumeLabel ;			// Master
	CRYLLabel*			m_pBGMVolumeLabel ;				// BGM
	CRYLLabel*			m_pEffectVolumeLabel ;			// 효과음

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

