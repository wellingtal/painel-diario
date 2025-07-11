// RYLMessageBoxManager.h: interface for the CRYLMessageBoxManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUIMESSAGEBOXMANAGER_H__BFC6391A_E840_459C_81E6_30E4224A983C__INCLUDED_)
#define AFX_GUIMESSAGEBOXMANAGER_H__BFC6391A_E840_459C_81E6_30E4224A983C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Texture.h>
#include <vector>
#include "RYLMessageBox.h"

using namespace std;

class CRYLMessageBoxManager  
{
	public:
		void DestroyList(bool bChk = false);
		void Render(LPDIRECT3DDEVICE8 lpD3DDevice);
		BOOL Update(void);
		void DeleteMessageBox(CRYLMessageBox *lpMessageBox);
		void Create(void);
		vector<CRYLMessageBox *> m_lstMessageBox;
		unsigned long m_dwMessageBoxTick;

		void AddMessageBox(CRYLMessageBox *lpMessageBox);
		CTexture *m_lpTexture;

		CRYLMessageBoxManager();
		~CRYLMessageBoxManager();

		BOOL GetIsExistModal()
		{
			vector<CRYLMessageBox *>::iterator it;
			for(it = m_lstMessageBox.begin(); it != m_lstMessageBox.end(); it++)
			{
				if((*it)->m_bModal) return TRUE;
			}

			return FALSE;
		}
};

extern CRYLMessageBoxManager g_MessageBoxManager;

#endif // !defined(AFX_GUIMESSAGEBOXMANAGER_H__BFC6391A_E840_459C_81E6_30E4224A983C__INCLUDED_)
