
#pragma once
#include <string>
#include <vector>

struct DailyRewardItem {
    std::string itemName;
    bool claimed;
    unsigned short protoID;
};

class DailyLoginRewardSystem {
public:
    static bool HasLoggedInToday();
    static void MarkLoginToday();
    static std::vector<DailyRewardItem> GetRewardItems();
};
