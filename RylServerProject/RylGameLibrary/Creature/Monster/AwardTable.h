#pragma once

#ifndef _AWARD_TABLE_H_
#define _AWARD_TABLE_H_

#include <Item/ItemConstants.h>

#include <Creature/Character/CharacterClass.h>
#include <Creature/AggresiveCreature.h>

class CMonster;
class CCharacter;

namespace AwardTable
{
	enum Const
	{
		MAX_DROP_ITEM		= 5,		// 한 몬스터가 최대로 떨구는 아이템 수

		MAX_MONSTER_LEVEL	= 109,		// 몬스터의 최고 레벨 
										// (보석, 광물, 물약, 스킬북 드랍 테이블용으로 실제와는 다르다. Creature::MONSTER_MAX_LEVEL 참고)
										
		SKILL_KIND			= 89,		// 스킬(북)의 종류 (스킬북이 없는 스킬도 있다.) hayzohar was 88 for add new skill
		FIFTHSKILL_KIND		= 15,		// 스킬(북)의 종류 (스킬북이 없는 스킬도 있다.)

		GEM_DROP_TABLE_COLUMN			= 11,		// 보석 드랍 테이블 세로 크기
		GEM_DROP_TABLE_ROW				= 3,		// 보석 드랍 테이블 가로 크기
		METAL_DROP_TABLE_COLUMN			= 11, 		// 광물 드랍 테이블 세로 크기
		METAL_DROP_TABLE_ROW			= 7, 		// 광물 드랍 테이블 가로 크기
		POTION_DROP_TABLE_COLUMN		= 11, 		// 물약 드랍 테이블 세로 크기
		POTION_DROP_TABLE_ROW			= 8, 		// 물약 드랍 테이블 가로 크기

		CASH_DROP_TABLE_COLUMN			= 13, 		// 캐쉬 드랍 테이블 세로 크기
		CASH_DROP_TABLE_ROW				= 7, 		// 캐쉬 드랍 테이블 가로 크기

		SKILLBOOK_DROP_TABLE_COLUMN		= 11, 		// 스킬북 드랍 테이블 세로 크기

		EQUIP_MAX_NUM_PER_LEVEL			= 15,		// 레벨당 드랍되는 장비의 종류수

		BLACK_MARKET_TABLE_COLUMN		= 228,		// 암시장 테이블 세로 크기
		BLACK_MARKET_TABLE_ROW			= 13,		// 암시장 테이블 가로 크기

		PRIMARY_EQUIP_TABLE_ROW			= 9,		// 장비 드랍 우선 순위 테이블 가로 크기
		HUMAN_EQUIP_TYPE_NUM			= 19,		// 휴먼 장비 타입 갯수
		AKHAN_EQUIP_TYPE_NUM			= 19		// 아칸 장비 타입 갯수
	};


