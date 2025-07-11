// UnifiedCharSelect.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DummyClient.h"
#include "UnifiedCharSelect.h"
#include "CharacterData.h"

#include <Network/Packet/PacketStruct/CharItemPacket.h>
#include <Network/ClientSocket/Send/SendAuthServer.h>
#include ".\unifiedcharselect.h"
#include "GMMemory.h"

// CUnifiedCharSelect 대화 상자입니다.

IMPLEMENT_DYNAMIC(CUnifiedCharSelect, CDialog)
CUnifiedCharSelect::CUnifiedCharSelect(CCharacterData& characterData, CWnd* pParent /*=NULL*/)
	: CDialog(CUnifiedCharSelect::IDD, pParent), m_CharacterData(characterData)
    , m_UnifiedPassword(_T(""))
    , m_SelectedStore(_T(""))
    , m_cSelectedStore(0)
{
    memset(&m_UserInfo, 0, sizeof(USER_INFO));

    memset(m_UnifiedStoreInfo, 0, sizeof(UnifiedStoreInfo) * PktUnifiedCharInfo::MAX_STORE_INFO);
    memset(m_UnifiedCharData, 0, sizeof(UnifiedCharData) * PktUnifiedCharInfo::MAX_CHAR_DATA);

    m_dwUnifiedStoreInfoNum = 0;
    m_dwUnifiedCharDataNum  = 0;
}

CUnifiedCharSelect::~CUnifiedCharSelect()
{

}

void CUnifiedCharSelect::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_UNIFIED_STORE_LIST, m_StoreInfoList);
    DDX_Control(pDX, IDC_UNIFIED_CHAR_LIST, m_CharDataList);
    DDX_Control(pDX, IDC_SELECTED_CHAR, m_SelectedCharList);
    DDX_Text(pDX, IDC_UNIFIED_PASSWORD, m_UnifiedPassword);
    DDX_Text(pDX, IDC_SELECTED_STORE, m_SelectedStore);
}


BEGIN_MESSAGE_MAP(CUnifiedCharSelect, CDialog)
    ON_NOTIFY(NM_DBLCLK, IDC_UNIFIED_CHAR_LIST, OnNMDblclkUnifiedCharList)
    ON_NOTIFY(NM_DBLCLK, IDC_UNIFIED_STORE_LIST, OnNMDblclkUnifiedStoreList)
    ON_NOTIFY(NM_DBLCLK, IDC_SELECTED_CHAR, OnNMDblclkSelectedChar)
    ON_WM_NCDESTROY()
END_MESSAGE_MAP()


// CUnifiedCharSelect 메시지 처리기입니다.

