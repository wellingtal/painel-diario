// GUIMessageBoxManager.h: interface for the CGUIMessageBoxManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUIMESSAGEBOXMANAGER_H__BFC6391A_E840_459C_81E6_30E4224A983C__INCLUDED_)
#define AFX_GUIMESSAGEBOXMANAGER_H__BFC6391A_E840_459C_81E6_30E4224A983C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Texture.h>
#include <vector>
#include "GUIMessageBox.h"

using namespace std;

class CGUIMessageBoxManager  
{
	public:
		void DestroyList(void);
		void Render(LPDIRECT3DDEVICE8 lpD3DDevice);
		BOOL Update(void);
		void DeleteMessageBox(CGUIMessageBox *lpMessageBox);
		void Create(void);
		vector<CGUIMessageBox *> m_lstMessageBox;
		unsigned long m_dwMessageBoxTick;

		void AddMessageBox(CGUIMessageBox *lpMessageBox);
		CTexture *m_lpTexture;

		CGUIMessageBoxManager();
		~CGUIMessageBoxManager();

		BOOL GetIsExistModal()
		{
			vector<CGUIMessageBox *>::iterator it;
			for(it = m_lstMessageBox.begin(); it != m_lstMessageBox.end(); it++)
			{
				if((*it)->m_dwFlag) return TRUE;
			}

			return FALSE;
		}
};

extern CGUIMessageBoxManager g_MessageBoxManager;

#endif // !defined(AFX_GUIMESSAGEBOXMANAGER_H__BFC6391A_E840_459C_81E6_30E4224A983C__INCLUDED_)
