// ItemStorage.h: interface for the CItemStorage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMSTORAGE_H__6BA291F9_CE20_4C62_8856_CD949BC00BE4__INCLUDED_)
#define AFX_ITEMSTORAGE_H__6BA291F9_CE20_4C62_8856_CD949BC00BE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <texture.h>
#include <vector>

using namespace std;

typedef struct TexStorage
{
	char strFileName[MAX_PATH];
	CTexture *lpTexture;
} TexStorage;

class CItemStorage  
{
	public:
/*		vector<char *> m_lstItemTextureFile;
		vector<char *> m_lstSkillTextureFile;
		vector<CTexture *> m_lstItemTexture;
		vector<CTexture *> m_lstSkillTexture;*/
		vector<TexStorage *> m_lstItemTexture;
		vector<TexStorage *> m_lstSkillTexture;
		vector<TexStorage *> m_lstQuestTexture;

		CItemStorage();
		~CItemStorage();

		void DeleteItemTexture(void);
		void DeleteSkillTexture(void);
		void DeleteQuestTexture(void);

		CTexture *GetItemTexture(const char *strTextureFile);
		CTexture *GetSkillTexture(const char *strTextureFile, BOOL bEnable = TRUE);
		CTexture *GetQuestTexture(const char *strTextureFile);
};

extern CItemStorage g_ItemStorage;

#endif // !defined(AFX_ITEMSTORAGE_H__6BA291F9_CE20_4C62_8856_CD949BC00BE4__INCLUDED_)
