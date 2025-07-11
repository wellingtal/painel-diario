// ----------------------------------------------------------------
// !! ���� : ���������� �� ���ڸ� ����ϹǷ� ������ ���� �ٶ� !!
// ���� �˾� �޴�

// �ΰ� NPC
#define POPUP_NORMAL					1
/*#define POPUP_GUARD						2
#define POPUP_NOBIND					3

#define POPUP_WEAPONMERCHANT			11
#define POPUP_ARMOURMERCHANT			12
#define POPUP_ETCMERCHANT				13

#define POPUP_WEAPONNARMOURMERCHANT		14

#define POPUP_DARKWEAPONMERCHANT		16
#define POPUP_DARKARMOURMERCHANT		17
#define POPUP_DARKETCMERCHANT			18

#define POPUP_BLACKSMITH				21

#define POPUP_COUNSELLER				26
#define POPUP_MEDALSHOP					27

#define POPUP_ADVENTURETRAINER			31
#define POPUP_FIGHTERTRAINER			32
#define POPUP_ROGUETRAINER				33
#define POPUP_MAGETRAINER				34
#define POPUP_ACOLYTETRAINER			35

#define POPUP_GUILDMAKER				36

#define POPUP_TELEPORTER				41
#define POPUP_PEACEGUILD				42
#define POPUP_CONTAINER					43

#define POPUP_STATUSCLEANER				44
#define POPUP_CLASSCLEANER				45

#define POPUP_COMPENSATION				46
#define POPUP_SKILLMERCHANT				47

// ��ĭ NPC
#define POPUP_A_NORMAL					51
#define POPUP_A_GUARD					52
#define POPUP_A_NOBIND					53

#define POPUP_A_WEAPONMERCHANT			61
#define POPUP_A_ARMOURMERCHANT			62
#define POPUP_A_ETCMERCHANT				63

#define POPUP_A_DARKWEAPONMERCHANT		66
#define POPUP_A_DARKARMOURMERCHANT		67
#define POPUP_A_DARKETCMERCHANT			68

#define POPUP_A_BLACKSMITH				71

#define POPUP_A_COUNSELLER				76
#define POPUP_A_MEDALSHOP				77

#define POPUP_A_BEGINNERTRAINER			81
#define POPUP_A_COMBATANTTRAINER		82
#define POPUP_A_OPPICATERTRAINER		83

#define POPUP_A_GUILDMAKER				86

#define POPUP_A_TELEPORTER				91
#define POPUP_A_PEACEGUILD				92
#define POPUP_A_CONTAINER				93

#define POPUP_A_STATUSCLEANER			94
#define POPUP_A_CLASSCLEANER			95

// PC
#define POPUP_PC						100
#define POPUP_LEADER					101
#define POPUP_MEMBER					102

// Battle Ground Client ����
#define POPUP_BATTLEGROUND_MANAGER      103
#define POPUP_MONOEY_CHANGER            104
#define POPUP_BATTLEGROUND_EXITER       105

#define POPUP_A_BATTLEGROUND_MANAGER    106
#define POPUP_A_MONOEY_CHANGER          107
#define POPUP_A_BATTLEGROUND_EXITER     108
*/
// ���� ���� POPUP �޴�
#define POPUP_EMBLEM					109			
#define POPUP_GATE						110
#define POPUP_BACKDOOR					111
#define POPUP_CAMP						112
#define POPUP_UPGRADE_CAMP				113					//���׷��̵� �� ���
#define POPUP_CAMP_SHOP					114					//���� ���
#define POPUP_MINING_CAMP				115					//ä����

#define POPUP_CASTLE_ARMS_NPC			116	
#define POPUP_CASTLE_GUARD				117
#define POPUP_SHORT_RANGE_CASTLE_ARMS	118
#define POPUP_LONG_RANGE_CASTLE_ARMS	119
#define POPUP_SHORT_RANGE_SIEGE_ARMS	120
#define POPUP_LONG_RANGE_SIEGE_ARMS		121
#define POPUP_AIRSHIP					122

// TODO : ���� NPC�� 3�� �̻��� ����â�� ���� �� �ֵ��� ������ �� �������� ��Ĩ�ϴ�.
// TODO : �� ���� �ܰ�δ� �ƿ� �˾��� ���� ��ũ��Ʈ�� ������ �� �ְԲ� �Ͽ� NPC ������ �����սô�.

// ��ų ���� Ʈ���̳�
#define POPUP_ADVANCEDADVENTURE			123
#define POPUP_ADVANCEDFIGHTER			124
#define POPUP_ADVANCEDROGUE				125
#define POPUP_ADVANCEDMAGE				126
#define POPUP_ADVANCEDACOLYTEE			127
#define POPUP_ADVANCEDBEGINNER			128
#define POPUP_ADVANCEDCOMBATANT			129
#define POPUP_ADVANCEDOPPICATER			130

/* 
#define POPUP_SELLINGLIST               131         // �� �̸��� '�ǸŸ��' �ϳ� �ִ� ��
#define POPUP_CHANGENAME				132			// �̸� �ٲٴ� NPC
*/