BOOL CUnifiedCharSelect::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    
    m_StoreInfoList.InsertColumn(0, "ServerGroupID", LVCFMT_LEFT, 100);
    m_StoreInfoList.InsertColumn(1, "TabNum", LVCFMT_LEFT, 60);
    m_StoreInfoList.InsertColumn(2, "Gold", LVCFMT_LEFT, 60);

    m_CharDataList.InsertColumn(0, "CID", LVCFMT_LEFT, 80);
    m_CharDataList.InsertColumn(1, "OldServerGroupID", LVCFMT_LEFT, 40);
    m_CharDataList.InsertColumn(2, "Name", LVCFMT_LEFT, 120);
    m_CharDataList.InsertColumn(3, "Class", LVCFMT_LEFT, 60);
    m_CharDataList.InsertColumn(4, "Level", LVCFMT_LEFT, 60);

    m_SelectedCharList.InsertColumn(0, "CID", LVCFMT_LEFT, 80);
    m_SelectedCharList.InsertColumn(1, "Name", LVCFMT_LEFT, 120);
    m_SelectedCharList.InsertColumn(2, "Class", LVCFMT_LEFT, 60);
    m_SelectedCharList.InsertColumn(3, "Level", LVCFMT_LEFT, 60);

    m_UnifiedPassword = m_CharacterData.GetPassword();
    UpdateData(false);

    return TRUE;  // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CUnifiedCharSelect::SetData(USER_INFO& userInfo, 
                                 UnifiedStoreInfo* lpUnifiedStoreInfo, unsigned long dwUnifiedStoreInfoNum,
                                 UnifiedCharData*  lpUnifiedCharData, unsigned long dwUnifiedCharDataNum)
{
    // 데이터를 넣고, 리스트에 추가하고, 화면을 갱신한다.
    m_UserInfo = userInfo;

    memcpy(m_UnifiedStoreInfo, lpUnifiedStoreInfo, 
        sizeof(UnifiedStoreInfo) * min(dwUnifiedStoreInfoNum, unsigned long(PktUnifiedCharInfo::MAX_STORE_INFO)));

    memcpy(m_UnifiedCharData,  lpUnifiedCharData, 
        sizeof(UnifiedCharData) * min(dwUnifiedCharDataNum, unsigned long(PktUnifiedCharInfo::MAX_CHAR_DATA)));

    m_dwUnifiedStoreInfoNum = dwUnifiedStoreInfoNum;
    m_dwUnifiedCharDataNum  = dwUnifiedCharDataNum;

    const int MAX_BUFFER = 1024;
    char szTempBuffer[MAX_BUFFER];

    // 리스트박스에 창고 데이터를 추가한다.
    for(unsigned long dwCount = 0; dwCount < dwUnifiedStoreInfoNum; ++dwCount)
    {
        unsigned long dwTabFlagNum = 0;

        if(m_UnifiedStoreInfo[dwCount].Flag & PktDepositUpdateDB::ITEM_TAB1) { ++dwTabFlagNum; }
        if(m_UnifiedStoreInfo[dwCount].Flag & PktDepositUpdateDB::ITEM_TAB2) { ++dwTabFlagNum; }
        if(m_UnifiedStoreInfo[dwCount].Flag & PktDepositUpdateDB::ITEM_TAB3) { ++dwTabFlagNum; }
        if(m_UnifiedStoreInfo[dwCount].Flag & PktDepositUpdateDB::ITEM_TAB4) { ++dwTabFlagNum; }

        m_StoreInfoList.InsertItem(dwCount, ultoa(m_UnifiedStoreInfo[dwCount].cOldServerGroupID, szTempBuffer, 10));
        m_StoreInfoList.SetItemText(dwCount, 1, ultoa(dwTabFlagNum, szTempBuffer, 10));
        m_StoreInfoList.SetItemText(dwCount, 2, ultoa(m_UnifiedStoreInfo[dwCount].Gold, szTempBuffer, 10));
    }

    // 리스트박스에 캐릭터 데이터를 추가한다.
    unsigned long dwItemIndex = 0;
    unsigned long dwSelectedItemIndex = 0;

    for(unsigned long dwCount = 0; dwCount < dwUnifiedCharDataNum; ++dwCount)
    {
        unsigned long dwViewCount = 0;

        for(; dwViewCount < USER_INFO::MAX_CHAR_NUM; ++dwViewCount)
        {
            if (0 != m_UserInfo.CharID[dwViewCount] && 
                m_UnifiedCharData[dwCount].dwNewCID == m_UserInfo.CharID[dwViewCount])
            {
                m_SelectedCharList.InsertItem(dwSelectedItemIndex, ultoa(m_UnifiedCharData[dwCount].dwNewCID, szTempBuffer, 10));
                m_SelectedCharList.SetItemText(dwSelectedItemIndex, 1, m_UnifiedCharData[dwCount].szName);
                m_SelectedCharList.SetItemText(dwSelectedItemIndex, 2, ultoa(m_UnifiedCharData[dwCount].sClass, szTempBuffer, 10));
                m_SelectedCharList.SetItemText(dwSelectedItemIndex, 3, ultoa(m_UnifiedCharData[dwCount].cLevel, szTempBuffer, 10));

                dwViewCount += USER_INFO::MAX_CHAR_NUM;
                ++dwSelectedItemIndex;
            }
        }

        if (dwViewCount == USER_INFO::MAX_CHAR_NUM)
        {
            m_CharDataList.InsertItem(dwItemIndex, ultoa(m_UnifiedCharData[dwCount].dwNewCID, szTempBuffer, 10));
            m_CharDataList.SetItemText(dwItemIndex, 1, ultoa(m_UnifiedCharData[dwCount].cOldServerGroupID, szTempBuffer, 10));
            m_CharDataList.SetItemText(dwItemIndex, 2, m_UnifiedCharData[dwCount].szName);
            m_CharDataList.SetItemText(dwItemIndex, 3, ultoa(m_UnifiedCharData[dwCount].sClass, szTempBuffer, 10));
            m_CharDataList.SetItemText(dwItemIndex, 4, ultoa(m_UnifiedCharData[dwCount].cLevel, szTempBuffer, 10));
            ++dwItemIndex;
        }
    }

    const char* szWindowText = "Unknown";

    switch(m_CharacterData.GetAgentServerType())
    {
    case UnifiedConst::Part2Unified:
        szWindowText = "파트2 통합서버(한국)";
        break;

    case UnifiedConst::Part2Selectable:
        szWindowText = "파트2 통합서버(중국)";
        break;
    }

    SetWindowText(szWindowText);
}

