#ifndef _GAMA_CLIENT_SOCKET_CONSTANTS_H_
#define _GAMA_CLIENT_SOCKET_CONSTANTS_H_


const unsigned short	NoError					= 0x0000;				// ���� ����
const unsigned short	WrongParameter			= 0x4001;				// �߸��� ����
const unsigned short	ExceptionError			= 0x4002;				// ���� �� �߻�


namespace ClientConstants
{
    enum Hand		
    {
        Hand_Weapon1	    = 1, 
        Hand_Weapon2	    = 2 
    };	 

    enum FameInfo	
    { 
        FameInfo_Fame		= 1, 
        FameInfo_Mileage	= 2	
    };

    enum Require	
    {
        Require_HPInfo	    = 0,
        Require_UDPInfo	    = 1, 
        Require_Shape	    = 2
    };

    enum BindPos	
    {
        Bind_SavePos	    = 0,
        Bind_Warp			= 1,
        Bind_Recall	= 2	
    };

    enum ChatType		
    {
        ChatType_Normal		= 0,	
        ChatType_Party		= 1,
        ChatType_Friend		= 2,	
        ChatType_Guild	    = 3,    
        ChatType_ClientLog	= 5,	
        ChatType_Stall		= 6,    
        ChatType_Shout      = 7,
		ChatType_FindParty	= 14,
		ChatType_Dice		= 16,
		ChatType_GMReport	= 17,
        ChatType_Notify		= 255	
    };

    enum CellOper		
    {	
        CellOper_Move	    = 0,	
        CellOper_Login      = 1,
        CellOper_Logout	    = 1,
        CellOper_Respawn	= 2
    };

    enum PartyCmd		
    {
        PartyCmd_Invite		    = 0,
        PartyCmd_Accept		    = 1,
        PartyCmd_Refuse		    = 2,
        PartyCmd_Banish 	    = 3,
        PartyCmd_Secession	    = 4,
        PartyCmd_Transfer	    = 5,
        PartyCmd_Destroy	    = 6,
        PartyCmd_Login		    = 7,
        PartyCmd_Logout		    = 8,
        PartyCmd_Delete		    = 9,
        PartyCmd_ReLogin	    = 10,
        PartyCmd_FindParty	    = 11,
        PartyCmd_FindMember	    = 12,	
        PartyCmd_AutoRoutingOn	= 13,
        PartyCmd_AutoRoutingOff	= 14
    };

    enum StateType	
    {	
        StateType_STR	= 1,	
        StateType_DEX	= 2,
        StateType_CON	= 3,	
        StateType_INT	= 4,
        StateType_WIS	= 5
    };

    enum Judge		
    {
        Judge_Front				= 0,	Judge_Side			= 1,	Judge_Back			= 2,	
        Judge_Guard				= 3,	Judge_Critical		= 4,	Judge_Heal			= 5,	
        Judge_ManaHeal			= 6,	Judge_Chant			= 7,	Judge_Enchant		= 8,	
        Judge_Disenchant		= 9,	Judge_Evade			= 10,	Judge_Resist		= 11,	
        Judge_ManaShell			= 12,	Judge_Poisoned		= 13,	Judge_NonShow		= 14, 
		Judge_Miss				= 15,	Judge_Counter		= 16,	Judge_FireCracker	= 17, 
		Judge_ChinaFireCracker	= 18,	Judge_SkillItem		= 19,   Judge_Fired			= 20,
		Judge_Social			= 21,	Judge_Gather		= 22
    };

    enum ExchangeCmd	
    {
        ExchangeCmd_Propose	= 0,	ExchangeCmd_Accept	= 1,	ExchangeCmd_Refuse		= 2,
        ExchangeCmd_OK		= 3,	ExchangeCmd_Cancel	= 4,	ExchangeCmd_Exchange	= 5,
        ExchangeCmd_Quit	= 6,	ExchangeCmd_Lock	= 7,	ExchangeCmd_UnLock		= 8
    };

    enum DuelCmd	
    {
        DuelCmd_Propose		= 0,	DuelCmd_Ready	= 1,			DuelCmd_Refuse			= 2,	
        DuelCmd_Logout		= 3,	DuelCmd_Dead	= 4,			DuelCmd_PartyPropose	= 5,
        DuelCmd_PartyReady	= 6,	DuelCmd_RequestPartyInfo = 7,	DuelCmd_Cancel			= 8
    };

