#include "MapRewardDayToItem.h"

int MapRewardDayToItem(int day) {
    switch (day) {
        case 1: return 3101; // HP&MP +6000
        case 2: return 4123; // Coin Medals (100)
        case 3: return 5244; // Life Return
        case 4: return 6332; // Holy Orb of Exp
        case 5: return 7888; // Repair Item
        case 6: return 8991; // Magnifying Glass
        case 7: return 9455; // Quest MonsterHunter
        default: return 0;   // Invalid day
    }
}