	// 아이템 드랍률
	// Desc : 각 값은 n/1000 의 확률을 의미한다. /* Each value has the probability that is n/1000 */
	const unsigned short aryItemDropRate[CAggresiveCreature::AGGRAVATION_NUM][MAX_DROP_ITEM] = { 
		{ 	1,			1,			1,			1,			1	}, // -20
		{ 	1,			1,			1,			1,			1	},
		{ 	1,			1,			1,			1,			1	},
		{ 	1,			1,			1,			1,			1	},
		{ 	1,			1,			1,			1,			1	},
		{ 	1,			1,			1,			1,			1	},
		{ 	1,			1,			1,			1,			1	},
		{ 	1,			1,			1,			1,			1	},
		{ 	1,			1,			1,			1,			1	},
		{ 	1,			1,			1,			1,			1	},
		{ 	1,			1,			1,			1,			1	},
		{ 	91,			1,			1,			1,			1	},
		{ 	189,		1,			1,			1,			1	},
		{ 	280,		1,			1,			1,			1	},
		{ 	364,		1,			1,			1,			1	},
		{ 	441,		1,			1,			1,			1	},  // -5
		{ 	511,		1,			1,			1,			1	},
		{ 	574,		1,			1,			1,			1	},
		{ 	623,		1,			1,			1,			1	},
		{ 	665,		91,			1,			1,			1	},	// -1
		{ 	700,		189,		1,			1,			1	},	// 0
		{ 	710,		280,		1,			1,			1	},	// +1
		{ 	720,		364,		1,			1,			1	},
		{ 	730,		441,		1,			1,			1	},
		{ 	740,		511,		1,			1,			1	},
		{ 	750,		574,		1,			1,			1	},  // +5
		{ 	760,		623,		91,			1,			1	},
		{ 	800,		665,		189,		1,			1	},
		{ 	800,		700,		280,		1,			1	},
		{ 	800,		710,		364,		91,			1	},  // +9
		{ 	800,		720,		441,		189,		1	},
		{ 	800,		730,		511,		280,		1	},
		{ 	800,		740,		574,		364,		91	},  // +12
		{ 	900,		750,		623,		441,		189	},
		{ 	900,		760,		665,		511,		280	},
		{ 	900,		800,		700,		574,		364	},  // +15
		{ 	900,		800,		710,		623,		441	},  // +16
		{ 	900,		800,		720,		665,		511	},
		{ 	900,		800,		730,		700,		574	},
		{ 	900,		800,		740,		710,		623	},
		{ 	900,		900,		750,		720,		665	}	// +20
	};
/*
	const unsigned short aryItemDropRate[CAggresiveCreature::AGGRAVATION_NUM][MAX_DROP_ITEM] = { 
		{ 	0,		0,		0,		0,		0,		0,		0,		0,		0,		0	},
		{ 	161,	1,		1,		1,		1,		1,		1,		1,		1,		1	},
		{ 	195,	1,		1,		1,		1,		1,		1,		1,		1,		1	},
		{ 	228,	1,		1,		1,		1,		1,		1,		1,		1,		1	},
		{ 	262,	1,		1,		1,		1,		1,		1,		1,		1,		1	},
		{ 	296,	1,		1,		1,		1,		1,		1,		1,		1,		1	},
		{ 	392,	1,		1,		1,		1,		1,		1,		1,		1,		1	},
		{ 	363,	1,		1,		1,		1,		1,		1,		1,		1,		1	},
		{ 	396,	1,		1,		1,		1,		1,		1,		1,		1,		1	},
		{ 	430,	1,		1,		1,		1,		1,		1,		1,		1,		1	},
		{ 	464,	1,		1,		1,		1,		1,		1,		1,		1,		1	},
		{ 	497,	1,		1,		1,		1,		1,		1,		1,		1,		1	},
		{ 	531,	1,		1,		1,		1,		1,		1,		1,		1,		1	},
		{ 	564,	1,		1,		1,		1,		1,		1,		1,		1,		1	},
		{ 	598,	1,		1,		1,		1,		1,		1,		1,		1,		1	},
		{ 	632,	1,		1,		1,		1,		1,		1,		1,		1,		1	},
		{ 	665,	1,		1,		1,		1,		1,		1,		1,		1,		1	},
		{ 	699,	1,		1,		1,		1,		1,		1,		1,		1,		1	},
		{ 	732,	1,		1,		1,		1,		1,		1,		1,		1,		1	},
		{ 	766,	1,		1,		1,		1,		1,		1,		1,		1,		1	},
		{ 	800,	1,		1,		1,		1,		1,		1,		1,		1,		1	},
		{ 	900,	51,		1,		1,		1,		1,		1,		1,		1,		1	},
		{ 	900,	252,	1,		1,		1,		1,		1,		1,		1,		1	},
		{ 	900,	504,	1,		1,		1,		1,		1,		1,		1,		1	},
		{ 	900,	807,	1,		1,		1,		1,		1,		1,		1,		1	},
		{ 	900,	900,	260,	1,		1,		1,		1,		1,		1,		1	},
		{ 	900,	900,	663,	1,		1,		1,		1,		1,		1,		1	},
		{ 	900,	900,	900,	216,	1,		1,		1,		1,		1,		1	},
		{ 	900,	900,	900,	720,	1,		1,		1,		1,		1,		1	},
		{ 	900,	900,	900,	900,	375,	1,		1,		1,		1,		1	},
		{ 	900,	900,	900,	900,	900,	80,		1,		1,		1,		1	},
		{ 	900,	900,	900,	900,	900,	735,	1,		1,		1,		1	},
		{ 	900,	900,	900,	900,	900,	900,	540,	1,		1,		1	},
		{ 	900,	900,	900,	900,	900,	900,	900,	396,	1,		1	},
		{ 	900,	900,	900,	900,	900,	900,	900,	900,	303,	1	},
		{ 	900,	900,	900,	900,	900,	900,	900,	900,	900,	260	},
		{ 	900,	900,	900,	900,	900,	900,	900,	900,	900,	900	},
		{ 	900,	900,	900,	900,	900,	900,	900,	900,	900,	900	},
		{ 	900,	900,	900,	900,	900,	900,	900,	900,	900,	900	},
		{ 	900,	900,	900,	900,	900,	900,	900,	900,	900,	900	},
		{ 	900,	900,	900,	900,	900,	900,	900,	900,	900,	900	}
	};
*/

