
#include "GUIDailyLoginPanel.h"
#include "RYLGameData.h"
#include "CharStatus.h"
#include "ItemInstance.h"
#include "RYLLabel.h"
#include "RYLButton.h"
#include "RYLMessageBoxManager.h"
#include <time.h>

CGUIDailyLoginPanel::CGUIDailyLoginPanel() {
    InitCtrl();
}

CGUIDailyLoginPanel::~CGUIDailyLoginPanel() {
    FinalCtrl();
}

void CGUIDailyLoginPanel::InitCtrl() {
    const int startX = 40;
    const int startY = 50;
    const int gapY = 50;

    for (size_t i = 0; i < m_RewardItems.size(); ++i) {
        DailyRewardItem item = m_RewardItems[i];

        CRYLLabel* label = new CRYLLabel(this);
        label->SetLeft(startX);
        label->SetTop(startY + gapY * (int)i);
        label->SetCaption((LPSTR)item.itemName.c_str());
        AttachGMUIControl(label);
        m_RewardLabels.push_back(label);

        CRYLButton* button = new CRYLButton(this);
        button->SetLeft(startX + 220);
        button->SetTop(startY + gapY * (int)i);
        button->SetCaption((LPSTR)(item.claimed ? "Recebido" : "Resgatar"));
        AttachGMUIControl(button);
        m_ClaimButtons.push_back(button);
    }
}

void CGUIDailyLoginPanel::FinalCtrl() {
    for (size_t i = 0; i < m_RewardLabels.size(); ++i)
        delete m_RewardLabels[i];
    m_RewardLabels.clear();

    for (size_t i = 0; i < m_ClaimButtons.size(); ++i)
        delete m_ClaimButtons[i];
    m_ClaimButtons.clear();
}

void CGUIDailyLoginPanel::Show(CGMUIObject* Sender) {
    SetVisible(TRUE);
}

void CGUIDailyLoginPanel::Hide(CGMUIObject* Sender) {
    SetVisible(FALSE);
}

HRESULT CGUIDailyLoginPanel::Update(BOOL &bClick, BOOL &bEdge) {
    return S_OK;
}

void CGUIDailyLoginPanel::SetRewardItems(const std::vector<DailyRewardItem>& items) {
    m_RewardItems = items;
    InitCtrl();
}

void CGUIDailyLoginPanel::OnClaimButtonClick(int index) {
    if (index >= 0 && index < (int)m_RewardItems.size()) {
        DailyRewardItem& item = m_RewardItems[index];
        if (!item.claimed) {
            item.claimed = true;
            m_ClaimButtons[index]->SetCaption((LPSTR)"Recebido");

            CItemInstance* pNewItem = new CItemInstance();
            pItem->Init(item.protoID, 1, TRUE);
            CRYLGameData::Instance()->m_csStatus.AddTempInventory(pNewItem);
            CRYLGameData::Instance()->m_csStatus.m_dwTempInventoryTime = timeGetTime();
        }
    }
}
