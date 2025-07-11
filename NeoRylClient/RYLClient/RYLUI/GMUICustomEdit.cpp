//------------------------------------------------------------------------------
//					CD2Edit : public CGMUIObject
//------------------------------------------------------------------------------
#include "GMUICustomEdit.h"
#include "GMUICustomDlg.h"
#include "GMUIScreen.h"
#include "GMUIUtil.h"

//-- Zergra From.--//
#include "IME/ImeConstant.h"
#include "IME/GMIMEPACK.h"
//#include "GMUICustomIMEView.h"
#include "GMMemory.h"
//-- Zergra To. --//

//-- Zergra From.--//
const UINT  LINEDELAY = 500;
const UINT  CARETBLINKINTERVAL = 500;
//-- Zergra To. --//

CGMUICustomEdit::CGMUICustomEdit( CGMUICustomDlg *pParentDlg ) : CGMUIObject()
{
    m_pGMUIParentDlg = pParentDlg ;
    m_PassWord		 = '\0' ;

    m_iMerginX		 = 2 ;

    CGMUIObject::SetWidth( 80 ) ;
    CGMUIObject::SetHeight( 16 ) ;
    CGMUIObject::SetLeft( 0 ) ;
    CGMUIObject::SetTop( 0 ) ;

    m_bFocus		     = FALSE ;
    m_iMaxLength	     = m_iWidth ;

    m_bDblClick			 = FALSE ;

    OnChanageEvent		 = NULL ;
    OnEnterFocus		 = NULL ;
    OnExitFocus			 = NULL ;

    OnDrawEvent			 = NULL ;
    OnDrawCharEvent		 = NULL ;
    OnUnderLineDraw		 = NULL ;
    OnSelectLineDraw	 = NULL ;

    //-- Zergra From.--//
    m_pImePack           = new GMIMEPACK;
    m_strText            = const_cast<LPSTR>( m_pImePack->GetString() );

    m_bIsEnglishOnly     = false;
    m_bIsNumeralOnly     = FALSE;
    m_bIsMoneyNumber     = FALSE;

    m_nOverWidthLength  = 0;

    m_strCurrentString.clear();
    m_unCurrentStartPos = 0;
    m_unCurrentEndPos   = 0;
    //-- Zergra To. --//

    CGMUIObject::SetGMUICtrlType( GMUICtrlEDITBOX ) ;
}

CGMUICustomEdit::~CGMUICustomEdit()
{	
    //-- Zergra From.--//
    GM_DELETE( m_pImePack );
    //-- Zergra To. --//

    OnDrawEvent			= NULL ;
    OnDrawCharEvent		= NULL ;
    OnUnderLineDraw		= NULL ;
    OnSelectLineDraw	= NULL ;
}

VOID CGMUICustomEdit::SetFocus()
{
    if( CGMUIScreen::IsInited() )
    {
        CGMUIScreen::GetInstance()->SetCurFocusEdit( this ) ;

        //-- 2004. 1. 19. Zergra From.--//
        if( m_bIsNumeralOnly )
        {
            m_pImePack->SetNumeralOnly( true );
        }
        else
        {
            m_pImePack->SetNumeralOnly( false );
        }

        //-- 2004. 11. 10. --//
        if( m_bIsEnglishOnly )
        {
            m_pImePack->SetEnglishOnly( true );
        }
        //-- Zergra To. --//      
    }
}

VOID CGMUICustomEdit::SetFocusState( BOOL Value )
{
    m_bFocus = Value ;

    if( m_bFocus )
    {
        EnterFocus( this ) ;
    }
    else
    {
        ExitFocus( this ) ;
    }
}

BOOL CGMUICustomEdit::GetFocusState()
{
    return m_bFocus;
}

INT		GetValue( INT A, INT B )
{
    if(A > B)
    {
        return A-B;
    }
    if(B > A)
    {
        return B-A;
    }
    return 0;
}

BOOL CGMUICustomEdit::GetMaxLengthState()
{
    return m_pImePack->GetStringLimit();
}

VOID CGMUICustomEdit::SetText( LPSTR Text )
{
    m_pImePack->SetString( static_cast<LPSTR>( Text ) );
}

LPSTR	CGMUICustomEdit::GetText()
{
    return m_strText = const_cast<LPSTR>( m_pImePack->GetString() );
}

VOID CGMUICustomEdit::SetMaxLength( INT aLength )
{
    m_iMaxLength = aLength ;

    m_pImePack->SetStringLimit( aLength );
}