	const short aryGemDropTable[GEM_DROP_TABLE_COLUMN][GEM_DROP_TABLE_ROW] = {
		//	보석조각	작은보석	일반보석		/* Jewelry pieces | Small jewelry | Common Jewelry*/
		{	100,		0,			0			},	// 10
		{	90,			10,			0			},	// 20
		{	70,			20,			10			},	// 30
		{	50,			30,			20			},	// 40
		{	30,			40,			30			},	// 50
		{	10,			50,			40			},	// 60
		{	10,			30,			60			},	// 70
		{	0,			20,			80			},	// 80
		{	0,			10,			90			},	// 90
		{	0,			10,			90			},	// 100
		{	0,			0,			100			}	// 110
	};

	const short aryMetalDropTable[METAL_DROP_TABLE_COLUMN][METAL_DROP_TABLE_ROW] = {
		//	Silvin		Mithril		Iternium	Soul Metal	Antonium	AncientStone 	Core				
		{	90,			0,			0,			0,			0,			0,			10			},	// 10
		{	81,			9,			0,			0,			0,			0,			10			},	// 20
		{	63,			18,			9,			0,			0,			0,			10			},	// 30
		{	45,			27,			18,			0,			0,			0,			10			},	// 40
		{	27,			36,			27,			0,			0,			0,			10			},	// 50
		{	9,			45,			36,			0,			0,			0,			10			},	// 60
		{	9,			27,			54,			0,			0,			0,			10			},	// 70
		{	0,			18,			72,			0,			0,			0,			10			},	// 80
		{	0,			9,			81,			0,			0,			0,			10			},	// 90
		{	0,			9,			81,			0,			0,			0,			10			},	// 100
		{	0,			0,			90,			0,			0,			0,			10			}	// 110
	};

	const short aryPotionDropTable[POTION_DROP_TABLE_COLUMN][POTION_DROP_TABLE_ROW] = {
		//	빵			생고기		구운고기	베이컨		버프물약1	버프물약2	버프물약3	버프물약4		(	Bacon		Meat		Grilled Meat	Bacon	Potion1		Potion2		Potion3		Potion4)
		//	만나1		만나2		만나3		만나4	    레벨		레벨		레벨		레벨			(	Meet? 1		Meet2		Meet3		Meet4		Level		Level		Level		Level)
		//  스프1		스프2		스프3		스프4															(	Soup1		Soup2		Soup3		Soup4)
		{	70,			0,			0,			0,			16,			8,			4,			2		},		// 10
		{	35,			35,			0,			0,			16,			8,			4,			2		},		// 20
		{	23,			23,			24,			0,			16,			8,			4,			2		},		// 30
		{	17,			17,			18,			18,			16,			8,			4,			2		},		// 40
		{	0,			23,			23,			24,			16,			8,			4,			2		},		// 50
		{	0,			23,			35,			24,			16,			8,			4,			2		},		// 60
		{	0,			0,			35,			35,			16,			8,			4,			2		},		// 70
		{	0,			0,			1,			70,			16,			8,			4,			2		},		// 80
		{	0,			0,			0,			70,			16,			8,			4,			2		},		// 90
		{	0,			0,			0,			70,			16,			8,			4,			2		},		// 100
		{	0,			0,			0,			70,			16,			8,			4,			2		}		// 110
	};


