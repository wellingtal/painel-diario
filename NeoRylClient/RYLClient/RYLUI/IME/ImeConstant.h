#ifndef __IME_CONSTANT_H__
#define __IME_CONSTANT_H__

namespace ZIme
{
	// Define language ID
	enum LanguageID
	{
		LID_KOREAN					= 0x0412,
		LID_JAPANESE				= 0x0411,
		LID_TRADITIONAL_CHINESE		= 0x0404,
		LID_SIMPLIFIED_CHINESE		= 0x0804,
		LID_THAI					= 0x041e,

		MAX_LANGUAGE_ID			= 0xFFFF
	};

   enum LanguageSubID
   {
      LID_CHANGJIE            = 0xe002,
      LID_US_KEYBOARD         = 0x0404,
      LID_USYAMI              = 0xe020,

      MAX_LANGUAGE_SUB_ID     = 0xFFFF
   };
	
	enum Language
	{
		DEFAULT                 = 0,
		KOREAN,
		JAPANESE,
		TRADITIONAL_CHINESE,
		SIMPLIFIED_CHINESE,
      THAI,

		MAX_LANGUAGE
	};

   enum     
   { 
      KEY_KOREAN = 0, 
      KEY_JAPANESE, 
      KEY_TRADITIONAL_CHINESE, 
      KEY_SIMPLIFIED_CHINESE, 
      KEY_THAI,
      KEY_UNKNOWN_LANGUAGE 
   };

   enum     
   { 
      SUB_KEY_CHANJIE_TRADITIONAL_CHINESE = 0, 
      SUB_KEY_US_KEYBOARD_TRADITIONAL_CHINESE, 
      SUB_KEY_USYAMI_TRADITIONAL_CHINESE, 
      SUB_KEY_UNKNOWN 
   };

	// Define code pages 
	const int CodePage[ MAX_LANGUAGE ] =
	{
		0,						   // DEFAULT 
		949,					   // KOREAN 
		932,			   		   // JAPANESE
		950,					   // TRADITIONAL CHINESE
		936					   // SIMPLIFIED CHINESE
	};

	// Define charset
	const int CharSet[ MAX_LANGUAGE ] =
	{
		DEFAULT_CHARSET,		// Default 
		HANGUL_CHARSET,		// KOREAN
		SHIFTJIS_CHARSET,		// JAPANESE
		CHINESEBIG5_CHARSET,	// TRADITIONAL CHINESE
		GB2312_CHARSET			// SIMPLIFIED CHINESE
	};
};

#endif