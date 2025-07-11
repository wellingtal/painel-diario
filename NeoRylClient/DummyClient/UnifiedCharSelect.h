#pragma once

#include <DB/DBDefine.h>
#include <Network/Packet/PacketStruct/UnifiedCharPacket.h>
#include "afxcmn.h"

// CUnifiedCharSelect 대화 상자입니다.
class CCharacterData;

class CUnifiedCharSelect : public CDialog
{
	DECLARE_DYNAMIC(CUnifiedCharSelect)

public:

// 대화 상자 데이터입니다.
	enum { IDD = IDD_UNIFIED_CHAR_SELECT };

    static CUnifiedCharSelect* CreateUnifiedCharSelect(CCharacterData& characterData, CWnd* pParentWnd = NULL);

    void SetData(USER_INFO& userInfo, 
        UnifiedStoreInfo* lpUnifiedStoreInfo, unsigned long dwUnifiedStoreInfoNum,
        UnifiedCharData*  lpUnifiedCharData, unsigned long dwUnifiedCharDataNum);

protected:

    CUnifiedCharSelect(CCharacterData& characterData, CWnd* pParent = NULL);   // 표준 생성자입니다.
    virtual ~CUnifiedCharSelect();

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()


private:

    bool HasUserInfo(unsigned long dwCID);

    USER_INFO           m_UserInfo;
    
    UnifiedStoreInfo    m_UnifiedStoreInfo[PktUnifiedCharInfo::MAX_STORE_INFO];
    UnifiedCharData     m_UnifiedCharData[PktUnifiedCharInfo::MAX_CHAR_DATA];

    unsigned long       m_dwUnifiedStoreInfoNum;
    unsigned long       m_dwUnifiedCharDataNum;

    CListCtrl           m_StoreInfoList;
    CListCtrl           m_CharDataList;
    CListCtrl           m_SelectedCharList;

    CString             m_UnifiedPassword;
    CString             m_SelectedStore;

    CCharacterData&     m_CharacterData;

    unsigned char       m_cSelectedStore;

protected:

    virtual void OnOK();

public:

    virtual BOOL OnInitDialog();
    afx_msg void OnNMDblclkUnifiedCharList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMDblclkUnifiedStoreList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMDblclkSelectedChar(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNcDestroy();
};