	const short aryCashDropTable[CASH_DROP_TABLE_COLUMN][CASH_DROP_TABLE_ROW] = {
		//	Oblivion  	Holy	Almighty		Orbs		Endurance	Rebirth 	Mount
		{	20,			60,			10,			10,			0,			0,			0		},		// 10
		{	20,			60,			10,			10,			0,			0,			0		},		// 20
		{	20,			60,			10,			10,			0,			0,			0		},		// 30
		{	20,			60,			10,			10,			0,			0,			0		},		// 40
		{	20,			60,			10,			10,			0,			0,			0		},		// 50
		{	20,			60,			10,			10,			0,			0,			0		},		// 60
		{	20,			60,			10,			10,			0,			0,			0		},		// 70
		{	20,			60,			10,			10,			0,			0,			0		},		// 80
		{	20,			60,			10,			10,			0,			0,			0		},		// 90
		{	20,			60,			10,			10,			0,			0,			0		},		// 100
		{	20,			60,			10,			10,			0,			0,			0		},		// 110
		{	20,			60,			10,			10,			0,			0,			0		},		// 치프 (Chief)
		{	0,			20,			50,			16,			8,			4,			2		},		// 네임드 (Named)
	};


	const short arySkillBookDropTable[SKILLBOOK_DROP_TABLE_COLUMN] = {
		1, 1, 2, 2, 2, 
		3, 3, 3, 4, 4,
		5
	};

	const unsigned short aryDropableSkillbookList[SKILL_KIND] = {
		// 17 common skills of human
		0x8104, 0x8102, 0x8103, 0x8105, 0x8203, 0x8205, 0x8302, 0x8303, 0x8402, 0x8405, 
		0x8106, 0x8107, 0x8202, 0x8304, 0x8305, 0x8306, 0x8404,	0x9114,									//0x9114 is staff\scyte mastery skill added by hayzohar

		// Main 29 Human Skills
		0x8503, 0x8504, 0x8602, 0x8603, 0x8604, 0x8702, 0x8704, 0x8705, 0x8706, 0x8803, 
		0x8804, 0x8805, 0x8902, 0x8903, 0x8904, 0x8905, 0x8A02, 0x8A04, 0x8A05, 0x8A06, 
		0x8B02, 0x8B03, 0x8B04, 0x8B05, 0x8B06, 0x8C02, 0x8C03, 0x8C04, 0x8C08,

		// 17 common skill akkans
		0x9102, 0x9105, 0x9104, 0x9202, 0x9106, 0x9206, 0x9207, 0x9213, 0x9209, 0x9108, 
		0x9109, 0x9110, 0x9204, 0x9208, 0x9210, 0x9212, 0x9214, 

		// Main 25 akkan Skills
		0x9302, 0x9305, 0x9307, 0x9402, 0x9404, 0x9405, 0x9407, 0x9502, 0x9504, 0x9506, 
		0x9602, 0x9603, 0x9604, 0x9605, 0x9606, 0x9703, 0x9704, 0x9706, 0x9802, 0x9803, 
		0x9804, 0x9805, 0x9303, 0x9708, 0x9709
	};

	const unsigned short aryDropableFifthSkillbookList[FIFTHSKILL_KIND] = {
		// Human 5th Skills
		0x8603, 0x8503, 0x8704, 0x8804, 0x8902, 0x8a05, 0x8c02, 0x8b04,

		// Akkan 5th Skills
		0x9402, 0x9302, 0x9502, 0x9606, 0x9603, 0x9706, 0x9804
	};

