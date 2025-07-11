
#pragma once

#include "GMUICustomPanel.h"
#include "RYLLabel.h"
#include "RYLButton.h"
#include "GMUICustomDlg.h"
#include <vector>
#include <string>
#include "DailyLoginRewardSystem.h"

class CGUIDailyLoginPanel : public CGMUICustomDlg {
public:
    CGUIDailyLoginPanel();
    virtual ~CGUIDailyLoginPanel();

    void InitCtrl(void);
    void FinalCtrl(void);

    virtual void Show(CGMUIObject* Sender);
    virtual void Hide(CGMUIObject* Sender);
    virtual HRESULT Update(BOOL &bClick, BOOL &bEdge);

    void SetRewardItems(const std::vector<DailyRewardItem>& items);

private:
    std::vector<CRYLLabel*> m_RewardLabels;
    std::vector<CRYLButton*> m_ClaimButtons;
    std::vector<DailyRewardItem> m_RewardItems;

    void OnClaimButtonClick(int index);
};
