#ifndef _HAN_UNITED_BILLING_PACKET_STRUCT_
#define _HAN_UNITED_BILLING_PACKET_STRUCT_

/********** LK->UID,UID->LK �ۼ��ſ� ����ü	*************************/

namespace HanUnitedBilling
{
    #pragma pack(1)

    struct GLTransStruct
    {
	    unsigned short  DataSize;   // Data �� ��ü����
	    unsigned short  HeaderMsg;  // �����ɸ޼����κкκ�
	    unsigned long   SeqID;
	    char            Data[120];  //Data
    };

    #pragma pack()

    //LK->UID , UID->LK �޼���
    enum CMD
    {
        AUTH				= 4000, // UID -> LK
        LOGOUT				= 4001, // UID -> LK
        CUT_IP              = 4002, // LK -> UID
        WARNNOTICE			= 4003, // LK -> UID
        BILL_AUTH			= 4004, // UID -> LK
        AUTH_RESULT			= 4005, // LK -> UID
        BILL_AUTH_RESULT	= 4006, // LK -> UID

        PING_AUTH           = 4007,
        LOGOUT_RESULT       = 4008
    };

    enum LENGTH
    {
        MAX_IP_LEN          = 17,
        MAX_ID_LEN          = 25,
        MAX_OUT_TIME_LEN    = 18,
        MAX_MSG_LEN         = 70
    };
};

#endif