	using namespace Item::ItemType;
	const unsigned char aryPrimaryEquipList[CClass::MAX_CLASS][PRIMARY_EQUIP_TABLE_ROW] = {
		{	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE	},				  

		{	CON_HELM,	CON_GLOVE,	CON_BOOTS,	CON_ARMOUR,	ONEHANDED_SWORD,	TWOHANDED_SWORD,	ONEHANDED_AXE,	TWOHANDED_AXE,	SHIELD			},	// 파이터
		{	DEX_HELM,	DEX_GLOVE,	DEX_BOOTS,	DEX_ARMOUR,	DAGGER,				BOW,				CROSSBOW,		SHIELD,			MAX_ITEM_TYPE	},	// 로그
		{	DEX_HELM,	DEX_GLOVE,	DEX_BOOTS,	DEX_ARMOUR,	STAFF,				DAGGER,				MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE	},	// 메이지
		{	CON_HELM,	CON_GLOVE,	CON_BOOTS,	CON_ARMOUR,	ONEHANDED_BLUNT,	TWOHANDED_BLUNT,	SHIELD,			MAX_ITEM_TYPE,	MAX_ITEM_TYPE	},	// 어콜라이트
		{	CON_HELM,	CON_GLOVE,	CON_BOOTS,	CON_ARMOUR,	ONEHANDED_BLUNT,	TWOHANDED_BLUNT,	ONEHANDED_AXE,	TWOHANDED_AXE,	SHIELD			},	// 디펜더
		{	CON_HELM,	CON_GLOVE,	CON_BOOTS,	CON_ARMOUR,	ONEHANDED_SWORD,	TWOHANDED_SWORD,	ONEHANDED_AXE,	TWOHANDED_AXE,	MAX_ITEM_TYPE	},	// 워리어
		{	DEX_HELM,	DEX_GLOVE,	DEX_BOOTS,	DEX_ARMOUR,	DAGGER,				ONEHANDED_SWORD,	ONEHANDED_AXE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE	},	// 어쌔신
		{	DEX_HELM,	DEX_GLOVE,	DEX_BOOTS,	DEX_ARMOUR,	DAGGER,				BOW,				CROSSBOW,		MAX_ITEM_TYPE,	MAX_ITEM_TYPE	},	// 아처
		{	DEX_HELM,	DEX_GLOVE,	DEX_BOOTS,	DEX_ARMOUR,	STAFF,				DAGGER,				MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE	},	// 소서러
		{	DEX_HELM,	DEX_GLOVE,	DEX_BOOTS,	DEX_ARMOUR,	STAFF,				BOW,				CROSSBOW,		MAX_ITEM_TYPE,	MAX_ITEM_TYPE	},	// 엔찬터
		{	CON_HELM,	CON_GLOVE,	CON_BOOTS,	CON_ARMOUR,	ONEHANDED_BLUNT,	TWOHANDED_BLUNT,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE	},	// 프리스트
		{	CON_HELM,	CON_GLOVE,	CON_BOOTS,	CON_ARMOUR,	ONEHANDED_BLUNT,	TWOHANDED_BLUNT,	SHIELD,			MAX_ITEM_TYPE,	MAX_ITEM_TYPE	},	// 클레릭
																																
		{	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE	},				  
		{	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE	},				  
		{	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE	},				  
		{	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE	},				  

		{	CON_HEAD,	CON_PROTECT_A,	CON_PELVIS,	CON_BODY,	COM_SWORD,	COM_BLUNT,		MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE	},	// 컴배턴트
		{	DEX_HEAD,	DEX_PROTECT_A,	DEX_PELVIS,	DEX_BODY,	OPP_AXE,	OPP_SLUSHER,	OPP_TALON,		OPP_SYTHE,		MAX_ITEM_TYPE	},	// 오피세이터	
		{	CON_HEAD,	CON_PROTECT_A,	CON_PELVIS,	CON_BODY,	COM_SWORD,	COM_BLUNT,		SKILL_A_GUARD,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE	},	// 템플러
		{	CON_HEAD,	CON_PROTECT_A,	CON_PELVIS,	CON_BODY,	COM_SWORD,	COM_BLUNT,		SKILL_A_ATTACK,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE	},	// 어태커
		{	CON_HEAD,	CON_PROTECT_A,	CON_PELVIS,	CON_BODY,	COM_SWORD,	COM_BLUNT,		SKILL_A_GUN,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE	},	// 거너
		{	DEX_HEAD,	DEX_PROTECT_A,	DEX_PELVIS,	DEX_BODY,	OPP_SYTHE,	OPP_SLUSHER,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE	},	// 룬오프
		{	DEX_HEAD,	DEX_PROTECT_A,	DEX_PELVIS,	DEX_BODY,	OPP_HAMMER,	OPP_AXE,		MAX_ITEM_TYPE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE	},	// 라이프오프
		{	DEX_HEAD,	DEX_PROTECT_A,	DEX_PELVIS,	DEX_BODY,	OPP_TALON,	OPP_SLUSHER,	SKILL_A_KNIFE,	MAX_ITEM_TYPE,	MAX_ITEM_TYPE	}	// 쉐도우오프
	};