bool CUnifiedCharSelect::HasUserInfo(unsigned long dwCID)
{
    for(int nViewCount = 0; nViewCount < USER_INFO::MAX_CHAR_NUM; ++nViewCount)
    {
        if(m_UserInfo.CharID[nViewCount] == dwCID)
        {
            return true;
        }
    }

    return false;
}

void CUnifiedCharSelect::OnOK()
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
    UpdateData(true);

    unsigned long ardwCID[USER_INFO::MAX_CHAR_NUM];
    unsigned long ardwViewCID[USER_INFO::MAX_CHAR_NUM];

    memset(ardwCID, 0, sizeof(unsigned long) * USER_INFO::MAX_CHAR_NUM);
    memset(ardwViewCID, 0, sizeof(unsigned long) * USER_INFO::MAX_CHAR_NUM);

    if(UnifiedConst::Part2Unified == m_CharacterData.GetAgentServerType())
    {
        for(int nViewCount = 0; nViewCount < USER_INFO::MAX_CHAR_NUM; ++nViewCount)
        {
            ardwViewCID[nViewCount] = m_UserInfo.CharID[nViewCount];
        }

        CString szCID;
        char*   lpStopPos = 0;

        int nMaxCount = m_SelectedCharList.GetItemCount();
        for (int nCount = 0; nCount < nMaxCount; ++nCount)
        {
            szCID = m_SelectedCharList.GetItemText(nCount, 0);

            for(int nViewCount = 0; nViewCount < USER_INFO::MAX_CHAR_NUM; ++nViewCount)
            {
                unsigned long dwCID = strtoul(szCID, &lpStopPos, 10);

                if(0 == m_UserInfo.CharID[nViewCount] && !HasUserInfo(dwCID))
                {
                    m_UserInfo.CharID[nViewCount] = ardwCID[nViewCount] = dwCID;
                    nViewCount = USER_INFO::MAX_CHAR_NUM;
                }
            }
        }
        
		unsigned char cSelectedNation = 0 ;

        SendPacket::UnifiedCharSelect(&m_CharacterData.GetClientSocket().GetHandler(ClientSocket::AuthEventHandler), 
            m_UnifiedPassword, ardwCID, USER_INFO::MAX_CHAR_NUM, m_cSelectedStore, cSelectedNation);

        CDialog::OnOK();
    }
    else if(UnifiedConst::Part2Selectable == m_CharacterData.GetAgentServerType() && 0 != m_cSelectedStore)
    {
        /*
        SendPacket::UnifiedCharSelect(m_CharacterData.GetClientSocket().GetSession(
            ClientSocket::AuthEventHandler), m_UnifiedPassword, ardwCID, 
            USER_INFO::MAX_CHAR_NUM, m_cSelectedStore);
        */

        CDialog::OnOK();
    }
}


