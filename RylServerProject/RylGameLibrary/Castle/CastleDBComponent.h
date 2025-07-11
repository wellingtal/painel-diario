#ifndef _RYL_CASTLE_DB_COMPONENT_H_
#define _RYL_CASTLE_DB_COMPONENT_H_

// forward decl.
struct POS;
struct TIME;

class CDBComponent;

namespace DBComponent
{
    namespace CastleDB
    {
        // --------------------------------------------------------------------------------------------
        // 공성 및 성 관련 - CastleDBComponent.cpp ----------------------------------------------------

		bool InsertCastleMineralInfo(CDBComponent& DBComponent, unsigned long dwCastleID, unsigned char cMineralType, unsigned short wMineralID, unsigned short wAmount);
        bool UpdateCastleInfo(CDBComponent& DBComponent, unsigned long dwCastleID, unsigned char cNation, unsigned char cSiegeCount,
							  unsigned char cInvincibleCount, unsigned short wTotalSiegeCount, unsigned long dwTotalTaxMoney,
							  unsigned short wItemID, unsigned char cItemNum);
		bool UpdateCastleUpgradeItemInfo(CDBComponent& DBComponent, unsigned long dwCastleID, unsigned short wItemID, unsigned char cItemNum);
		bool UpdateCastleSiegeCount(CDBComponent& DBComponent, unsigned long dwCastleID, unsigned char cSiegeCount, unsigned char cInvincibleCount, unsigned short wTotalSiegeCount);
		bool UpdateCastleTotalTaxMoney(CDBComponent& DBComponent, unsigned long dwCastleID, unsigned long dwTotalTaxMoney);
        bool UpdateCastleRight(CDBComponent& DBComponent, unsigned long dwCastleID, char* szRight, unsigned short wSize);
        bool UpdateCastleTax(CDBComponent& DBComponent, unsigned long dwCastleID, unsigned char cType, unsigned char cTax);
		bool UpdateCastleTaxChangable(CDBComponent& DBComponent, unsigned long dwCastleID, unsigned char cType, unsigned char cTaxChangable);
		bool UpdateCastleTaxMoney(CDBComponent& DBComponent, unsigned long dwCastleID, unsigned char cType, unsigned long dwTempTaxMoney, unsigned long dwTaxMoney);
		bool UpdateCastleMineralInfo(CDBComponent& DBComponent, unsigned long dwCastleID, unsigned char cMineralType, unsigned short wMineralID, unsigned short wAmount);
		bool DeleteCastleTaxInfo(CDBComponent& DBComponent, unsigned long dwCastleID);
		bool DeleteCastleMineralInfo(CDBComponent& DBComponent, unsigned long dwCastleID);
		bool DeleteCastleMineralInfo(CDBComponent& DBComponent, unsigned long dwCastleID, unsigned char cMineralType);
		bool DeleteCastleMineralInfo(CDBComponent& DBComponent, unsigned long dwCastleID, unsigned char cMineralType, unsigned short wMineralID);

        // 수성 병기
        bool UpdateCastleObjectHP(CDBComponent& DBComponent, unsigned long dwCID, unsigned long dwHP);
        bool UpdateCastleObjectType(CDBComponent& DBComponent, unsigned long dwCID, unsigned short wObjectType, unsigned long dwOwnerID);
        bool UpdateCastleObjectState(CDBComponent& DBComponent, unsigned long dwCID, unsigned char cState, unsigned char cSubState);
        bool UpdateCastleObjectUpgrade(CDBComponent& DBComponent, unsigned long dwCID, unsigned char cUpgradeStep, unsigned char cUpgradeType);
        bool UpdateCastleObjectTime(CDBComponent& DBComponent, unsigned long dwCID, SYSTEMTIME RemainTime);
		bool UpdateCastleObjectLastUseTime(CDBComponent& DBComponent, unsigned long dwCID, SYSTEMTIME LastUseTime);

        // 길드 요새
        bool InsertCampInfo(CDBComponent& DBComponent, unsigned long dwGID, unsigned short wObjectType, unsigned char cZone, unsigned char cChannel, const POS& Pos, unsigned long* dwCampID, unsigned short* wError);
		bool InsertMiningCampMineralInfo(CDBComponent& DBComponent, unsigned long dwCampID, unsigned char cMineralType, unsigned short wMineralID, unsigned short wAmount);
		bool InsertCampShopInfo(CDBComponent& DBComponent, unsigned long dwCampID);
		bool InsertWorldWeaponInfo(CDBComponent& DBComponent, unsigned char cZone, unsigned char cChannel, unsigned char cNation);
		bool DeleteCampInfo(CDBComponent& DBComponent, unsigned long dwCampID, unsigned short wCampType);
		bool DeleteMiningCampMineralInfo(CDBComponent& DBComponent, unsigned long dwCampID, unsigned char cMineralType);
		bool DeleteMiningCampMineralInfo(CDBComponent& DBComponent, unsigned long dwCampID, unsigned char cMineralType, unsigned short wMineralID);
		bool DeleteCampShopInfo(CDBComponent& DBComponent, unsigned long dwCampID);
		bool DeleteWorldWeaponInfo(CDBComponent& DBComponent, unsigned char cZone, unsigned char cChannel);
        bool UpdateCampRight(CDBComponent& DBComponent, unsigned long dwCampID, char* szRight, unsigned short wSize);
        bool UpdateCampHP(CDBComponent& DBComponent, unsigned long dwCampID, unsigned long dwHP);
		bool UpdateCampType(CDBComponent& DBComponent, unsigned long dwCampID, unsigned short wType);
        bool UpdateCampState(CDBComponent& DBComponent, unsigned long dwCampID, unsigned char cState);
		bool UpdateCampSubState(CDBComponent& DBComponent, unsigned long dwCampID, unsigned char cSubState);
        bool UpdateCampUpgrade(CDBComponent& DBComponent, unsigned long dwCampID, unsigned char cUpgradeStep);
        bool UpdateCampTime(CDBComponent& DBComponent, unsigned long dwCampID, unsigned char cType, SYSTEMTIME TimeValue);
		bool UpdateMiningCampMineralInfo(CDBComponent& DBComponent, unsigned long dwCampID, unsigned char cMineralType, unsigned short wMineralID, unsigned short wAmount);
		bool UpdateCampShopInfo(CDBComponent& DBComponent, unsigned long dwCampID, const char* lpItemBuffer, unsigned long dwItemBufferSize, unsigned char cItemNum, unsigned long dwTempSafe, unsigned char cTax);
		bool UpdateFertilityInfo(CDBComponent& DBComponent, unsigned char cZone, unsigned char cChannel, unsigned long dwVeinColor, unsigned long dwFertility);
		bool UpdateWorldWeaponInfo(CDBComponent& DBComponent, unsigned char cZone, unsigned char cChannel, unsigned char cKarRemainSiegeTime, unsigned char cMerRemainSiegeTime);
		bool UpdateCampMaterial(CDBComponent& DBComponent, unsigned long dwCampID, unsigned char cMaterial);
		bool UpdateCampSiegeCount(CDBComponent& DBComponent, unsigned long dwCampID, unsigned char cSiegeCount);
    }
}

#endif