	const unsigned char aryHumanEquipTypeList[HUMAN_EQUIP_TYPE_NUM] = { //what human can hold hayzohar
		CON_ARMOUR,			CON_HELM,			CON_GLOVE,			CON_BOOTS,		
		DEX_ARMOUR,			DEX_HELM,			DEX_GLOVE,			DEX_BOOTS,		

		ONEHANDED_SWORD,	TWOHANDED_SWORD,	ONEHANDED_AXE,		TWOHANDED_AXE,	
		ONEHANDED_BLUNT,	TWOHANDED_BLUNT,	BOW,				CROSSBOW,		
		STAFF,				DAGGER,				SHIELD
	};

	const unsigned char aryAkhanEquipTypeList[AKHAN_EQUIP_TYPE_NUM] = { //what akhan can hold hayzohar
		CON_BODY,			CON_HEAD,			CON_PELVIS,			CON_PROTECT_A,		
		DEX_BODY,			DEX_HEAD,			DEX_PELVIS,			DEX_PROTECT_A,		

		COM_BLUNT,			COM_SWORD,			OPP_HAMMER,			OPP_AXE,				
		OPP_SLUSHER,		OPP_TALON,			OPP_SYTHE,				

		SKILL_A_GUARD,		SKILL_A_ATTACK,		SKILL_A_GUN,		SKILL_A_KNIFE		
	};


	class CAward : public CSingleton<CAward>
	{
	public:

		unsigned long GetAward(unsigned char cItemKind, CMonster* lpDeadMonster, CCharacter* lpAttackCharacter, 
			Item::CItem** lpItem_Out);

		unsigned short GetAwardEquipment(Item::EquipType::OptionType eOptionType, 
			unsigned char cDeadMonsterLevel, CCharacter* lpAttackCharacter, bool bQuestAward);

		unsigned short GetQuestEquipmentClass(Item::EquipType::OptionType eOptionType, unsigned char cClass,
			unsigned char cDeadMonsterLevel, CCharacter* lpAttackCharacter);

	private:

		unsigned long GetAwardCoin(unsigned char cDeadMonsterLevel, float cAddPer);
		unsigned long GetAwardGem(unsigned char cDeadMonsterLevel);
		unsigned long GetAwardMetal(unsigned char cDeadMonsterLevel);
		unsigned long GetAwardPotion(unsigned char cDeadMonsterLevel);
		unsigned long GetAwardSkill(unsigned char cDeadMonsterLevel, char cAttackCharacterNation);
		unsigned long GetAwardCashItem(char cAttackCharacterNation, unsigned char cDeadMonsterLevel, unsigned int cDeadMonsterPattern);
		unsigned long GetAwardLottery(void);

		//--//	start..
		unsigned long GetAwardRing(unsigned char cLevel, unsigned char cGrade);
		unsigned long GetAwardNecklace(unsigned char cLevel, unsigned char cGrade);
		unsigned long GetAwardRune(unsigned char cLevel, unsigned char cGrade);
		unsigned long GetAwardDestructionRune();
		//--//	end..

		unsigned short SelectItemByRace(Item::EquipType::OptionType eOptionType, 
			const unsigned char* aryItemType, unsigned char cMaxItemType, unsigned char cLevel);

		static CAward	ms_this;
	};
}

#endif