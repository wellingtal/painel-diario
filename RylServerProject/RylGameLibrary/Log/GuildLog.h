#ifndef _GAMELOG_GUILD_LOG_H_
#define _GAMELOG_GUILD_LOG_H_

/*
01	길드 결성	                    결성길드(GID)	결성자(CID)		                        사용 금액		

02	길드 가입 신청	                대상길드(GID)	가입신청자(CID)				
03	길드 가입 허가	                대상길드(GID)	허가승인자(CID)	    가입신청자(CID)			
04	길드 가입 불가(가입대기자 탈퇴)	대상길드(GID)	허가승인자(CID)	    가입신청자(CID)			
05	길드 탈퇴(혹은 탈퇴 당함)	    대상길드(GID)	탈퇴승인자(CID)	    탈퇴자(CID)			

06	길드원 등급 변경	            대상길드(GID)	승인자(이름,CID)	대상길드원(CID)	    길드원등급		
07	길드 권한 설정	                대상길드(GID)	변경자(이름,CID)		                길드 권한		

08	길드 등급 조정	                대상길드(GID)	등급조정자(CID)		                    변경전 금액, 변경후 금액, 현재 등급
09	길드마크 등록/변경	            대상길드(GID)	변경자(CID)		                        변경전 금액, 변경후 금액
10	길드 금고 변경	                대상길드(GID)	변경자(CID)		                        변경전 금액, 변경후 금액

11	길드 소멸시 경고	            대상길드(GID)			
12	길드 소멸	                    대상길드(GID)			                                해제시 소지금

20	길드 요청 결과(에러코드0이면 성공)	대상길드(GID)	행위자	대상자	요청 종류
*/


namespace GAMELOG
{
    // 길드 생성
    void LogGuildCreate(unsigned char cType, unsigned long dwGID, 
        unsigned long dwCreateCID, unsigned long dwCreateGold, unsigned short usErrorCode);

    // 길드 가입 신청
    void LogGuildJoin(unsigned char cType, unsigned long dwGID, 
        unsigned long dwJoinCID, unsigned long dwFirstTitle, unsigned short usErrorCode);

    // 길드 탈퇴
    void LogGuildLeave(unsigned char cType, unsigned long dwGID,
        unsigned long dwPermitterCID, unsigned long dwLeaveCID, unsigned short usErrorCode);

    // 길드원 등급 변경
    void LogGuildMemberLevelAdjust(unsigned char cType, unsigned long dwGID,
        unsigned long dwPermitterCID, unsigned long dwMemberCID, 
        unsigned long dwGuildMemberLevel, unsigned short usErrorCode);

    // 길드원 권한 설정
    void LogGuildRightsChange(unsigned char cType, unsigned long dwGID,
        unsigned long dwPermitterCID, const char* cGuildRights, 
        unsigned short usGuildRightsSize, unsigned short usErrorCode);

    // 길드 레벨 변경
    void LogGuildLevel(unsigned char cType, unsigned long dwGID, 
        unsigned long dwPermitterCID, unsigned long dwGuildLevel, 
        unsigned long dwPreChangeGold, unsigned long dwPostChangeGold, unsigned short usErrorCode);

    // 길드마크 등록/변경
    void LogGuildMarkChange(unsigned char cType, unsigned long dwGID,
        unsigned long dwChangerCID, unsigned long dwPreChangeGold, unsigned long dwPostChangeGold,
        const char* szGuildMarkData, unsigned short usGuildMarkDataSize, unsigned short usErrorCode);

    // 길드 금고 변경
    void LogGuildStoreGoldChange(unsigned char cType, unsigned long dwGID,
        unsigned long dwChangerCID, unsigned char cCmd, unsigned long dwPreChangeGold, 
        unsigned long dwPostChangeGold, unsigned short usErrorCode);

    // 길드 소멸됨
    void LogGuildDispose(unsigned char cType, unsigned long dwGID, const char* szDissolveReason, 
        const char* szGuildDestroyFileName, int nGuildDestroyLine, unsigned short usErrorCode);        
};


#endif