void CUnifiedCharSelect::OnNMDblclkUnifiedCharList(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    *pResult = 0;

    // 이미 선택한 캐릭터가 아니면, 선택 추가한다.
    if(NM_DBLCLK == pNMHDR->code && m_SelectedCharList.GetItemCount() < USER_INFO::MAX_CHAR_NUM)
    {
        POSITION pos = m_CharDataList.GetFirstSelectedItemPosition();

        if (pos != NULL)
        {
            UpdateData(true);

            int nItem = m_CharDataList.GetNextSelectedItem(pos);

            CString szCID = m_CharDataList.GetItemText(nItem, 0);
            char* stopPtr = 0;

            unsigned long dwCharDataCID = strtoul(szCID, &stopPtr, 10);

            int nMaxCount = m_SelectedCharList.GetItemCount();
            for (int nCount = 0; nCount < nMaxCount; ++nCount)
            {
                szCID = m_SelectedCharList.GetItemText(nCount, 0);

                if(dwCharDataCID == strtoul(szCID, &stopPtr, 10))
                {
                    break;
                }
            }

            if (nCount == nMaxCount)
            {
                m_SelectedCharList.InsertItem(nMaxCount, m_CharDataList.GetItemText(nItem, 0));
                m_SelectedCharList.SetItemText(nMaxCount, 1, m_CharDataList.GetItemText(nItem, 2));
                m_SelectedCharList.SetItemText(nMaxCount, 2, m_CharDataList.GetItemText(nItem, 3));
                m_SelectedCharList.SetItemText(nMaxCount, 3, m_CharDataList.GetItemText(nItem, 4));
                UpdateData(false);
            }
        }
    }
}

void CUnifiedCharSelect::OnNMDblclkUnifiedStoreList(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    *pResult = 0;

    if(NM_DBLCLK == pNMHDR->code)
    {
        POSITION pos = m_StoreInfoList.GetFirstSelectedItemPosition();

        if (pos != NULL)
        {
            UpdateData(true);

            int nItem = m_StoreInfoList.GetNextSelectedItem(pos);
            CString szStoreID = m_StoreInfoList.GetItemText(nItem, 0);
            
            m_SelectedStore.Format("Selected Store : %s", szStoreID);
            m_cSelectedStore = atoi(szStoreID);

            UpdateData(false);
        }
    }
}

void CUnifiedCharSelect::OnNMDblclkSelectedChar(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    *pResult = 0;

    // 이미 선택한 캐릭터가 아니면 목록에서 삭제한다.
    if(NM_DBLCLK == pNMHDR->code && UnifiedConst::Part2Unified == m_CharacterData.GetAgentServerType())
    {
        POSITION pos = m_SelectedCharList.GetFirstSelectedItemPosition();

        if (pos != NULL)
        {
            UpdateData(true);

            int nItem = m_SelectedCharList.GetNextSelectedItem(pos);

            CString szCID = m_SelectedCharList.GetItemText(nItem, 0);
            char* stopPtr = 0;

            unsigned long dwCID = strtoul(szCID, &stopPtr, 10);

            for(unsigned long dwCount = 0; dwCount < USER_INFO::MAX_CHAR_NUM; ++dwCount)
            {
                if (0 != m_UserInfo.CharID[dwCount] && dwCID == m_UserInfo.CharID[dwCount])
                {
                    break;
                }
            }

            if (dwCount == USER_INFO::MAX_CHAR_NUM)
            {
                m_SelectedCharList.DeleteItem(nItem);
                UpdateData(false);
            }
        }
    }
}

void CUnifiedCharSelect::OnNcDestroy()
{
    CDialog::OnNcDestroy();

    // TODO: 여기에 메시지 처리기 코드를 추가합니다.

    delete this;
}


CUnifiedCharSelect* CUnifiedCharSelect::CreateUnifiedCharSelect(CCharacterData& characterData, CWnd* pParentWnd)
{
#include "GMUndefNew.h"
    CUnifiedCharSelect* lpUnifiedCharSelect = 
        new CUnifiedCharSelect(characterData, pParentWnd);
#include "GMRedefNew.h"

    if(0 != lpUnifiedCharSelect)
    {
        lpUnifiedCharSelect->Create(IDD, AfxGetMainWnd());
    }

    return lpUnifiedCharSelect;
}
