#include "DailyLoginRewardSystem.h"
#include "GUIDailyLoginPanel.h"
#include "LoginRewardIntegration.h"

void LoginRewardIntegration::CheckAndShowLoginWindow() {
    if (!DailyLoginRewardSystem::HasLoggedInToday()) {
        auto items = DailyLoginRewardSystem::GetRewardItems();
        auto* loginPanel = new CGUIDailyLoginPanel();
        loginPanel->SetRewardItems(items);
        loginPanel->Show(nullptr);
    }
}
