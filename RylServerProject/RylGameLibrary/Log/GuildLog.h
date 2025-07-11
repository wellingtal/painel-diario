#ifndef _GAMELOG_GUILD_LOG_H_
#define _GAMELOG_GUILD_LOG_H_

/*
01	��� �Ἲ	                    �Ἲ���(GID)	�Ἲ��(CID)		                        ��� �ݾ�		

02	��� ���� ��û	                �����(GID)	���Խ�û��(CID)				
03	��� ���� �㰡	                �����(GID)	�㰡������(CID)	    ���Խ�û��(CID)			
04	��� ���� �Ұ�(���Դ���� Ż��)	�����(GID)	�㰡������(CID)	    ���Խ�û��(CID)			
05	��� Ż��(Ȥ�� Ż�� ����)	    �����(GID)	Ż�������(CID)	    Ż����(CID)			

06	���� ��� ����	            �����(GID)	������(�̸�,CID)	������(CID)	    �������		
07	��� ���� ����	                �����(GID)	������(�̸�,CID)		                ��� ����		

08	��� ��� ����	                �����(GID)	���������(CID)		                    ������ �ݾ�, ������ �ݾ�, ���� ���
09	��帶ũ ���/����	            �����(GID)	������(CID)		                        ������ �ݾ�, ������ �ݾ�
10	��� �ݰ� ����	                �����(GID)	������(CID)		                        ������ �ݾ�, ������ �ݾ�

11	��� �Ҹ�� ���	            �����(GID)			
12	��� �Ҹ�	                    �����(GID)			                                ������ ������

20	��� ��û ���(�����ڵ�0�̸� ����)	�����(GID)	������	�����	��û ����
*/


namespace GAMELOG
{
    // ��� ����
    void LogGuildCreate(unsigned char cType, unsigned long dwGID, 
        unsigned long dwCreateCID, unsigned long dwCreateGold, unsigned short usErrorCode);

    // ��� ���� ��û
    void LogGuildJoin(unsigned char cType, unsigned long dwGID, 
        unsigned long dwJoinCID, unsigned long dwFirstTitle, unsigned short usErrorCode);

    // ��� Ż��
    void LogGuildLeave(unsigned char cType, unsigned long dwGID,
        unsigned long dwPermitterCID, unsigned long dwLeaveCID, unsigned short usErrorCode);

    // ���� ��� ����
    void LogGuildMemberLevelAdjust(unsigned char cType, unsigned long dwGID,
        unsigned long dwPermitterCID, unsigned long dwMemberCID, 
        unsigned long dwGuildMemberLevel, unsigned short usErrorCode);

    // ���� ���� ����
    void LogGuildRightsChange(unsigned char cType, unsigned long dwGID,
        unsigned long dwPermitterCID, const char* cGuildRights, 
        unsigned short usGuildRightsSize, unsigned short usErrorCode);

    // ��� ���� ����
    void LogGuildLevel(unsigned char cType, unsigned long dwGID, 
        unsigned long dwPermitterCID, unsigned long dwGuildLevel, 
        unsigned long dwPreChangeGold, unsigned long dwPostChangeGold, unsigned short usErrorCode);

    // ��帶ũ ���/����
    void LogGuildMarkChange(unsigned char cType, unsigned long dwGID,
        unsigned long dwChangerCID, unsigned long dwPreChangeGold, unsigned long dwPostChangeGold,
        const char* szGuildMarkData, unsigned short usGuildMarkDataSize, unsigned short usErrorCode);

    // ��� �ݰ� ����
    void LogGuildStoreGoldChange(unsigned char cType, unsigned long dwGID,
        unsigned long dwChangerCID, unsigned char cCmd, unsigned long dwPreChangeGold, 
        unsigned long dwPostChangeGold, unsigned short usErrorCode);

    // ��� �Ҹ��
    void LogGuildDispose(unsigned char cType, unsigned long dwGID, const char* szDissolveReason, 
        const char* szGuildDestroyFileName, int nGuildDestroyLine, unsigned short usErrorCode);        
};


#endif