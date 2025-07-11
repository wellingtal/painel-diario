//================================================================
//
//	Name : GMUICustomEdit.h 
//	Desc : Gama Soft Custom EditBox Class
//  Date : 2003. 09. 17
//
//================================================================
#ifndef __GMUICUSTOMEDIT_H__
#define __GMUICUSTOMEDIT_H__

#include "GMUIObject.h"
#include <string>
using namespace std;

class CGMUICustomForm;

//-- Zergra From.--//
class GMIMEPACK;
//-- Zergra To. --//

class CGMUICustomEdit : public CGMUIObject
{
private:
                BOOL        m_bFocus;
	            CHAR		m_PassWord;
	            LPSTR		m_strText;
	            INT		    m_iMaxLength;
	            BOOL		m_bDblClick;

                INT		    m_iMerginX;

      //-- Zergra From.--//
protected:
               GMIMEPACK*   m_pImePack;
               BOOL         m_bIsNumeralOnly;

               BOOL         m_bIsMoneyNumber;
               bool         m_bIsEnglishOnly;

               int          m_nOverWidthLength;

               string       m_strCurrentString;
            unsigned int    m_unCurrentStartPos;
            unsigned int    m_unCurrentEndPos;
      //-- Zergra To. --//

public :
	      OnNotifyEvent	    OnChanageEvent;
	      OnNotifyEvent	    OnEnterFocus;
	      OnNotifyEvent	    OnExitFocus;

	        OnDraw			OnDrawEvent;
	        OnDrawChar		OnDrawCharEvent;
	        OnDraw			OnUnderLineDraw;
	        OnDraw			OnSelectLineDraw;

	                        CGMUICustomEdit( CGMUICustomDlg *pParentDlg );
	   virtual              ~CGMUICustomEdit();

	            VOID        SetFocus();
	            VOID	    SetFocusState( BOOL Value );
	            BOOL	    GetFocusState();

	            VOID	    SetText( LPSTR Text );
	            LPSTR	    GetText();

	            VOID	    GetSelText( CHAR** Text );

	            VOID	    SetMaxLength( INT aLength );
	            INT	        GetMaxLength();

	            BOOL	    GetMaxLengthState();

	            VOID	    SetPassWord( const CHAR pchar );
	            BOOL	    GetPassWord();

                //-- Zergra From.--//
public:
                VOID		DrawCaret( INT xx, INT yy );
	            VOID		DrawSelectRegion( INT xx, INT yy );
	            VOID		DrawString( INT xx, INT yy );

                //-----------------------------------------------------------------------------------------------------
                // return GMIMEPACK pointer
      const     GMIMEPACK*  GetImePack()                                         { return m_pImePack; }

                //-----------------------------------------------------------------------------------------------------
                // ¡ØCAUTION¡Ø   this is the KEY( most importamnt ) function!!!!!!!
                bool        GetIMEMessage( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

                //-- 2004. 1. 19. Zergra --//
                VOID        SetNumeralOnly( BOOL bOnly );
                BOOL        GetNumeralOnly();

                //-- 2004. 1. 30. --//
                VOID        SetIsMoneyNumber( BOOL bMoney )                     { m_bIsMoneyNumber = bMoney; }
                BOOL        GetISMoneyNumber()                                  { return m_bIsMoneyNumber; }

                //-- 2004. 11. 10. --//
                VOID        SetEnglishOnly( bool bOnly );
                bool        GetEnglishOnly()                                    { return m_bIsEnglishOnly; }

private:
                string      GetStringByWidth( unsigned int unPos, bool bFront );
                //-- Zergra To. --//

public:
	   virtual  VOID	    Click( CGMUIObject* Sender, INT X, INT Y );
	   virtual  VOID	    DblClick( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y );
	   virtual  VOID	    MouseDown( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y );
	   virtual  VOID	    MouseMove( CGMUIObject* Sender, INT Shift, INT X, INT Y );
	   virtual  VOID	    MouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y );

	   virtual  VOID	    KeyDown( CGMUIObject* Sender, WORD key, INT Shift );
	   virtual  VOID	    KeyUp( CGMUIObject* Sender, WORD key, INT Shift );

	   virtual  VOID	    DoDraw( CGMUIObject* Sender, RECT r );
	   virtual  VOID	    DoCharDraw( CGMUIObject* Sender, RECT r, LPSTR str );
	   virtual  VOID	    DoUnderLineDraw( CGMUIObject* Sender, RECT r );
	   virtual  VOID	    DoSelectLineDraw( CGMUIObject* Sender, RECT r );

	   virtual  VOID	    EnterFocus( CGMUIObject* Sender );
	   virtual  VOID	    ExitFocus( CGMUIObject* Sender );

	   virtual  VOID	    SetSize( CGMUIObject* Sender );
	   virtual  VOID	    GMUIPaint( INT xx, INT yy );
};

#endif // __GMUICUSTOMEDIT_H__

