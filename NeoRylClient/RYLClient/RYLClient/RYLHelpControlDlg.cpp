
#include "BaseDataDefine.h"

#include "GMUICustomPanel.h"
#include "GMImageDib.h"

#include "RYLImage.h"
#include "RYLImageManager.h"
#include "RYLSpriteList.h"
#include "RYLSpriteEX.h"
#include "RYLSprite.h"

#include "RYLHelpControlDlg.h"
#include "GMMemory.h"

CRYLHelpControlDlg*	g_pHelpControlDlg = NULL;

CRYLHelpControlDlg::CRYLHelpControlDlg(CGMUICustomDlg *pParentDlg) 
:	CGMUICustomPanel(pParentDlg)
{
	m_pGMUIParentDlg		= pParentDlg;

	CGMImageDib*	pImageDIB = NULL;
	CRYLImage*		pImage	  = NULL;

	CRYLSpriteList::Instance()->CreateGMImage(512, 512, &pImageDIB);
	pImageDIB->ClearbyColor(0x00000000);
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage();
	pImage->LoadFromImage(pImageDIB);
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage(pImage);
	
	AttachPanelImage((CGMTexture*)pImage);

	GM_DELETE(pImageDIB);

	SetWidth(512);
	SetHeight(512);
	SetLeft((CGMUIScreen::GetInstance()->GetWidth() - GetWidth()) / 2);
	SetTop((CGMUIScreen::GetInstance()->GetHeight() - GetHeight()) / 2);
	
	g_pHelpControlDlg = this;	

	InitCtrl();
}

CRYLHelpControlDlg::~CRYLHelpControlDlg()
{
	FinalCtrl();
}

void	CRYLHelpControlDlg::InitCtrl(void)
{
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite(ERYLSPRITENAME::RYL_MAINGUI_256x256);
	
	m_pHelpControlSprite[ 0 ] = new CRYLSpriteEx(m_pGMUIParentDlg);
	m_pHelpControlSprite[ 0 ]->m_pSprite = new CRYLSprite;
	m_pHelpControlSprite[ 0 ]->m_pTexture = new CTexture;
	m_pHelpControlSprite[ 0 ]->SetLeft(0);
	m_pHelpControlSprite[ 0 ]->SetTop(0);
	m_pHelpControlSprite[ 0 ]->SetWidth(256);
	m_pHelpControlSprite[ 0 ]->SetHeight(256);
	CGMUICustomPanel::AddChild(m_pHelpControlSprite[ 0 ]);

	m_pHelpControlSprite[ 1 ] = new CRYLSpriteEx(m_pGMUIParentDlg);
	m_pHelpControlSprite[ 1 ]->m_pSprite = new CRYLSprite;
	m_pHelpControlSprite[ 1 ]->m_pTexture = new CTexture;
	m_pHelpControlSprite[ 1 ]->SetLeft(256);
	m_pHelpControlSprite[ 1 ]->SetTop(0);
	m_pHelpControlSprite[ 1 ]->SetWidth(256);
	m_pHelpControlSprite[ 1 ]->SetHeight(256);
	CGMUICustomPanel::AddChild(m_pHelpControlSprite[ 1 ]);

	m_pHelpControlSprite[ 2 ] = new CRYLSpriteEx(m_pGMUIParentDlg);
	m_pHelpControlSprite[ 2 ]->m_pSprite = new CRYLSprite;
	m_pHelpControlSprite[ 2 ]->m_pTexture = new CTexture;
	m_pHelpControlSprite[ 2 ]->SetLeft(0);
	m_pHelpControlSprite[ 2 ]->SetTop(256);
	m_pHelpControlSprite[ 2 ]->SetWidth(256);
	m_pHelpControlSprite[ 2 ]->SetHeight(256);
	CGMUICustomPanel::AddChild(m_pHelpControlSprite[ 2 ]);

	m_pHelpControlSprite[ 3 ] = new CRYLSpriteEx(m_pGMUIParentDlg);
	m_pHelpControlSprite[ 3 ]->m_pSprite = new CRYLSprite;
	m_pHelpControlSprite[ 3 ]->m_pTexture = new CTexture;
	m_pHelpControlSprite[ 3 ]->SetLeft(256);
	m_pHelpControlSprite[ 3 ]->SetTop(256);
	m_pHelpControlSprite[ 3 ]->SetWidth(256);
	m_pHelpControlSprite[ 3 ]->SetHeight(256);
	CGMUICustomPanel::AddChild(m_pHelpControlSprite[ 3 ]);
}

void	CRYLHelpControlDlg::FinalCtrl(void)
{
	for (int i = 0; i < 4; ++i)
	{
		GM_DELETE(m_pHelpControlSprite[ i ]);
	}
}

VOID	CRYLHelpControlDlg::Show(CGMUIObject* Sender)
{	
}	

VOID	CRYLHelpControlDlg::Hide(CGMUIObject* Sender)
{
}

VOID	CRYLHelpControlDlg::GMUIPaint(INT xx, INT yy)
{
	CGMUICustomPanel::GMUIPaint( xx, yy ) ;
}

VOID	CRYLHelpControlDlg::DlgClick(CGMUIObject* Sender, INT X, INT Y)
{
}

void	CRYLHelpControlDlg::LoadHelpControlTexture(void)
{
	CTexture::SetPath(INTERFACETEXTUREPATH);

	CHAR strFile[ MAX_PATH ];

	for (unsigned long i = 0; i < 4; i ++)
	{
		if (NULL != m_pHelpControlSprite[ i ]->m_pTexture->GetTexture()) 
		{
			m_pHelpControlSprite[ i ]->m_pTexture->Unload();
		}

		sprintf(strFile, "Help\\Help%02d.dds", i + 1);
		m_pHelpControlSprite[ i ]->m_pTexture->Load(strFile);
		m_pHelpControlSprite[ i ]->m_pSprite->Create(0, 0, 0, 0, 256, 256, m_pHelpControlSprite[ i ]->m_pTexture);
	}
}

