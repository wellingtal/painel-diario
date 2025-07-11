///////////////////////////////////////////////////////////////////////////////////////////////
//
// « ≈Õ
//
///////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _Filter
#define _Filter

namespace Filter
{
	const char LimitFileName[]							= "./Script/Server/WordFilter.txt";
	const char FilterFileName[]							= "./Script/Server/Abuse.txt";
	const char AccountFileName[]						= "./Script/Server/Account.txt";

	enum 
	{ 
		LIMIT_STRING_NUM	= 4000, 
		MAX_FILTER_LEN		= 15
	};

	struct FilterName
	{
		unsigned short Total;
		char FilterStrings[LIMIT_STRING_NUM][MAX_FILTER_LEN];
	};

	bool InitFilter(const char* szFilterFileName = NULL);
	bool NameCheck(const char *CheckName_In);
	bool AccountCheck(const char *Account_In);

	void ClearFilterDB(void);
	bool SaveFilterDBToBinary(const char* szFileNameBinary, const char* szTrashFile);
	bool LoadFilterDBFromBinary(const char* szFileNameBinary);
}

#endif