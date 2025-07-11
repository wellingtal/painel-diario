////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ��Ŷ Ŀ�ǵ�
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _PACKET_COMMAND_H_
#define _PACKET_COMMAND_H_

#include <Network/Packet/PacketBase.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Ŀ�ǵ�
//
////////////////////////////////////////////////////////////////////////////////////////////////////
const PktBase::CMDType	CmdNull						= 0x00;	

const PktBase::CMDType	CmdAuthAccount				= 0x01;			// ���� ����

const PktBase::CMDType	CmdServerGroup				= 0x02;			// ������ ����

const PktBase::CMDType	CmdUserLogin				= 0x03;			// ���� �α���
const PktBase::CMDType	CmdUserKill					= 0x04;			// ���� ����

const PktBase::CMDType	CmdCharSelect				= 0x05;			// ĳ���� ����
const PktBase::CMDType	CmdCharCreate				= 0x06;			// ĳ���� ����
const PktBase::CMDType	CmdCharDelete				= 0x07;			// ĳ���� ����

const PktBase::CMDType	CmdCharLogin				= 0x08;			// ĳ���� �α���
const PktBase::CMDType	CmdCharSuicide				= 0x09;			// ĳ���� �ڻ�
const PktBase::CMDType	CmdCharRespawn				= 0x0A;			// ĳ���� ��Ȱ
const PktBase::CMDType	CmdCharMove					= 0x0B;			// ĳ���� �̵� (P2P)
const PktBase::CMDType	CmdCharMoveUpdate			= 0x0C;			// ĳ���� �̵� ������Ʈ
const PktBase::CMDType	CmdCharChat					= 0x0D;			// ĳ���� ä��
const PktBase::CMDType	CmdCharAttack				= 0x0E;			// ĳ���� ����
const PktBase::CMDType	CmdCharCastObject			= 0x0F;			// ĳ���� ������Ʈ ������
const PktBase::CMDType	CmdCharUseSkill				= 0x10;			// ĳ���� ��ų ���
const PktBase::CMDType	CmdCharTakeItem				= 0x11;			// ĳ���� ������ ����
const PktBase::CMDType	CmdCharSwapItem				= 0x12;			// ĳ���� ������ ����
const PktBase::CMDType	CmdCharRepairItem			= 0x13;			// ĳ���� ������ ����
const PktBase::CMDType	CmdCharUseItem				= 0x14;			// ĳ���� ������ ���
const PktBase::CMDType	CmdCharTradeItem			= 0x15;			// ĳ���� ������ �ŷ�
const PktBase::CMDType	CmdCharSkillLock			= 0x16;			// ĳ���� ��ų ��
const PktBase::CMDType	CmdCharSkillCreate			= 0x17;			// ĳ���� ��ų ����
const PktBase::CMDType	CmdCharSkillErase			= 0x18;			// ĳ���� ��ų �����
const PktBase::CMDType	CmdCharClassUpgrade			= 0x19;			// ĳ���� Ŭ���� ���׷��̵�
const PktBase::CMDType	CmdCharShapeInfo			= 0x1A;			// ĳ���� ���� (P2P)
const PktBase::CMDType	CmdCharIncreasePoint		= 0x1B;			// ĳ���� �ɷ� ����Ʈ ����
const PktBase::CMDType	CmdCharBindPosition			= 0x1C;			// ĳ���� ���ε� ������
const PktBase::CMDType	CmdCharRequireInfo			= 0x1D;			// ĳ���� �ش� ���� ��û
const PktBase::CMDType	CmdCharUpdateAddress		= 0x1E;			// ĳ���� UDP �ּ� ������Ʈ
const PktBase::CMDType	CmdCharPartyCmd				= 0x1F;			// ĳ���� ��Ƽ ���
const PktBase::CMDType	CmdCharPartyMemInfo			= 0x20;			// ĳ���� ��Ƽ �ɹ� ���� ����
const PktBase::CMDType	CmdCharExchangeCmd			= 0x21;			// ĳ���� ������ ��ȯ ���
const PktBase::CMDType	CmdSysConnectAgent			= 0x22;			// �ý��� �߰� ���� (UDP �߰�)		# ��� ����
const PktBase::CMDType	CmdSysPacketTransmit		= 0x23;			// �ý��� ��Ŷ ���� (UDP �߰�)		# ��� ����
const PktBase::CMDType	CmdCharLogout				= 0x24;			// ĳ���� �α� �ƿ�
const PktBase::CMDType	CmdDBGetData				= 0x25;			// DB ������ ���
const PktBase::CMDType	CmdDBUpdateData				= 0x26;			// ������Ʈ DB ������
const PktBase::CMDType	CmdAgentParty				= 0x27;			// DB ������Ʈ ��Ƽ
const PktBase::CMDType	CmdSysServerLogin			= 0x28;			// �ý��� ���� �α���

const PktBase::CMDType	CmdServerZone				= 0x29;			// ������ ����

const PktBase::CMDType	CmdGameCellInfo				= 0x2A;			// ���� �� ���� (������Ʈ ����)	
const PktBase::CMDType	CmdCharInfo					= 0x2B;			// ĳ���� ���� (������ ���տ� �����)
const PktBase::CMDType	CmdCharAddressInfo			= 0x2C;			// ĳ���� UDP �ּ� ����
const PktBase::CMDType	CmdCharCellLogin			= 0x2D;			// ĳ���� �� �α���
const PktBase::CMDType	CmdCharCellLogout			= 0x2E;			// ĳ���� �� �α׾ƿ�
const PktBase::CMDType	CmdMonMove					= 0x2F;			// ���� �̵�
const PktBase::CMDType	CmdCharAttackInfo			= 0x30;			// ĳ���� ���� ���� (P2P)
const PktBase::CMDType	CmdCharAttacked				= 0x31;			// ĳ���� ���� ����
const PktBase::CMDType	CmdCharAward				= 0x32;			// ĳ���� ����� 
const PktBase::CMDType	CmdCharItemInfo				= 0x33;			// ĳ���� ������ ����
const PktBase::CMDType	CmdCharPickUp				= 0x34;			// ĳ���� ����
const PktBase::CMDType	CmdCharPullDown				= 0x35;			// ĳ���� ������
const PktBase::CMDType	CmdCharPickUpInfo			= 0x36;			// ĳ���� ���� ����
const PktBase::CMDType	CmdCharPullDownInfo			= 0x37;			// ĳ���� ������ ����
const PktBase::CMDType	CmdCharCastObjectInfo		= 0x38;			// ĳ���� ������ ������Ʈ ����
const PktBase::CMDType	CmdCharInstallSocket		= 0x39;			// ĳ���� ������ ���� ��ġ
const PktBase::CMDType	CmdCharLevelUp				= 0x3A;			// ĳ���� ���� ��
const PktBase::CMDType	CmdCharPartyInfo			= 0x3B;			// ĳ���� ��Ƽ ����
const PktBase::CMDType	CmdCharUpgradeItem			= 0x3C;			// ĳ���� ������ ���׷��̵�
const PktBase::CMDType	CmdCharHPRegen				= 0x3D;			// ĳ���� HP ����
const PktBase::CMDType	CmdCharLevelUpInfo			= 0x3E;			// ĳ���� ������ ����
const PktBase::CMDType	CmdCharSplitItem			= 0x3F;			// ĳ���� ���ø� ������ (���� �ִ� �������� �ΰ��� �ɰ� �� ����)
const PktBase::CMDType	CmdUpdateUIDTable			= 0x40;			// UID ���̺� ������Ʈ
const PktBase::CMDType	CmdCharQuickSlotMove		= 0x41;			// ĳ���� ������ �̵�
const PktBase::CMDType	CmdCharSwitchEQ				= 0x42;			// ��� �ٲٱ�
const PktBase::CMDType	CmdSysMngerRegistry			= 0x43;			// ���� ������ ���
const PktBase::CMDType	CmdSysMngerRequest			= 0x44;			// ���� �������� ��û
const PktBase::CMDType	CmdSysMngerResponse			= 0x45;			// ���� �����ڷ� ����
const PktBase::CMDType	CmdCharTakeItems			= 0x46;			// ĳ���� ������ ���� (����)
const PktBase::CMDType	CmdCharTakeGold				= 0x47;			// ĳ���� �� ����
const PktBase::CMDType	CmdCharExchangeItem			= 0x48;			// ĳ���� ������ ��ȯ
const PktBase::CMDType	CmdCellBroadCasting			= 0x49;			// �� ��ε� ĳ����
const PktBase::CMDType	CmdSysPatchAddress			= 0x4A;			// ��ġ �ּ� 
const PktBase::CMDType	CmdCharPartyCmdInfo			= 0x4B;			// ��Ƽ ��� ����
const PktBase::CMDType	CmdServerLog				= 0x4C;			// �α� ���� (???)
const PktBase::CMDType	CmdCharWhisper				= 0x4D;			// ĳ���� �ӼӸ�
const PktBase::CMDType	CmdSysServerVerUpdate		= 0x4E;			// ���� ���� ������Ʈ
const PktBase::CMDType	CmdSysMng					= 0x4F;			// ���� ���� ����, Ŭ���̾�Ʈ�� ����ϴ� ��Ŷ (�ӽ�) (???)
const PktBase::CMDType	CmdSysChannelUpdate			= 0x50;			// ���� ä�� ������Ʈ
const PktBase::CMDType	CmdCharPartyFind			= 0x51;			// ��Ƽ ã��
const PktBase::CMDType	CmdCharPartyMemData			= 0x52;			// ��Ƽ ��� ������
const PktBase::CMDType	CmdCharControlOption		= 0x53;			// ĳ���� �ɼ� ����
const PktBase::CMDType	CmdCharDuelCmd				= 0x54;			// ĳ���� ��� ���
const PktBase::CMDType	CmdCharFameInfo				= 0x55;			// ĳ���� �� ����

const PktBase::CMDType	CmdLoginServerList			= 0x56;			// ���� ����Ʈ #!! ��ȣ ���� �Ұ� !!#

const PktBase::CMDType	CmdCharSpellInfo			= 0x57;			// ĳ���� æƮ&��æƮ ����
const PktBase::CMDType	CmdCharSkillUnLock			= 0x58;			// ĳ���� ��ų �� ����
const PktBase::CMDType	CmdSysPing					= 0x59;			// ���� �� ��Ŷ
const PktBase::CMDType	CmdCharMoveZone				= 0x5A;			// �� �̵�

const PktBase::CMDType	CmdAgentZone				= 0x5B;			// �� ����

const PktBase::CMDType  CmdDeposit              	= 0x5C;         // â�� ó�� ��Ŷ (Client <--> GameServer)
const PktBase::CMDType  CmdDepositUpdate        	= 0x5D;         // â�� ������Ʈ ��Ŷ (GameServer <--> DBAgent)

const PktBase::CMDType	CmdCharStallOpen			= 0x5E;			// ĳ���� ������ ����
const PktBase::CMDType	CmdCharStallRegisterItem	= 0x5F;			// ĳ���� ������ ������ ���
const PktBase::CMDType	CmdCharStallEnter			= 0x60;			// ĳ���� ������ ����
const PktBase::CMDType	CmdCharStallItemInfo		= 0x61;			// ĳ���� ������ ������ ����

const PktBase::CMDType	CmdCharAdminCmd				= 0x62;			// ĳ���� ����

const PktBase::CMDType	CmdCharTeamBattleInfo		= 0x63;			// ����Ʋ ����

const PktBase::CMDType  CmdFriendAddRequest         = 0x64;         // ģ�� �߰�
const PktBase::CMDType  CmdFriendRemoveRequest      = 0x65;         // ģ�� ����
const PktBase::CMDType  CmdFriendEtcRequest         = 0x66;         // ģ�� ��Ÿ
const PktBase::CMDType  CmdFriendAck                = 0x67;         // ģ�� Ack
const PktBase::CMDType  CmdFriendDB                 = 0x68;         // ģ�� ������ (�� ģ�� ����Ʈ ����)

const PktBase::CMDType  CmdEliteBonus				= 0x69;         // ����Ʈ ���ʽ�

const PktBase::CMDType	CmdCharStartQuest			= 0x6A;			// ����Ʈ ����
const PktBase::CMDType	CmdCharOperateTrigger		= 0x6B;			// Ʈ���� �ߵ�
const PktBase::CMDType	CmdQuestDB					= 0x6C;			// ����Ʈ ���� (DB�� ���̺�/�ε�, ĳ���� �α��)
const PktBase::CMDType	CmdCharEndQuest				= 0x6D;			// ����Ʈ ����

const PktBase::CMDType	CmdCharDisappearItem		= 0x6E;			// ĳ���� ������ �������

const PktBase::CMDType	CmdCharAuthorizePanel		= 0x6F;			// ĳ���� ���� �ǳ�
const PktBase::CMDType	CmdCharPeaceMode			= 0x70;			// ĳ���� ���� ���
const PktBase::CMDType	CmdConfigInfoDB				= 0x71;			// ���� ����
const PktBase::CMDType	CmdCharAutoRouting			= 0x72;			// ĳ���� ���� ����
const PktBase::CMDType	CmdRankingInfo				= 0x73;			// ��ŷ ����
const PktBase::CMDType	CmdCharStateRedistribution	= 0x74;			// ���� ��й� (Ŭ���� ó�� ���·�)
const PktBase::CMDType  CmdBillingTimeoutNotify     = 0x75;         // ���� Ÿ�Ӿƿ� Notify����

const PktBase::CMDType  CmdAdminToolGetData         = 0x76;         // ������� DBAgent�� ������ ��� ����.
const PktBase::CMDType  CmdAdminToolSetData         = 0x77;         // ������� DBAgent�� ������ ����.

const PktBase::CMDType	CmdEventDropItem			= 0x78;			// ������ ������ �̺�Ʈ
const PktBase::CMDType	CmdCharCancelQuest			= 0x79;			// ����Ʈ ���
const PktBase::CMDType	CmdBillingTimeCheckNotify	= 0x7A;			// ���� Ÿ�� üũ
const PktBase::CMDType	CmdCharLotteryResult		= 0x7B;			// ���� ���
const PktBase::CMDType	CmdCharSummonCmd			= 0x7C;			// ��ȯ�� ���

const PktBase::CMDType  CmdChatClientLogin          = 0x7D;         // Ŭ���̾�Ʈ���� ä�ü����� ���� �� ���� ��
const PktBase::CMDType  CmdChatLogin                = 0x7E;         // �߰輭���� ä�ü����� Ŭ���̾�Ʈ �α��� ��.
const PktBase::CMDType  CmdChatLogout               = 0x7F;         // �߰輭���� ä�ü����� Ŭ���̾�Ʈ �α׾ƿ� ��.
const PktBase::CMDType  CmdChatInfoChanged          = 0x80;         // �߰輭���� ä�ü����� Ŭ���̾�Ʈ ���� ���� ��.

const PktBase::CMDType	CmdCharSummon				= 0x81;			// ��ȯ

const PktBase::CMDType	CmdJapanAuthAccount	        = 0x82;			// �Ϻ��� ���� ���� (ID, Password���, ������ �ڵ尡 �´�.)

const PktBase::CMDType	CmdCharBattleGroundRespawn	= 0x83;			// ������ (��Ʋ�׶��� �����)
const PktBase::CMDType	CmdCharRespawnWaitQueue		= 0x84;			// ������ ť ����� ���� (��Ʋ�׶����)
const PktBase::CMDType	CmdStatueInfo				= 0x85;			// ���� ����
const PktBase::CMDType	CmdCameraScript				= 0x86;			// ī�޶� ��ũ��Ʈ
const PktBase::CMDType	CmdCharEquipDurability		= 0x87;			// ĳ���� ��� ������

const PktBase::CMDType	CmdCreateGuild				= 0x88;			// ��� ����
const PktBase::CMDType	CmdGuildCmd					= 0x89;			// ��� ��� ���� ���
const PktBase::CMDType	CmdGuildMark				= 0x8A;			// ��� ��ũ ����
const PktBase::CMDType	CmdGuildLevel				= 0x8B;			// ��� ���� ����
const PktBase::CMDType	CmdGuildRelation			= 0x8C;			// ��� ���� ����
const PktBase::CMDType	CmdGuildList				= 0x8D;			// ��� ����Ʈ
const PktBase::CMDType	CmdGuildDB					= 0x8E;			// ��� ������
const PktBase::CMDType	CmdGuildRight				= 0x8F;			// ��� ���� ����
const PktBase::CMDType	CmdGuildMemberList			= 0x90;			// ��� ��� ����Ʈ
const PktBase::CMDType	CmdMyGuildInfo				= 0x91;			// �ڱ� ��� ����
const PktBase::CMDType	CmdGuildSafe				= 0x92;			// ��� �ݰ�
const PktBase::CMDType	CmdGuildMemberInfoUpdate	= 0x93;			// ��� ��� ���� ������Ʈ

const PktBase::CMDType	CmdCharStatusRetrain		= 0x94;			// �������ͽ� ���Ʒ� (������ �ٽ� ��������)
const PktBase::CMDType	CmdSysServerLogout			= 0x95;			// ���� �α׾ƿ�

const PktBase::CMDType	CmdCharUseCashItem			= 0x96;			// ĳ���� ĳ�� ������ ���
const PktBase::CMDType	CmdExtraEvent				= 0x97;			// ����Ʈ�� �̺�Ʈ ����

//const PktBase::CMDType	CmdCharPartyAddress			= 0x96;			// ��Ƽ�� UDP �ּ� ����
//const PktBase::CMDType	CmdCharPartyMemAddress		= 0x97;			// ��Ƽ�� UDP �ּ� ���� (P2P)

const PktBase::CMDType	CmdBGServerMapList			= 0x98;			// ��Ʋ �׶��� ���� ��(��) ���� ����Ʈ ��û
const PktBase::CMDType	CmdBGServerResultList		= 0x99;			// ��Ʋ �׶��� ���� ��(��) ��� ����Ʈ ��û
const PktBase::CMDType	CmdBGServerMoveZone			= 0x9A;			// ��Ʋ �׶��� ���� ��(��) �̵� (�� �̵�)
const PktBase::CMDType	CmdBGServerMileageChange	= 0x9B;			// ��Ʋ �׶��� ���� ȯ���� ���
const PktBase::CMDType	CmdBGServerCharSlot			= 0x9C;			// ��Ʋ �׶��� ���� ���� ĳ���� ���� ��û

const PktBase::CMDType  CmdHanBTNWarning            = 0x9D;         // �Ѱ��� ���պ��� ���� ������ ��� �޽���
const PktBase::CMDType  CmdHanBTNUserKill           = 0x9E;         // �Ѱ��� ���պ��� ���� ���� �޽���

const PktBase::CMDType  CmdCharRepairAllItem        = 0x9F;         // ��� ������ ��� ����
const PktBase::CMDType	CmdCSAuth					= 0xA0;			// ���� �ڵ� (���� ����)
const PktBase::CMDType	CmdCharItemChemical			= 0xA1;			// ������ �ռ�
const PktBase::CMDType  CmdItemQtyCheck             = 0xA2;         // ������ ���� ����
const PktBase::CMDType	CmdGuildInclination			= 0xA3;			// ��� ����
const PktBase::CMDType	CmdGuildMemberFameUpdate	= 0xA4;			// ��� ��� �� ������Ʈ