    enum AdminCmd 
    {	
        MoveZoneChar	= 1,
        MoveZoneParty	= 2,
        MoveChar		= 3,	
        MoveParty	    = 4,
        RespawnChar		= 5,	
        RespawnParty	= 6,	
        KillChar		= 7,	
        KillParty	    = 8,
        DuelInit		= 9,	
        CreateItem		= 10,	
        CreateMon		= 11,	
        InfoChar	    = 12,
        GetExp			= 13,	
        GetGold			= 14,	
        LevelUp			= 15,	
        PeaceMode	    = 16,
        WarMode			= 17,	
        GetFame			= 18,	
        GetMileage		= 19,
        TraceChar	    = 20,
        HideChar		= 21,	
        ShowChar		= 22,	
        InvincibleChar	= 23,	
        NormalChar	    = 24,
        LogoutChar		= 25,	
        Apocalypse		= 26,	
        CreateEquip		= 27,	
        EliteBonus	    = 28,

		StatueInit		= 29,	// ���� ���� �ʱ�ȭ
		RespawnAllChar	= 30,	// ��� ĳ���� ������ (��Ʋ �׶���)
		ChangeClass		= 31,	// ���� �ϱ�
		LearnSkill		= 32,	// �ش� ��ų ����
		EraseSkill		= 33,	// �ش� ��ų �����
		LevelDown		= 34,	// ���� �ٿ�
		GetQuest		= 35,	// ����Ʈ �ޱ�
		AddQuest		= 36,	// �Ϸ� ����Ʈ�� �߰�		(�����丮�� �߰�)
		DeleteQuest		= 37,	// ����/�Ϸ� ����Ʈ�� ����  (�����丮���� ����)

								// BATTLEGROUND_SERVER ��� ���
		FinishGame 		= 38,	// �ش� ���� ��� ���� (���� ����)
		DrawGame		= 39,	// �ش� ���� ��� ���� (���º�)
		IgnoreGame		= 40,	// �ش� ���� ��� ���� (���� ����)
		SetMax			= 41,	// �ش� ���� �ִ� �ο��� ����
		SetScore		= 42,	// �ش� ���� ��ǥ ���� ����
		SetLimitTime	= 43,	// �ش� ���� �ð� ���� ����
		SetRestTime		= 44,	// �ش� ���� ���� �ð� ����

					//	= 45,	// �������� �ӽ� �κ������� ���

		AddSpell		= 46,	// �ش� ������ �ο��Ѵ�.
		DeleteSpell		= 47,	// �ش� ������ �����.

		Stop			= 48,	// �ش� ĳ���͸� ����д�.
		StopOff			= 49,	// ���� ĳ���͸� Ǯ���ش�.

		Neo				= 52,	// �׿�
		Smith			= 53,	// ���̽�

		Relation		= 55,

		LearnAbility	= 60,	// �ش� �����Ƽ ����
		EraseAbility	= 61,	// �ش� �����Ƽ �����
					//	= 100	// �������� �����뵵�� ���
    };

    enum AuthorizePanelCmd
    { 
        APCmd_Resurrection	    = 1,
        APCmd_Recall		    = 2
    };

    enum RespawnCmd
    {
		RespawnCmd_Normal			= 0,
		RespawnCmd_Select			= 1,
		RespawnCmd_FormerPlace		= 2,
		RespawnCmd_Human1			= 3,
		RespawnCmd_Human2			= 4,
		RespawnCmd_Human3			= 5,
		RespawnCmd_Akhan1			= 6,
		RespawnCmd_Akhan2			= 7,
		RespawnCmd_Akhan3			= 8,
        //-- 2004. 2. 13. Zergra From.--//
        RespawnCmd_BGServer_Random	= 9,
        //-- Zergra To. --//        
		RespawnCmd_BG_Exit			= 10
    };

    enum AutoRoutingCmd
    {
        ARCmd_Order			    = 0,
		ARCmd_Possible	    	= 1,	
		ARCmd_Impossible    	= 2	
	};

	enum SummonCmd
	{
		SummonCmd_Attack	    = 0,
		SummonCmd_Return	    = 1,
		SummonCmd_Die		    = 2,	
		SummonCmd_GuardOn   	= 3,
		SummonCmd_GuardOff	    = 4
	};
};


#endif