INT CGMUICustomEdit::GetMaxLength()
{
    return m_pImePack->GetStringLimit();
}

VOID CGMUICustomEdit::Click (CGMUIObject* Sender, INT X, INT Y)
{
    if( !CGMUIObject::GetVisible() ) {return;}
    if( !CGMUIObject::GetEnable() ) {return;}
    CGMUIObject::Click (this, X, Y);
    SetFocus();
}

VOID CGMUICustomEdit::DblClick (CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y)
{
    if( !CGMUIObject::GetVisible() ) 
    {
        return;
    }

    if( !CGMUIObject::GetEnable() ) 
    {
        return;
    }

    m_bDblClick = TRUE;

    CGMUIObject::DblClick( this, Button, Shift, X, Y );
}

VOID CGMUICustomEdit::MouseDown(CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y)
{
    if( !CGMUIObject::GetVisible() ) 
    {
        return;
    }

    if( !CGMUIObject::GetEnable() )
    {
        return;
    }

    if( m_bDblClick ) // DblClick시 all Select 처리
    {
        m_bDblClick = FALSE;
    }

    CGMUIObject::MouseDown( this, Button, Shift, X, Y );
}

VOID CGMUICustomEdit::MouseMove( CGMUIObject* Sender, INT Shift, INT X, INT Y )
{
    if( !CGMUIObject::GetVisible() ) 
    {
        return;
    }

    if( !CGMUIObject::GetEnable() ) 
    {
        return;
    }

    CGMUIObject::MouseMove( this, Shift, X, Y );
}

VOID CGMUICustomEdit::MouseUp(CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y)
{
    if( !CGMUIObject::GetVisible() )
    {
        return;
    }

    if( !CGMUIObject::GetEnable() ) 
    {
        return;
    }

    CGMUIObject::MouseUp(this, Button, Shift, X, Y);
}

VOID CGMUICustomEdit::KeyDown(CGMUIObject* Sender, WORD key, INT Shift)
{
    if( !CGMUIObject::GetVisible() ) 
    {
        return;
    }

    if( !CGMUIObject::GetEnable() ) 
    {
        return;
    }

    CGMUIObject::KeyDown (this, key, Shift);
}

VOID CGMUICustomEdit::KeyUp(CGMUIObject* Sender, WORD key, INT Shift)
{
    if( !CGMUIObject::GetVisible() ) {return;}
    if( !CGMUIObject::GetEnable() ) {return;}
    CGMUIObject::KeyUp (this, key, Shift);
}

VOID CGMUICustomEdit::EnterFocus(CGMUIObject* Sender)
{
    if(OnEnterFocus != NULL)
    {
        OnEnterFocus(this);
    }
}

VOID CGMUICustomEdit::SetPassWord( const CHAR pchar)
{
    m_PassWord = pchar;
}

BOOL CGMUICustomEdit::GetPassWord()
{
    if(m_PassWord != 0) 
        return TRUE;

    return FALSE;
}

VOID CGMUICustomEdit::ExitFocus(CGMUIObject* Sender)
{
    if(OnExitFocus != NULL)
    {
        OnExitFocus (this);
    }
}

VOID CGMUICustomEdit::DoDraw(CGMUIObject* Sender, RECT r)
{
    if(OnDrawEvent != NULL)
    {
        OnDrawEvent (Sender, r);
    }
}

VOID CGMUICustomEdit::DoCharDraw(CGMUIObject* Sender, RECT r, LPSTR str)
{
    if(OnDrawCharEvent != NULL)
    {
        OnDrawCharEvent (Sender, r, str);
    }
}

VOID CGMUICustomEdit::DoUnderLineDraw(CGMUIObject* Sender, RECT r)
{
    if(OnUnderLineDraw != NULL)
    {
        OnUnderLineDraw (Sender, r);
    }
}

VOID CGMUICustomEdit::DoSelectLineDraw(CGMUIObject* Sender, RECT r)
{
    if(OnSelectLineDraw != NULL)
    {
        OnSelectLineDraw (Sender, r);
    }
}



VOID CGMUICustomEdit::SetSize (CGMUIObject* Sender)
{
    CGMUIObject::SetSize (Sender);
    m_iMaxLength = m_iWidth;
}

