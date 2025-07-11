//================================================================
//
//	Name : GMUIMain.h 
//	Desc : GMUI Initialize & Finalize
//  Date : 2003. 09. 22
//
//================================================================
#ifndef __GMUIMAIN_H__
#define __GMUIMAIN_H__


#include "GMUIScreen.h"
#include "GMFont.h"
#include "RYLTextureManager.h"
#include "RYLRectangle.h"
#include "RYLImageManager.h"

#include "RYLDialog.h"
#include "RYLButton.h"
//#include "RYLCheckBox.h"
//#include "RYLComboBox.h"
//#include "RYLEdit.h"
//#include "RYLImeView.h"
//#include "RYLLabel.h"
//#include "RYLListBox.h"
//#include "RYLPicture.h"

HRESULT GMUIInitialize( HWND hWnd, INT ScreenWidth, INT ScreenHeight ) ;
HRESULT GMUIFinalize() ;


#endif