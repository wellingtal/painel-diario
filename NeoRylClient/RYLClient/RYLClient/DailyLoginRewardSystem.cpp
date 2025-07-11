
#include "DailyLoginRewardSystem.h"
#include <ctime>

static bool s_hasLoggedInToday = false;
static std::vector<DailyRewardItem> s_items;

bool DailyLoginRewardSystem::HasLoggedInToday() {
    return s_hasLoggedInToday;
}

void DailyLoginRewardSystem::MarkLoginToday() {
    s_hasLoggedInToday = true;
}

std::vector<DailyRewardItem> DailyLoginRewardSystem::GetRewardItems() {
    if (s_items.empty()) {
        s_items.clear();

DailyRewardItem item1; item1.itemName = "HP & MP +6000"; item1.claimed = false; s_items.push_back(item1);
DailyRewardItem item2; item2.itemName = "Coin Medals (100)"; item2.claimed = false; s_items.push_back(item2);
DailyRewardItem item3; item3.itemName = "Life Return"; item3.claimed = false; s_items.push_back(item3);
DailyRewardItem item4; item4.itemName = "Holy Orb of Exp"; item4.claimed = false; s_items.push_back(item4);
DailyRewardItem item5; item5.itemName = "Repair Item"; item5.claimed = false; s_items.push_back(item5);
DailyRewardItem item6; item6.itemName = "Magnifying Glass"; item6.claimed = false; s_items.push_back(item6);
DailyRewardItem item7; item7.itemName = "MonsterHunter Quest Item"; item7.claimed = false; s_items.push_back(item7);

        };
 //   }
    return s_items;
}