VOID CGMUICustomEdit::GMUIPaint( INT xx, INT yy )
{
    if( !CGMUIObject::GetVisible() ) {return;}
    RECT r;
    SetRect (&r, xx, yy, xx + CGMUIObject::GetWidth (), yy + CGMUIObject::GetHeight () );

    // draw Texture
    DoDraw (this, r);

    // draw Selection Region
    DrawSelectRegion( xx, yy );

    // draw Caret
    if( m_bFocus )
    {
        static   ULONG ulTick = GetTickCount();

        ULONG ulCurrentTick = GetTickCount();

        if( ulCurrentTick < ulTick + CARETBLINKINTERVAL )
        {
            DrawCaret( xx, yy );
        }
        else if( ulCurrentTick > ulTick + CARETBLINKINTERVAL * 2 )
        {
            ulTick = ulCurrentTick;
        }
    }

    // draw String
    DrawString( xx, yy );

    CGMUIObject::GMUIPaint( xx, yy );
}

//-- Zergra From.--//
//-----------------------------------------------------------------------------------------------------
// CGMUICustomEdit::DrawCaret()
VOID CGMUICustomEdit::DrawCaret(INT xx, INT yy)
{
	UINT    unCaretPos  = m_pImePack->GetCaretPos();

	if ( unCaretPos < (UINT)m_iMaxLength )
	{
		string  strTemp1    = m_pImePack->GetString();
		string  strTemp2    = m_pImePack->GetSubString( 0, unCaretPos );

		if(unCaretPos>strTemp1.length())
		{
			unCaretPos = strTemp1.length();
		}

		string  strTemp3    = m_pImePack->GetSubString( unCaretPos, strTemp1.length() );
		int     nCaretPixel = 0;
		int     nRemainPixel= 0;

		if( m_PassWord != 0 )
		{
			string pchPassword( strTemp2.length(), m_PassWord );
			string pchRemainPassword( strTemp3.length(), m_PassWord );
			nCaretPixel     = GetGMUITextWidth( const_cast<char*>( pchPassword.c_str() ) );
			nRemainPixel    = GetGMUITextWidth( const_cast<char*>( pchRemainPassword.c_str() ) );
		}
		else
		{
			nCaretPixel     = GetGMUITextWidth( const_cast<char*>( strTemp2.c_str() ) );
			nRemainPixel    = GetGMUITextWidth( const_cast<char*>( strTemp3.c_str() ) );
		}

		UINT     unCaretWidth   = 1;
		UINT     unCaretHeight  = CGMUIObject::GetHeight() - 2;

		RECT     rtCaret;

		if( nCaretPixel + m_iMerginX < m_iWidth )
		{
			rtCaret.left    = xx + m_iMerginX + nCaretPixel;
			rtCaret.top     = yy + 1;
			rtCaret.right   = xx + m_iMerginX + nCaretPixel + unCaretWidth;
			rtCaret.bottom  = yy + unCaretHeight;
		}
		else
		{
			rtCaret.left    = xx + m_iMerginX + nCaretPixel - m_nOverWidthLength + nRemainPixel;
			rtCaret.top     = yy + 1;
			rtCaret.right   = xx + m_iMerginX + nCaretPixel + unCaretWidth - m_nOverWidthLength + nRemainPixel;
			rtCaret.bottom  = yy + unCaretHeight;
		}
	
		DoUnderLineDraw( this, rtCaret );      
	}
}


