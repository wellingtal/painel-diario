// ItemStorage.cpp: implementation of the CItemStorage class.
//
//////////////////////////////////////////////////////////////////////

#include "ItemStorage.h"
#include <BaseDataDefine.h>
#include "GMMemory.h"

CItemStorage g_ItemStorage;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemStorage::CItemStorage()
{
}

CItemStorage::~CItemStorage()
{
	DeleteItemTexture();
	DeleteSkillTexture();
	DeleteQuestTexture();
}

void CItemStorage::DeleteItemTexture(void)
{
	vector<TexStorage *>::iterator it;
	TexStorage *lpTexture;
	for(it = m_lstItemTexture.begin(); it != m_lstItemTexture.end();)
	{
		if((*it)->lpTexture) { delete (*it)->lpTexture; (*it)->lpTexture = NULL; }
		lpTexture = (*it);
		it = m_lstItemTexture.erase(it);
		delete lpTexture;
	}
	m_lstItemTexture.clear();
}

void CItemStorage::DeleteSkillTexture(void)
{
	vector<TexStorage *>::iterator it;
	TexStorage *lpTexture;
	for(it = m_lstSkillTexture.begin(); it != m_lstSkillTexture.end();)
	{
		if((*it)->lpTexture) { delete (*it)->lpTexture; (*it)->lpTexture = NULL; }
		lpTexture = (*it);
		it = m_lstSkillTexture.erase(it);
		delete lpTexture;
	}
	m_lstSkillTexture.clear();
}

void CItemStorage::DeleteQuestTexture(void)
{
	vector<TexStorage *>::iterator it;
	TexStorage *lpTexture;
	for(it = m_lstQuestTexture.begin(); it != m_lstQuestTexture.end();)
	{
		if((*it)->lpTexture) { delete (*it)->lpTexture; (*it)->lpTexture = NULL; }
		lpTexture = (*it);
		it = m_lstQuestTexture.erase(it);
		delete lpTexture;
	}
	m_lstQuestTexture.clear();
}

CTexture *CItemStorage::GetItemTexture(const char *strTextureFile)
{
	char strName[MAX_PATH];
	sprintf(strName, "\\item\\%s.dds", strTextureFile);

	vector<TexStorage *>::iterator it;
	for(it = m_lstItemTexture.begin(); it != m_lstItemTexture.end(); it++)
	{
		if(!strcmp((*it)->strFileName, strName))
		{
			return (*it)->lpTexture;
		}
	}

	TexStorage *lpStorage = new TexStorage;
	CTexture::SetPath(INTERFACETEXTUREPATH);
	lpStorage->lpTexture = new CTexture;
	lpStorage->lpTexture->Load(strName);
	strcpy(lpStorage->strFileName, strName);

	m_lstItemTexture.push_back(lpStorage);

	return lpStorage->lpTexture;
}

CTexture *CItemStorage::GetSkillTexture(const char *strTextureFile, BOOL bEnable)
{
	char strName[MAX_PATH];

	if(bEnable == TRUE)
		sprintf(strName, "\\item\\%s", strTextureFile);
	else
		sprintf(strName, "\\item\\d%s", strTextureFile);

	vector<TexStorage *>::iterator it;
	for(it = m_lstSkillTexture.begin(); it != m_lstSkillTexture.end(); it++)
	{
		if(!strcmp((*it)->strFileName, strName))
		{
			return (*it)->lpTexture;
		}
	}

	TexStorage *lpStorage = new TexStorage;
	CTexture::SetPath(INTERFACETEXTUREPATH);
	lpStorage->lpTexture = new CTexture;
	lpStorage->lpTexture->Load(strName);
	strcpy(lpStorage->strFileName, strName);

	m_lstSkillTexture.push_back(lpStorage);

	return lpStorage->lpTexture;
}

CTexture *CItemStorage::GetQuestTexture(const char *strTextureFile)
{
	char strName[MAX_PATH];
	sprintf(strName, "\\%s", strTextureFile);

	vector<TexStorage *>::iterator it;
	for(it = m_lstQuestTexture.begin(); it != m_lstQuestTexture.end(); it++)
	{
		if(!strcmp((*it)->strFileName, strName))
		{
			return (*it)->lpTexture;
		}
	}

	TexStorage *lpStorage = new TexStorage;
	CTexture::SetPath(INTERFACETEXTUREPATH);
	lpStorage->lpTexture = new CTexture;
	lpStorage->lpTexture->Load(strName);
	strcpy(lpStorage->strFileName, strName);

	m_lstQuestTexture.push_back(lpStorage);

	return lpStorage->lpTexture;
}
