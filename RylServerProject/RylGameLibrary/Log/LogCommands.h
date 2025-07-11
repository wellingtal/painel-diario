#ifndef _LOGCOMMANDS_H_
#define _LOGCOMMANDS_H_

#include "LogStruct.h"

namespace GAMELOG
{
	namespace CMD
	{
        // Depleted 항목은, 이전 파일과의 호환성을 위해 남겨두는 부분이므로
        // 안 쓰더라도 삭제해서는 안 될 것.

        const CMDType CHAR_LOGIN			= 0x01;	// 캐릭터 로그인
		const CMDType CHAR_LOGOUT			= 0x02;	// 캐릭터 로그아웃
		const CMDType CHAR_CREATE			= 0x03;	// 캐릭터 생성
		const CMDType CHAR_DELETE			= 0x04;	// 캐릭터 삭제
		const CMDType CHAR_DBUPDATE			= 0x05; // DB에 업데이트 할 수 없을 경우 파일로 기록
		const CMDType CHAR_LEVELUP			= 0x06;	// 캐릭터 레벨 업
		const CMDType CHAR_BIND_POS			= 0x07; // 캐릭터 위치 바인드
		const CMDType CHAR_DEAD				= 0x08;	// 캐릭터 사망
		const CMDType CHAR_RESPAWN			= 0x09;	// 캐릭터 리스폰
			
		const CMDType MOVE_ITEM				= 0x10;
		const CMDType SWAP_ITEM				= 0x11;
		const CMDType USE_ITEM				= 0x12;
		const CMDType SPLIT_ITEM			= 0x13;
		const CMDType PICKUP_ITEM			= 0x14;
		const CMDType DROP_ITEM				= 0x15;
		const CMDType BUY_ITEM				= 0x16;
		const CMDType SELL_ITEM				= 0x17;
		const CMDType BEFORE_EXCHANGE_ITEM	= 0x18;
		const CMDType AFTER_EXCHANGE_ITEM	= 0x19;
		const CMDType INSTALL_SOCKET_ITEM	= 0x20;
        const CMDType REPAIR_ITEM           = 0x21;
        const CMDType CHANGE_WEAPON         = 0x22;
        const CMDType TAKE_GOLD             = 0x23; // Depleted. 
        const CMDType UPGRADE_ITEM          = 0x24;

        // 노점상 관련. 2003/05/30 추가 (by sparrowhawk)
        const CMDType STALL_OPEN_CLOSE              = 0x25;
        const CMDType STALL_ENTER_LEAVE             = 0x26;
        const CMDType STALL_ITEM_REGISTER_REMOVE    = 0x27;

        // 메달 상점에서 아이템 사기 추가. 돈이 아니라 마일리지가 깎인다. 2003/07/11 추가 (by Sparrowhawk)
        const CMDType MEDAL_BUY_ITEM        = 0x28;

        // TakeGold Version2. 이동한 금액 및 사유가 추가됨.
        const CMDType TAKE_GOLD_V2          = 0x29;

        // 로터리
        const CMDType USE_LOTTERY	        = 0x2A;

        // 길드 로그 추가 2004/03/22 추가 (by sparrowhawk)
        const CMDType GUILD_CREATE          = 0x30;     // 길드 생성
        const CMDType GUILD_JOIN            = 0x31;     // 길드 가입 신청(가입대기자 레벨로 가입됨)
        const CMDType GUILD_MEMBER_LEVEL    = 0x32;     // 길드 멤버 레벨 조정        
        const CMDType GUILD_LEAVE           = 0x33;     // 길드 탈퇴 (길드원 탈퇴 혹은, 다른 사람이 탈퇴시킴)
        
        const CMDType GUILD_RIGHTS_CHANGE   = 0x34;     // 길드 전체 권한 조정
        const CMDType GUILD_LEVEL_ADJUST    = 0x35;     // 길드 레벨 조정
        const CMDType GUILD_MARK_ADJUST     = 0x36;     // 길드 마크 등록/변경
        const CMDType GUILD_GOLD_CHANGE     = 0x37;     // 길드 창고 돈 입금/출금
                
        const CMDType GUILD_DISSOLVE        = 0x38;     // 길드 소멸

        const CMDType ZONE_MOVE             = 0x39;     // 존 이동

        const CMDType ITEM_ATTACH_OPTION    = 0x3A;     // 아이템 옵션 이식
        const CMDType ITEM_COMPENSATION     = 0x3B;     // 아이템 보상

        const CMDType TICKET_BUY_SKILLBOOK  = 0x3C;     // 스킬북 교환권으로 스킬북 구매
        const CMDType UPGRADE_ITEM_V2       = 0x3D;     // 업그레이드 아이템 V2 (성공/실패여부 들어감)

        const CMDType MONSTER_DEAD          = 0x3E;     // 몬스터 사망(및 그에 따르는 아이템 로그)

		const CMDType FAME_GET_BATTLE		= 0x3F;		// 전투로 얻은 명성
		const CMDType FAME_LOSE_BATTLE		= 0x40;		// 전투로 잃은 명성

		const CMDType FAME_GET_CAMP			= 0x41;		// 요새로 얻은 명성
		const CMDType FAME_LOSE_CAMP		= 0x42;		// 요새로 잃은 명성

		const CMDType QUEST_GET_REWARD		= 0x43;		// 퀘스트 수행으로 얻은 보상

		const CMDType CHANGE_RIDE           = 0x44;

		const CMDType ILLEGAL_ITEM          = 0x45;
		const CMDType ILLEGAL_WARPPOS       = 0x46;
		const CMDType HACK_DOUBT			= 0x47;

		const CMDType MAX_LOGCMD			= 0x48;     // 마지막 커맨드
	};

    /*
     	참고 : 돈 이동에 관련된 커맨드들
     
        const CMDType PICKUP_ITEM			= 0x14;
        const CMDType DROP_ITEM				= 0x15;
       
        const CMDType BUY_ITEM				= 0x16;
        const CMDType SELL_ITEM				= 0x17;
        const CMDType BEFORE_EXCHANGE_ITEM	= 0x18;
        const CMDType AFTER_EXCHANGE_ITEM	= 0x19;
        const CMDType REPAIR_ITEM           = 0x21;
        const CMDType TAKE_GOLD             = 0x23;
        const CMDType UPGRADE_ITEM          = 0x24; 

    */

    //  아직 쓰고 있지 않음...
	namespace ERR
	{
		const ERRType NO_LOG_ERR			= 0x00;	// 에러 아님
		const ERRType CANNOT_UPDATE_DB		= 0x01;	// DB에 업데이트 할 수 없습니다.
		const ERRType CANNOT_UPDATE_CLIENT	= 0x02;	// 클라이언트에 업데이트 할 수 없습니다.
		const ERRType CANNOT_GET_DATA		= 0x03; // 데이터를 얻어 올 수 없습니다.
	};

};


#endif