//-----------------------------------------------------------------------------------------------------
// CGMUICustomEdit::DrawSelectRegion()
VOID CGMUICustomEdit::DrawSelectRegion(INT xx, INT yy)
{
    if( m_pImePack->GetTextSelecting() )
    {
        RECT  rtSelectRegion;

        int     nCaretPos   = m_pImePack->GetCaretPos();
        string  strTemp     = m_pImePack->GetSubString( 0, nCaretPos );
        int     nCaretPixel = GetGMUITextWidth( const_cast<char*>( strTemp.c_str() ) );

        if( nCaretPixel + m_iMerginX < m_iWidth )
        {
            RECT  rtSelectRegion;

            string  strTemp1    = m_pImePack->GetSubString( 0, m_pImePack->GetSelectStartPos() );
            string  strTemp2    = m_pImePack->GetSubString( 0, m_pImePack->GetSelectEndPos() );

            int nTemp1          = GetGMUITextWidth( const_cast<char*>( strTemp1.c_str() ) );
            int nTemp2          = GetGMUITextWidth( const_cast<char*>( strTemp2.c_str() ) );
            int nCurrent        = GetGMUITextWidth( const_cast<char*>( m_strCurrentString.c_str() ) );

            if( nTemp2 > nCurrent )
                nTemp2 = nCurrent;
                
            SetRect( &rtSelectRegion, xx + m_iMerginX + nTemp1, yy, xx + m_iMerginX + nTemp2, yy + CGMUIObject::GetHeight() );

            DoSelectLineDraw( this, rtSelectRegion );
        }
        else
        {
            string  strFront    = m_pImePack->GetSubString( 0, m_unCurrentStartPos );
            string  strTemp1    = m_pImePack->GetSubString( 0, m_pImePack->GetSelectStartPos() );
            string  strTemp2    = m_pImePack->GetSubString( 0, m_pImePack->GetSelectEndPos() );

            int nFront          = GetGMUITextWidth( const_cast<char*>( strFront.c_str() ) );
            int nTemp1          = GetGMUITextWidth( const_cast<char*>( strTemp1.c_str() ) );
            int nTemp2          = GetGMUITextWidth( const_cast<char*>( strTemp2.c_str() ) );
            int nCurrent        = GetGMUITextWidth( const_cast<char*>( m_strCurrentString.c_str() ) );

            int nPosLeft = nTemp1 - nFront;
            if( nPosLeft < 0 )
                nPosLeft = 0;

            int nPosRight = nTemp2 - nFront;
            if( nPosRight > nCurrent )
                nPosRight = nCurrent;
                
            SetRect( &rtSelectRegion, xx + m_iMerginX + nPosLeft, yy, xx + m_iMerginX + nPosRight, yy + CGMUIObject::GetHeight() );

            DoSelectLineDraw( this, rtSelectRegion );
        }
    }
}

//-----------------------------------------------------------------------------------------------------
// CGMUICustomEdit::DrawString()
VOID CGMUICustomEdit::DrawString( INT xx, INT yy )
{
    RECT     rtRegion;
    string   strPassword;

    m_strText   = const_cast<LPSTR>( m_pImePack->GetString() );

    string strTemp;

    if( m_bIsMoneyNumber )
    {
        strTemp = m_strText;

        if( strTemp.length() > 3 )
        {
            for( int i = strTemp.length() - 3 ; i > 0 ; i -= 3 )
            {
                strTemp.insert( i, "," );
            }

            m_strText = const_cast<char*>( strTemp.c_str() );
        }
    }

    int nWidth  = GetGMUITextWidth( m_strText );
    int nHeight = GetGMUITextHeight( m_strText );
    int nGap    = ( CGMUIObject::GetHeight() - nHeight ) / 2;

    if( nWidth < m_iWidth )
    {
        SetRect( &rtRegion, xx + m_iMerginX, yy + nGap, xx + m_iMerginX + nWidth, yy + nHeight + nGap );
    }
    else
    {
        SetRect( &rtRegion, xx + m_iMerginX, yy + nGap, xx + m_iMerginX + m_iWidth, yy + nHeight + nGap );
    }

    if( !m_strText )
        return;

    m_nOverWidthLength = 0;

    if( m_PassWord != 0 )
    {
        string pchPassword( m_pImePack->GetStringLength(), m_PassWord );

        if( !pchPassword.empty() )
        {
            DoCharDraw( this, rtRegion, const_cast<LPSTR>( pchPassword.c_str() ) );
        }
        else
        {
            DoCharDraw( this, rtRegion, "" );
        }
    }
    else
    {
        if( nWidth < m_iWidth )
        {         
            m_strCurrentString = m_strText;

            DoCharDraw( this, rtRegion, m_strText );
        }
        else
        {
            // 입력된 텍스트가 에디트박스보다 길 때
            int     nCaretPos   = m_pImePack->GetCaretPos();
            string  strTemp     = m_pImePack->GetSubString( 0, nCaretPos );
            int     nCaretPixel = GetGMUITextWidth( const_cast<char*>( strTemp.c_str() ) );
            string  strOutput;
            string  strComposingString = m_pImePack->GetCurrentComposingString();

            if( nCaretPixel + m_iMerginX < m_iWidth )
            {
                strOutput = GetStringByWidth( 0, false );
            }
            else
            {
                strOutput = GetStringByWidth( nCaretPos + strComposingString.length(), true );
            }

            m_strCurrentString = strOutput.c_str();

            DoCharDraw( this, rtRegion, const_cast<LPSTR>( strOutput.c_str() ));
        }
    }
}