// �ӽ÷� �ڷ� �������� (���� ��ȣ�� NPC ��ũ��Ʈ�� ������ ��ġ�� ������...)
#define POPUP_WORLDWEAPON				133					//�������

// ----------------------------------------------------------------
// �˾� �޴��� ����

//-- PC�� 
#define POPUPMENU_EXCHANGE			                0
#define POPUPMENU_DUEL				                1
#define POPUPMENU_PARTYINVATE		                2
#define POPUPMENU_STREETSTALL		                3
#define POPUPMENU_LEADER			                4
#define POPUPMENU_VANISH			                5
#define POPUPMENU_PARTYDUEL			                6
#define POPUPMENU_FRIEND			                7
#define POPUPMENU_BAN				                8
#define POPUPMENU_FOLLOW			                9

#define	POPUPMENU_TACTICS							10
#define	POPUPMENU_TACTICS_JOIN						11

//-- NPC��
#define POPUPMENU_DIALOG			                10000   // ���̾�α� ���� �޴�

#define POPUPMENU_SAVEPOS			                101     // ��ġ ����
#define POPUPMENU_UPGRADE			                102     // ���
#define POPUPMENU_UNLOCK			                103     // ��ų ���?(�Ⱦ�?)
#define POPUPMENU_JOBCHANGE			                104     // ����
#define POPUPMENU_ZONEMOVE			                105     // �ڷ���Ʈ
#define POPUPMENU_DIALOG2			                106     // �̰� �׽�Ʈ �� ���� ��
#define POPUPMENU_DEPOSIT			                107     // â��
#define POPUPMENU_QUEST				                108     // ����Ʈ
#define POPUPMENU_QUESTTRIGGER						109		// ����Ʈ Ʈ����
#define POPUPMENU_PEACE				                111     // ��ȭ���
#define POPUPMENU_STATUSCLEAN		                113     // ���� �ʱ�ȭ
#define POPUPMENU_CLASSCLEAN		                114     // Ŭ���� �ʱ�ȭ
#define POPUPMENU_CHANGENAME						115		// �̸� �ٲٱ�

#define POPUPMENU_TRADE						        401     // �ŷ��ϱ�( �ڿ� ���� �ٴ� �͵��� ���Ŀ� ����� ����, �ŷ��� �� ���̵� �ϳ��� ������ ���� )
#define POPUPMENU_TRADE0						    402     
#define POPUPMENU_TRADE1						    403
#define POPUPMENU_TRADE2						    404

// ��� ����
#define POPUPMENU_GUILDINVITE		                221     // ��� �ʴ�( PC�� �˾� �޴� )
#define POPUPMENU_MAKEGUILD			                222     // ��� �����

#define POPUPMENU_STATUSRETRAIN		                223     // ���� ��й�

// ��Ʋ���� ����
#define POPUPMENU_BATTLEGROUND_MANAGE               224     // ��Ʋ���� 
#define POPUPMENU_BATTLEGROUNDPREV_RESULT           225     // ���� ��� ��� ����
#define POPUPMENU_MONEY_CHANGE                      226     // ȯ����
#define POPUPMENU_BATTLEGROUND_EXIT                 227     // ����� ������( �����ʻ� Ż���ư ����� NPC�� ������鼭 �Ⱦ� )

//---------------------------------------------
// �������� PopupMenu 

// �� ��¡��
#define POPUPMENU_TAXCONTROL						330		// �Ǹ� ���� ���� ����
#define POPUPMENU_CASTLERIGHTSETTING				331		// �� �������� ���� ���� ����
#define POPUPMENU_CASTLEEMBLEMUPGRADE				332		// �� ���� ��¡���� ���׷��̵�
#define POPUPMENU_CASTLEEMBLEMUPGRADEEXPLANATION	333		// �� ���� ���׷��̵� ���� ����explanation
#define POPUPMENU_CURRENTTEX						334		// ���� ��Ȳ

// ����
#define POPUPMENU_CASTLEOPEN						335		// ���� ����
#define POPUPMENU_CASTLECLOSE						336		// ���� �ݱ�
#define POPUPMENU_CASTLEDEFENCE						337		// ���� ����
#define POPUPMENU_CASTLEUPGRADE						338		// ���� ���׷��̵�
#define POPUPMENU_CASTLEREPAIR						339		// ���� ����
#define POPUPMENU_CASTLERECOVERY					340		// ���� ����

// �޹�
#define POPUPMENU_CASTLEIN							341		// �� ������
#define POPUPMENU_CASTLEOUT							342		// �� ������

// ����
#define POPUPMENU_CAMPPOSSAVE						343		// ��ġ ����
#define POPUPMENU_CAMPCARE							344		// ���� �����ϱ�
#define POPUPMENU_CAMPUPGRADE						345		// ���� ���׷��̵�
#define POPUPMENU_CAMPREPAIR						346		// ���� �����ϱ�
#define POPUPMENU_CAMPRIGHTSETTING					347		// ���� ���� ���� ����
#define POPUPMENU_CAMPDESTROY						348		// ���� �ı�
#define POPUPMENU_CAMPEXPLANATION					349		// ���� ���� (����)
#define POPUPMENU_CHANGEBASE						350		// �⺻������ ����
#define POPUPMENU_CHANGESHOP						351		// ���� Ÿ������ ����
#define POPUPMENU_CHANGEMINING						352		// ä���� Ÿ������ ����

// �ٰŸ� ��������
#define POPUPMENU_SHORTSIEGEARMSRIDE				353		// �ٰŸ� �������� ž��
#define POPUPMENU_SHORTSIEGEARMSREPAIR				354		// �ٰŸ� �������� ����
#define POPUPMENU_SHORTSIEGEARMSSTARTKITRECOVERY	355		// �ٰŸ� �������� ��Ÿ�� Ŷ ����

// ���Ÿ� ��������
#define POPUPMENU_LONGSIEGEARMSRIDE					356		// ���Ÿ� �������� ž��
#define POPUPMENU_LONGSIEGEARMSREPAIR				357		// ���Ÿ� �������� ����
#define POPUPMENU_LONGSIEGEARMSSTARTKITRECOVERY 	358		// ���Ÿ� �������� ��Ÿ�� Ŷ ����

// �ٰŸ� ��������
#define POPUPMENU_SHORTCASTLEARMSRIDE				359		// �ٰŸ� �������� ž��
#define POPUPMENU_SHORTCASTLEARMSREPAIR				360		// �ٰŸ� �������� ����
#define POPUPMENU_SHORTCASTLEARMSUPGRADE			361		// �ٰŸ� �������� ���׷��̵�
#define POPUPMENU_SHORTCASTLEARMSDESTROY			362		// �ٰŸ� �������� �ı�

// ���Ÿ� ��������
#define POPUPMENU_LONGCASTLEARMSRIDE				363		// ���Ÿ� �������� ž��
#define POPUPMENU_LONGCASTLEARMSREPAIR				364		// ���Ÿ� �������� ����
#define POPUPMENU_LONGCASTLEARMSUPGRADE				365		// ���Ÿ� �������� ���׷��̵�
#define POPUPMENU_LONGCASTLEARMSDESTROY				366		// ���Ÿ� �������� �ı�

// �����
#define POPUPMENU_DROPSHIPRIDE						367		// ���Ÿ� �������� ž��
#define POPUPMENU_DROPSHIPREPAIR					368		// ���Ÿ� �������� ����
#define POPUPMENU_DROPSHIPRECOVERY					369		// ���Ÿ� �������� ��Ÿ�� Ŷ ���� 

// ���� ���� NPC
#define POPUPMENU_CASTLEDEFENCEARMSMENU				370		// �� ��� ���� ���� ����
#define POPUPMENU_GUARDDEVELOP						371		// ���� ����
#define POPUPMENU_SHORTDEFENCEARMSDEVELOP			372		// �ٰŸ� ���� ���� ����
#define POPUPMENU_LONGDEFENCEARMSDEVELOP			373		// ���Ÿ� ��ƿ� ���� ����

// ����
#define POPUPMENU_CASTLEGUARDREPAIR					374		// ���Ÿ� �������� ����
#define POPUPMENU_CASTLEGUARDUPGRADE				375		// ���Ÿ� �������� ���׷��̵�
#define POPUPMENU_CASTLEGUARDDESTROY				376		// ���Ÿ� �������� �ı�

//---------------------------------------------

#define POPUPMENU_OPTIONCOMPOSITION				    379		// �ɼ� �̽�
#define POPUPMENU_COMPENSATION					    380		// ���� �Ǹ�
#define POPUPMENU_MININGCAMPMENU					381		// ä���� �޴�

#define POPUPMENU_WEAPONLAUNCH						382		// ���� ���� �߻�
#define POPUPMENU_WEAPONREPAIR						383		// ���� ���� ����
#define POPUPMENU_WEAPONRIGHTSETTING				384		// ���� ���� ���� ����
#define POPUPMENU_WEAPONDESTROY						385		// ���� ���� �ı�
#define POPUPMENU_WEAPONEXPLANATION					386		// ���� ���� ����
#define POPUPMENU_WEAPONLOAD						388		// ���� ���� ����

#define POPUPMENU_CAMPSHOPMENU						389		// ��� ��� ���� �޴�
#define POPUPMENU_WEAPONUPGRADE						390		// ���� ���� ���׷��̵�
#define POPUPMENU_CAMPNORIGHTMASTER					391		// ��� ��� ���� - ���� ����. ��� �����͸� ����
#define POPUPMENU_CAMPNORIGHTADMIN					392		// ��� ��� ���� - ���� ����. ��� ������ + ������ ����
#define POPUPMENU_CAMPSHOPADMIN						393		// ��� ��� ���� ����

// ��� ���� �߰�
#define POPUPMENU_GUILDKICK			                399     // ��� ����( PC�� �˾� �޴� )