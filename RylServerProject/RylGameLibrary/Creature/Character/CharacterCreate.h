
#include "CharacterClass.h"

namespace CharCreate
{
	enum Const
	{
		START_GOLD			= 0,//hayzohar start gold
		MAX_START_POS_NUM	= 6,

		MAX_LOBBY_RESPAWN_POS = 2
	};

	// ÆÄÆ® 1¿ë ¸®Á¨Æ÷ÀÎÆ® °¬´Ù ½á¶ó.
	const POS StartPosNum[CClass::MAX_RACE][MAX_START_POS_NUM] = {
		// ÈÞ¸Õ
		{
			//{ 2812,	15, 2498 },
			//{ 2817,	15, 2471 },
			//{ 2792,	15, 2515 },
			//{ 2792,	15, 2515 },
			//{ 2792,	15, 2515 },
			//{ 2792,	15, 2515 }

			{ 2807,	10, 2498 },
			{ 2807,	10, 2498 },
			{ 2807,	10, 2498 },
			{ 2807,	10, 2498 },
			{ 2807,	10, 2498 },
			{ 2807,	10, 2498 }
		},

		// ¾ÆÄ­
		{
			{ 2807,	10, 2498 },
			{ 2807,	10, 2498 },
			{ 2807,	10, 2498 },
			{ 2807,	10, 2498 },
			{ 2807,	10, 2498 },
			{ 2807,	10, 2498 }
		}
	};
/*
	const POS StartPosNum[CClass::MAX_RACE][MAX_START_POS_NUM] = {
		// ÈÞ¸Õ
		{
			{ 2812,	15, 2498 },
			{ 2817,	15, 2471 },
			{ 2792,	15, 2515 },
			{ 2792,	15, 2515 },
			{ 2792,	15, 2515 },
			{ 2792,	15, 2515 }
		},

		// ¾ÆÄ­
		{
			{ 2812,	15, 2498 },
			{ 2834,	15, 2482 },
			{ 2804,	15, 2514 },
			{ 2804,	15, 2514 },
			{ 2804,	15, 2514 },
			{ 2804,	15, 2514 }
		}
	};
*/
	const POS BGServerStartPos[CClass::MAX_RACE][MAX_LOBBY_RESPAWN_POS] = {//start position hayzohar battleground pos
		// Human
		{ 
            { 2165, 1135, 1005 },
            { 1727, 1135, 1005 } 
        },

		// Akhan
		{ 
            { 2119, 1132, 1841 },		
            { 1683, 1132, 1841 } 
        }
	};

	const unsigned long StartPointVariation[MAX_START_POS_NUM] = { 
		500, 1000, 1500, 2000, 2500, 0xFFFFFFFF 
	};

	const unsigned short ALLOW_HAN_NUM = 39;
	const unsigned short AllowHans[ALLOW_HAN_NUM] = {
		'°¦', '°§',	'°Ê', '°þ', '³ê',
		'´Ó', '·×', '·Ü', '·Ý', '·Þ',
		'·à', '·ñ', '·õ', '·ö', '·÷',
		'·ø', '·þ', '¸¡', '¸«', '¸¬',
		'¸×', '¸õ', '¹¾', '¹Å', '¹Ü',
		'¹õ', '»Î', '¾·', 'Áâ', 'Áé',
		'Â¨', 'Ãñ', 'Äú', 'Ç§', 'Ç¨',
		'Çµ', 'ÇÓ', 'È§', 'Èè'
	};

	const unsigned short ALLOW_LETTER_NUM = 2;
	const char AllowLetters[ALLOW_LETTER_NUM] = {
		'-', '_'
	};

	const unsigned short DISALLOW_LETTER_NUM = 9;
	const char DisAllowLetters[DISALLOW_LETTER_NUM] = {
		' ', '\'', '\"', '#', '~', '!', '@', '[', ']'
	};


	bool CheckCharCreateName(const char *Name_In, bool HanCheck_In);
	bool CheckCharCreateData(CHAR_CREATE &CharCreate_In);

	unsigned long GetDefaultStartGold(void);
	POS	GetDefaultCharacterPos(unsigned long dwRace, unsigned long dwRacePlayerNum);
}