//-----------------------------------------------------------------------------------------------------
// CGMUICustomEdit::GetIMEMessage()
bool  CGMUICustomEdit::GetIMEMessage( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    //-- 2004. 1. 29. Zergra From.--//
    CGMUICustomEdit*    pEdit = NULL;
    CGMUIScreen::GetInstance()->GetCurFocusEdit( &pEdit );
    if( !pEdit->GetFocusState() )
        return FALSE;
    //-- Zergra To. --//

    if( m_pImePack )
    {
        if( m_pImePack->GetIMEMessage( hWnd, message, wParam, lParam ) )
        {
            // if there exist remained process after ime message processed, to do here.
            // -- CAUTION!!! --
            // if there's no special reason, do not process the default window procedure after this statements processed
            // it concerned with drawing default composition and candidate window of WINDOWS
            if( m_pImePack->GetStringLength() )
                m_strText = const_cast<LPSTR>( m_pImePack->GetString() );

            return TRUE;
        }
    }

    return FALSE;
}

//-----------------------------------------------------------------------------------------------------
// CGMUICustomEdit::SetNumeralOnly()
VOID CGMUICustomEdit::SetNumeralOnly( BOOL bOnly )
{
    m_bIsNumeralOnly = bOnly;

    if( m_pImePack )
    {
        m_pImePack->SetNumeralOnly( static_cast<bool>( bOnly != FALSE ) );
    }
}

//-----------------------------------------------------------------------------------------------------
// CGMUICustomEdit::SetEnglishOnly()
VOID CGMUICustomEdit::SetEnglishOnly( bool bOnly )
{
    m_bIsEnglishOnly = bOnly;

    if( m_pImePack )
    {
        m_pImePack->SetEnglishOnly( static_cast<bool>( bOnly != FALSE ) );
    }
}

//-----------------------------------------------------------------------------------------------------
// CGMUICustomEdit::GetNumeralOnly()
BOOL CGMUICustomEdit::GetNumeralOnly()
{
    return m_bIsNumeralOnly;
}

//--------------------------------------------------------------------------------------------------------------------------
// CGMUICustomEdit::GetStringByWidth()
string CGMUICustomEdit::GetStringByWidth( unsigned int unPos, bool bFront )
{
    string  strInputText = m_pImePack->GetString();
    string  strTemp1;
    string  strTemp2;
    int     nShift = 0;
    int     nShiftCount = 0;
    int     nCurrentWidth = 0;

    strTemp1.clear();
    strTemp2.clear();

    if( bFront )
    {
        m_unCurrentEndPos   = unPos;

        while( m_iWidth > nCurrentWidth + m_iMerginX )
        {
            nShiftCount += nShift;
            strTemp2 = strTemp1;

            LPSTR buf = CharPrev( &strInputText[ 0 ], &strInputText[ unPos ] );

            nShift = &strInputText[ unPos ] - buf;

            if( nShift == 0 )
                break;

            strTemp1.insert( strTemp1.begin(), strInputText.begin() + unPos - nShift, strInputText.begin() + unPos );

            unPos -= nShift;

            nCurrentWidth = GetGMUITextWidth( const_cast<char*>( strTemp1.c_str() ) );
        }

        m_unCurrentStartPos = m_unCurrentEndPos - nShiftCount;

        nCurrentWidth = GetGMUITextWidth( const_cast<char*>( strTemp2.c_str() ) );
        m_nOverWidthLength = GetGMUITextWidth( const_cast<char*>( strInputText.c_str() ) ) - nCurrentWidth;
    }
    else
    {
        m_unCurrentStartPos = 0;

        while( m_iWidth > nCurrentWidth + m_iMerginX )
        {
            strTemp2 = strTemp1;

            LPSTR buf = CharNext( &strInputText[ unPos ] );

            nShift = buf - &strInputText[ unPos ];

            if( nShift == 0 )
                break;

            strTemp1.insert( strTemp1.begin() + strTemp1.length(), strInputText.begin() + unPos, strInputText.begin() + unPos + nShift );

            unPos += nShift;

            nCurrentWidth = GetGMUITextWidth( const_cast<char*>( strTemp1.c_str() ) );
        }

        nCurrentWidth = GetGMUITextWidth( const_cast<char*>( strTemp2.c_str() ) );
        m_nOverWidthLength = GetGMUITextWidth( const_cast<char*>( strInputText.c_str() ) ) - nCurrentWidth;
    }

    return strTemp2;
}

//-- Zergra To. --//