const PktBase::CMDType	CmdCastleInfo				= 0xA5;			// �� ���� ������			(DBAgentServer -> GameServer)
const PktBase::CMDType	CmdCampInfo					= 0xA6;			// ��� ��� ���� ������	(DBAgentServer -> GameServer)
const PktBase::CMDType	CmdCreateCastle				= 0xA7;			// �� ����					(GameServer -> Client)
const PktBase::CMDType	CmdCreateCamp				= 0xA8;			// ��� ��� ����			(Client <- GameServer -> DBAgentServer)
const PktBase::CMDType	CmdCreateSiegeArms			= 0xA9;			// ���� ���� ����			(Client <- GameServer -> DBAgentServer)
const PktBase::CMDType	CmdCastleCmd				= 0xAA;			// �� ���� ��� ��Ŷ		(Client <-> GameServer <-> DBAgentServer)
const PktBase::CMDType	CmdCampCmd					= 0xAB;			// ��� ��� ���� ��� ��Ŷ	(Client <-> GameServer <-> DBAgentServer)
const PktBase::CMDType	CmdSiegeArmsCmd				= 0xAC;			// ���� ���� ���� ��� ��Ŷ	(Client <-> GameServer <-> DBAgentServer)
const PktBase::CMDType	CmdCastleRight				= 0xAD;			// �� ���� ���� ��Ŷ		(Client <-> GameServer <-> DBAgentServer)
const PktBase::CMDType	CmdCampRight				= 0xAE;			// ��� ��� ���� ���� ��Ŷ
const PktBase::CMDType	CmdSiegeBroadCast			= 0xAF;			// ���� ������Ʈ ��ε� ĳ��Ʈ
const PktBase::CMDType	CmdGameTimeInfo				= 0xB0;			// ���� �ð� ����
const PktBase::CMDType	CmdStealthInfo				= 0xB1;			// ���ڽ� ��밡�� ����
//const PktBase::CMDType	CmdCastleUpdate				= 0xB2;			// �� ���� ������Ʈ			(DBAgentServer -> GameServer -> Client)

const PktBase::CMDType  CmdCellBroadCast2nd         = 0xB3;         // �� ��ε�ĳ��Ʈ ������ ��Ŷ

const PktBase::CMDType	CmdCharRespawnInfo			= 0xB4;			// ������ ���� (������ ���� ����Ʈ ����)
const PktBase::CMDType	CmdCharRespawnAreaInfo		= 0xB5;			// ������ ������ ������ ���� ����

const PktBase::CMDType	CmdCharEquipShopInfo		= 0xB6;			// NPC ��� ���� ����
const PktBase::CMDType	CmdSiegeBroadCast2nd		= 0xB7;			// ���� ������Ʈ ��ε� ĳ��Ʈ �ٸ� ����
const PktBase::CMDType	CmdCharItemOptionGraft		= 0xB8;			// ������ �ɼ� �̽�
const PktBase::CMDType	CmdCharItemCompensation		= 0xB9;			// ������ ���� �Ǹ�

const PktBase::CMDType	CmdGuildMemberGoldUpdate	= 0xBA;			// ��� ��� �� ������Ʈ

const PktBase::CMDType	CmdCampMessage				= 0xBB;			// ��� ��� ���� �޽��� ��Ŷ
const PktBase::CMDType  CmdCharDeadInfo             = 0xBC;         // ĳ���� ��� ����

const PktBase::CMDType	CmdNewSiegeBroadCast		= 0xBD;			// ���� ������Ʈ ��ε� ĳ��Ʈ �ֽ� ���� (���Ǿ� Ʈ�� ���)


const PktBase::CMDType	CmdSelectAccountNation		= 0xBE;			// ���� ���� ���� ��Ŷ

const PktBase::CMDType	CmdNationChangeResult		= 0xBF;			// ���� ���� �������� ���� ����� ĳ���� ���� ��Ŷ

const PktBase::CMDType  CmdUnifiedCharInfo          = 0xC0;         // �������ս� ĳ���� ���� ������
const PktBase::CMDType  CmdUnifiedCharSelect        = 0xC1;         // �������ս� ĳ���� / â�� ����

const PktBase::CMDType  CmdChatBan					= 0xC2;			// ä�� ����
const PktBase::CMDType  CmdGiveItemToTempInven      = 0xC3;         // �ӽ� �κ����� ������ ����
const PktBase::CMDType  CmdCharNameChange           = 0xC4;         // ĳ���� �̸� �ٲٱ�

const PktBase::CMDType	CmdFertilityInfo			= 0xC5;			// ���� ���� ����				(DBAgentServer -> GameServer)
const PktBase::CMDType	CmdMiningCampMineralInfo	= 0xC6;			// ä���� ���� ���� ���� ����	(GameServer -> Client)
const PktBase::CMDType	CmdProcessMining			= 0xC7;			// ä���� ���μ��� ����
const PktBase::CMDType	CmdWorldWeaponInfo			= 0xC8;			// ���� ���� ���� ����			(DBAgentServer -> GameServer)
const PktBase::CMDType	CmdCampShopInfo				= 0xC9;			// ��� ��� ���� ���� ����		(GameServer -> DBAgentServer)

const PktBase::CMDType	CmdCharInstallRuneSocket	= 0xCA;			// �� ������ ��ġ/��������.

const PktBase::CMDType	CmdTakeMaterial				= 0xCB;			// ��� ��� ���� ���� �ֱ�/����

const PktBase::CMDType	CmdCastleTaxMove			= 0xCC;			// ���� �ӽ� ������ ���� �������� �Ű����� ���� ����
const PktBase::CMDType	CmdCastleSiegeCount			= 0xCD;			// ���� �ð��� ���� ���� Ƚ�� ���� ����

const PktBase::CMDType  CmdCompressedPacket         = 0xCE;         // ��Ŷ �������� �����ؼ� ���� �� ���

const PktBase::CMDType	CmdTakeCastleJewel			= 0xCF;			// �� ��¡�� ���� ������ �ֱ�/����
const PktBase::CMDType	CmdCastleMineralInfo		= 0xD0;			// ���� �ӽ� ���� ���� ������Ʈ, ���� ���� ���� ���� ���� ����
																	// (GS -> DBAS),				 (GS -> Client)

const PktBase::CMDType	CmdWarOnOff					= 0xD1;			// ���, ���� ���� ���� ��Ŷ (Client <-> GS).

const PktBase::CMDType	CmdRealmPoint				= 0xD2;			// ���� ���� ���� ���� ����Ʈ ���� (DBAgentServer -> GameServer -> Client).

const PktBase::CMDType	CmdStatueCmd				= 0xD3;			// ���� ���� ���� Ŀ�ǵ� ��Ŷ

const PktBase::CMDType	CmdSaveEnemy				= 0xD4;			// ���� ���� ���� ��Ŷ (DBAgentServer <-> GameServer).

const PktBase::CMDType	CmdGuildHostilityList		= 0xD5;			// ���� ���� ��Ȳ ����Ʈ ��û ��Ŷ
const PktBase::CMDType	CmdGuildRelationInfo		= 0xD6;			// ��� ���Խ� �ڱ� ����� ���� ����Ʈ ��û ��Ŷ

const PktBase::CMDType	CmdGuildPosition			= 0xD7;			// ������� ��� ������ ����.
const PktBase::CMDType	CmdAdminCommandLog			= 0xD8;			// ��� ��ɾ� �α� ��Ŷ.

const PktBase::CMDType	CmdChatBanInfo				= 0xD9;			// ä�ñ��� ���� (GameServer <-> ChatServer).

const PktBase::CMDType	CmdKeyInfo					= 0xDA;			// ���̽�ƽ Ű ����.
const PktBase::CMDType	CmdNameCheck				= 0xDB;	
const PktBase::CMDType	CmdFinalPacketNum			= 0xDC;//was DB		hz add information packet	// ������ ��Ŷ ��ȣ	
														
#endif
