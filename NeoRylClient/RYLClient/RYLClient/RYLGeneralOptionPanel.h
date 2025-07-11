//================================================================
//
//	Name : RYLGeneralOptionPanel.h 
//	Desc : �Ϲ� �ɼ� �ǳ�
//  Date : 2004. 04. 13
//
//================================================================
#ifndef __RYL_GENERALOPTIONPANEL_H__
#define __RYL_GENERALOPTIONPANEL_H__

#include "GMUICustomPanel.h"
#include <list>

class CRYLLabel ;
class CRYLCheckBox ;
class CRYLButton ;

namespace RYLGENERALOPTIONPANEL
{
	const DWORD		DUEL_BUTTON				= 0x00000000 ; // 1 : 1 ��û
	const DWORD		EXCHANGE_BUTTON			= 0x00000001 ; // �ŷ���û�ź�
	const DWORD		PARTY_BUTTON			= 0x00000002 ; // ��Ƽ��û�ź�
	const DWORD		FRIEND_BUTTON			= 0x00000003 ; // ģ����ϰź�
	const DWORD		RECALL_BUTTON			= 0x00000004 ; // ���ݰź�

	const DWORD		WHISPER_BUTTON			= 0x00000005 ; // �ӼӸ� ����
	const DWORD		SHOUT_BUTTON			= 0x00000006 ; // ��ġ�� ����
	const DWORD		TIPBROAD_BUTTON			= 0x00000007 ; // ���� ����
	const DWORD		CHAT_BUTTON				= 0x00000008 ; // �Ϲ� ��ȭ ����
	// �ɸ��� �̸�
	const DWORD		CHARNAME01_BUTTON		= 0x00000009 ; // ��ü ����
	const DWORD		CHARNAME02_BUTTON		= 0x00000010 ; // �Ʊ��� ����
	const DWORD		CHARNAME03_BUTTON		= 0x00000011 ; // ��Ƽ���� ����
	const DWORD		CHARNAME04_BUTTON		= 0x00000012 ; // �̸� ���� ����

	const DWORD		OK_BUTTON				= 0x00000013 ;
	const DWORD		JOY_BUTTON				= 0x00000014 ;
} ;

class CRYLGeneralOptionPanel : public CGMUICustomPanel
{
private :
	CRYLButton*		m_pOkButton ;				// Ȯ��
	CRYLButton*		m_pJoyButton ;				// �����е� ����.

	CRYLLabel*		m_pProposalRefusalLabel ;	// ��û �ź�
	CRYLLabel*		m_pChattingLabel ;			// ��ȭ
	CRYLLabel*		m_pCharacternameLabel ;		// ĳ���� �̸�

	CRYLLabel*	m_pDuelLabel ;					// 1 : 1 ��û
	CRYLLabel*	m_pExchangeLabel ;				// �ŷ���û�ź�
	CRYLLabel*	m_pPartyLabel ;					// ��Ƽ��û�ź�
	CRYLLabel*	m_pFriendLabel ;				// ģ����ϰź�
	CRYLLabel*	m_pRecallLabel ;				// ����

	CRYLLabel*	m_pWhisperLabel ;				// �ӼӸ� ����
	CRYLLabel*	m_pShoutLabel ;					// ��ġ�� ����
	CRYLLabel*	m_pTipBroadLabel ;				// ���� ����
	CRYLLabel*	m_pChatLabel ;					// �Ϲ� ��ȭ ����
	// �ɸ��� �̸�
	CRYLLabel*	m_pCharName01Label ;			// ��ü ����
	CRYLLabel*	m_pCharName02Label ;			// �Ʊ��� ����
	CRYLLabel*	m_pCharName03Label ;			// ��Ƽ���� ����
	CRYLLabel*	m_pCharName04Label ;			// �̸� ���� ����

	CRYLCheckBox*	m_pDuelButton ;				// 1 : 1 ��û
	CRYLCheckBox*	m_pExchangeButton ;			// �ŷ���û�ź�
	CRYLCheckBox*	m_pPartyButton ;			// ��Ƽ��û�ź�
	CRYLCheckBox*	m_pFriendButton ;			// ģ����ϰź�
	CRYLCheckBox*	m_pRecallButton ;			// ��������ź�

	CRYLCheckBox*	m_pWhisperButton ;			// �ӼӸ� ����
	CRYLCheckBox*	m_pShoutButton ;			// ��ġ�� ����
	CRYLCheckBox*	m_pTipBroadButton ;			// ���� ����
	CRYLCheckBox*	m_pChatButton ;				// �Ϲ� ��ȭ ����

	// �ɸ��� �̸�
	CRYLCheckBox*	m_pCharName01Button ;		// ��ü ����
	CRYLCheckBox*	m_pCharName02Button ;		// �Ʊ��� ����
	CRYLCheckBox*	m_pCharName03Button ;		// ��Ƽ���� ����
	CRYLCheckBox*	m_pCharName04Button ;		// �̸� ���� ����
	
public :
	CRYLGeneralOptionPanel( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLGeneralOptionPanel() ;

	VOID	InitCtrl() ;
	VOID	FinalCtrl() ;
	
	HRESULT		Update() ;

	static VOID DlgClick( CGMUIObject* Sender, INT X, INT Y ) ;
	static VOID GeneralOptionMouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) ;

	VOID	ReLoadOption() ;
} ;

#endif //__RYL_GENERALOPTIONPANEL_H__

