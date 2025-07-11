//-----------------------------------------------------------------------------------------------------
// File Name: GMIMEPACK.cpp
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// Date: 2003. 12. 23. ( TUE )
//-----------------------------------------------------------------------------------------------------
#include <winsock2.h>
#include <windows.h>

#include "RYLMessageBox.h"
#include "RYLStringTable.h"

#include "ImeConstant.h"
#include "GMIMEPACK.h"
#include "RYLNetworkData.h"
#include "GMMemory.h"

//-----------------------------------------------------------------------------------------------------
// Support Local Functions
//-----------------------------------------------------------------------------------------------------
bool CheckAlphabet( int c )
{
	if( ( ( c >= 'A' ) && ( c <= 'Z' ) )
		|| ( ( c >= 'a' ) && ( c <= 'z' ) ) )
	{
		return true;
	}

	return false;
}

bool CheckDigit( int c )
{
	if( ( c >= '0' ) && ( c <= '9' ) )
	{
		return true;
	}

	return false;
}

bool CheckSpecialLetter( int c )
{
	if( ( ( c >= 33 )  && ( c <= 47 ) ) 
		|| ( ( c >= 58 )  && ( c <= 64 ) ) 
		|| ( ( c >= 91 )  && ( c <= 96 ) ) 
		|| ( ( c >= 123 ) && ( c <= 126 ) )
		|| ( ( c >= 128 ) && ( c <= 254 ) ) )
	{
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
// class GMIMEPACK
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
string GMIMEPACK::m_strCopyString;

//-----------------------------------------------------------------------------------------------------
// constructor, destructor
//-----------------------------------------------------------------------------------------------------
GMIMEPACK::GMIMEPACK()
: m_hWnd(NULL), m_hImc(NULL), m_unStringLimit(0), m_unCaretPos(0), m_unWritePos(0), m_bInsert(true), 
m_bFullWidth(false), m_bEnglishOnly(false), m_bNumeralOnly(false), m_bHalfWidthOnly(false), m_lpCompositionForm(NULL), m_bUseCustomImeWindow( true ),
m_unSelectStartPos(0), m_unSelectEndPos(0), m_bShiftPushed(false), m_bCtrlPushed(false), m_bTextSelecting(false),
m_unCandidatePageStartIdx(0), m_unCandidateOnceRetrieveCount(9), m_unCandidateCount(0), m_unCandidateSelectedIdx(0), m_unCandidateCurrentSelectedIdx(0), m_lpCandidateForm(NULL)
{
	ClearString();
}

GMIMEPACK::GMIMEPACK( HWND hWnd, UINT unStringLimit )
: m_hWnd(hWnd), m_hImc(NULL), m_unStringLimit(unStringLimit), m_unCaretPos(0), m_unWritePos(0), m_bInsert(true), 
m_bFullWidth(false), m_bEnglishOnly(false), m_bNumeralOnly(false), m_bHalfWidthOnly(false), m_lpCompositionForm(NULL), m_bUseCustomImeWindow( true ),
m_unSelectStartPos(0), m_unSelectEndPos(0), m_bShiftPushed(false), m_bCtrlPushed(false), m_bTextSelecting(false),
m_unCandidatePageStartIdx(0), m_unCandidateOnceRetrieveCount(9), m_unCandidateCount(0), m_unCandidateSelectedIdx(0), m_unCandidateCurrentSelectedIdx(0), m_lpCandidateForm(NULL)
{
	InitIme( hWnd, unStringLimit );
}

GMIMEPACK::~GMIMEPACK()
{
	ClearString();

	if( m_lpCompositionForm )
	{
		delete m_lpCompositionForm;
		m_lpCompositionForm = NULL;
	}

	if( m_lpCandidateForm )
	{
		delete m_lpCandidateForm;
		m_lpCandidateForm = NULL;
	}
}

//-----------------------------------------------------------------------------------------------------
// member functions
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
// GMIMEPACK::InitIme()
void GMIMEPACK::InitIme( HWND hMainWnd, UINT unStringLimit )
{
	m_hWnd            = hMainWnd;
	m_unStringLimit   = unStringLimit;

	m_lpCompositionForm  = new COMPOSITIONFORM;
	ZeroMemory( m_lpCompositionForm, sizeof( COMPOSITIONFORM ) );
	m_lpCandidateForm    = new CANDIDATEFORM;
	ZeroMemory( m_lpCandidateForm, sizeof( CANDIDATEFORM ) );

	SetStringLimit( unStringLimit );
	m_vtCandidateList.reserve( m_unCandidateOnceRetrieveCount );   // '9' is basic windows default retrieve count
	m_vtCandidateList.clear();

	// 태국일때는 LID_THAI넣는다.
	if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::THAILAND )
		OnInputLangChange( 0, ZIme::LID_THAI );                    // check current keyboard layout	
	else
		OnInputLangChange( 0, 0 );                                 // check current keyboard layout
}

//-----------------------------------------------------------------------------------------------------
// GMIMEPACK::GetIMEMessage()
bool GMIMEPACK::GetIMEMessage( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	// set window handle
	if( !m_hWnd && hWnd )
	{
		InitIme( hWnd, m_unStringLimit ? m_unStringLimit : 64 );
	}

	if( !m_hWnd )
		return false;

	switch( msg )
	{
	case WM_KEYDOWN:
		{
			return OnKeyDown( wParam, lParam );
		}
		break;

	case WM_KEYUP:
		{
			switch( wParam )
			{
			case VK_SHIFT:
				{
					m_bShiftPushed = false;
				}
				break;

			case VK_CONTROL:
				{
					m_bCtrlPushed = false;
				}
				break;
			}
		}
		break;

	case WM_CHAR:
		{
			return OnChar( wParam, lParam );
		}
		break;

	case WM_INPUTLANGCHANGE:
		{
			return OnInputLangChange( wParam, lParam );
		}
		break;

	case WM_IME_STARTCOMPOSITION:
		{
            // 컴포지션 윈도우에서 작업 시작할 때
			return OnImeStartComposition( wParam, lParam );
		}
		break;

	case WM_IME_COMPOSITION:
		{
            // 컴포지션 윈도우 관련 작업
			return OnImeComposition( wParam, lParam );
		}
		break;

	case WM_IME_ENDCOMPOSITION:
		{
            // 컴포지션 끝날 때
			return OnImeEndComposition( wParam, lParam );
		}
		break;

	case WM_IME_NOTIFY:
		{
			return OnImeNotify( wParam, lParam );
		}
		break;
	}

	return false;
}

//-----------------------------------------------------------------------------------------------------
// GMIMEPACK::OpenIme()
void GMIMEPACK::OpenIme()
{
	m_hImc			= ImmGetContext( m_hWnd );

	ImmSetOpenStatus( m_hImc, true );


/*	전각 / 반각 전환할려고 해둔 임시 코드
	DWORD dwConv, dwSent ;
	::ImmGetConversionStatus( m_hImc, &dwConv, &dwSent ) ;
	//dwConv = IME_CMODE_FULLSHAPE ;
	dwConv &= ~IME_CMODE_FULLSHAPE; //반각

	::ImmSetConversionStatus( m_hImc, dwConv, dwSent ) ;
*/
	
	ImmNotifyIME( m_hImc, NI_CLOSECANDIDATE, 0, 0 );

	HWND hImeWnd = ImmGetDefaultIMEWnd( m_hWnd );
	SendMessage( hImeWnd, WM_IME_CONTROL, IMC_CLOSESTATUSWINDOW, 0 );

	ImmReleaseContext( m_hWnd, m_hImc );
}

//-----------------------------------------------------------------------------------------------------
// GMIMEPACK::CloseIme()
void GMIMEPACK::CloseIme()
{
	m_hImc			= ImmGetContext( m_hWnd );

	ImmSetOpenStatus( m_hImc, false );

	ImmReleaseContext( m_hWnd, m_hImc );
}

//-----------------------------------------------------------------------------------------------------
// GMIMEPACK::OnKeyDown()
bool GMIMEPACK::OnKeyDown( WPARAM wParam, LPARAM lParam )
{
	// process toggle insert state, backsapce, delete, home, end, copy, taste, and so on.
	switch( wParam )
	{
	case VK_ESCAPE:
		{
			if( m_bTextSelecting )
				m_unSelectStartPos = m_unSelectEndPos = m_unCaretPos = m_unWritePos;
		}
		break;

	case VK_SHIFT:
		{
			//if( m_strCurrentComposingString.length() )
			//{
			//   // get ime handle
			//   m_hImc			= ImmGetContext( m_hWnd );

			//   ImmNotifyIME( m_hImc, NI_COMPOSITIONSTR, CPS_COMPLETE, 0 );

			//   ImmReleaseContext( m_hWnd, m_hImc );
			//}

			if( !m_bTextSelecting )
				m_unSelectStartPos = m_unSelectEndPos = m_unWritePos;

			m_bShiftPushed = true;
		}
		break;

	case VK_CONTROL:
		{
			m_bCtrlPushed = true;
		}
		break;

	case VK_HOME:
		{
			m_unCaretPos = 0;

			// when shift key was pushed, select string
			if( m_bShiftPushed )
			{
				if( m_unSelectStartPos == m_unWritePos && m_unSelectEndPos == m_unWritePos )
				{
					m_unSelectStartPos = m_unCaretPos;
				}
				else if( m_unSelectStartPos != m_unWritePos && m_unSelectEndPos == m_unWritePos )
				{
					m_unSelectStartPos = m_unCaretPos;
				}
				else if( m_unSelectStartPos == m_unWritePos && m_unSelectEndPos != m_unWritePos )
				{
					m_unSelectEndPos = m_unSelectStartPos;
					m_unSelectStartPos = m_unCaretPos;
				}

				// selection region vaildness check
				if( m_unSelectStartPos != m_unSelectEndPos )
				{
					m_bTextSelecting = true;
				}
				else
				{
					m_bTextSelecting = false;
				}
			}
			else
			{
				m_bTextSelecting = false;

				m_unSelectStartPos = m_unSelectEndPos = m_unCaretPos;

				m_unWritePos = m_unCaretPos;
			}
		}
		break;

	case VK_END:
		{
			m_unCaretPos   = static_cast<UINT>( m_strCurrentString.size() );

			// when shift key was pushed, select string
			if( m_bShiftPushed )
			{
				if( m_unSelectStartPos == m_unWritePos && m_unSelectEndPos == m_unWritePos )
				{
					m_unSelectEndPos = m_unCaretPos;
				}
				else if( m_unSelectStartPos != m_unWritePos && m_unSelectEndPos == m_unWritePos )
				{
					m_unSelectStartPos = m_unSelectEndPos;
					m_unSelectEndPos = m_unCaretPos;
				}
				else if( m_unSelectStartPos == m_unWritePos && m_unSelectEndPos != m_unWritePos )
				{
					m_unSelectEndPos = m_unCaretPos;
				}

				// selection region vaildness check
				if( m_unSelectStartPos != m_unSelectEndPos )
				{
					m_bTextSelecting = true;
				}
				else
				{
					m_bTextSelecting = false;
				}
			}
			else
			{
				m_bTextSelecting = false;

				m_unSelectStartPos = m_unSelectEndPos = m_unCaretPos;

				m_unWritePos = m_unCaretPos;
			}
		}
		break;

	case VK_LEFT:
		{
			LPSTR buf = CharPrev( &m_strCurrentString[ 0 ], &m_strCurrentString[ m_unCaretPos ] );

			UINT unPos = static_cast<UINT>( &m_strCurrentString[ m_unCaretPos ] - buf );

			if( unPos != 0 && m_unPrimaryLanguage == ZIme::KEY_THAI )
				unPos = 1;

			m_unCaretPos -= unPos;

			// when shift key was pushed, select string
			if( m_bShiftPushed )
			{
				if( m_unSelectStartPos == m_unWritePos && m_unSelectEndPos == m_unWritePos )
				{
					m_unSelectStartPos = m_unCaretPos;
				}
				else if( m_unSelectStartPos != m_unWritePos && m_unSelectEndPos == m_unWritePos )
				{
					m_unSelectStartPos = m_unCaretPos;
				}
				else if( m_unSelectStartPos == m_unWritePos && m_unSelectEndPos != m_unWritePos )
				{
					m_unSelectEndPos = m_unCaretPos;
				}

				// selection region vaildness check
				if( m_unSelectStartPos != m_unSelectEndPos )
				{
					m_bTextSelecting = true;
				}
				else
				{
					m_bTextSelecting = false;
				}
			}
			else
			{
				m_bTextSelecting = false;

				if( m_unSelectStartPos != m_unSelectEndPos )
				{
					m_unCaretPos = m_unSelectStartPos;
				}

				m_unSelectStartPos = m_unSelectEndPos = m_unCaretPos;

				m_unWritePos = m_unCaretPos;
			}
		}
		break;

	case VK_RIGHT:
		{
			LPSTR buf = CharNext( &m_strCurrentString[ m_unCaretPos ] );

			UINT unPos = static_cast<UINT>( buf - &m_strCurrentString[ m_unCaretPos ] );

			if( unPos != 0 && m_unPrimaryLanguage == ZIme::KEY_THAI )
				unPos = 1;

			m_unCaretPos += unPos;

			// when shift key was pushed, select string
			if( m_bShiftPushed )
			{
				if( m_unSelectStartPos == m_unWritePos && m_unSelectEndPos == m_unWritePos )
				{
					m_unSelectEndPos = m_unCaretPos;
				}
				else if( m_unSelectStartPos != m_unWritePos && m_unSelectEndPos == m_unWritePos )
				{
					m_unSelectStartPos = m_unCaretPos;
				}
				else if( m_unSelectStartPos == m_unWritePos && m_unSelectEndPos != m_unWritePos )
				{
					m_unSelectEndPos = m_unCaretPos;
				}

				// selection region vaildness check
				if( m_unSelectStartPos != m_unSelectEndPos )
				{
					m_bTextSelecting = true;
				}
				else
				{
					m_bTextSelecting = false;
				}
			}
			else
			{
				m_bTextSelecting = false;

				if( m_unSelectStartPos != m_unSelectEndPos )
				{
					m_unCaretPos = m_unSelectEndPos;
				}

				m_unSelectStartPos = m_unSelectEndPos = m_unCaretPos;

				m_unWritePos = m_unCaretPos;
			}
		}
		break;

	case VK_BACK:   // BACKSPACE
		{
			if( m_unCaretPos >= 0 )
			{
				// if selection region is valid state, delete string in region
				if( m_unSelectStartPos != m_unSelectEndPos )
				{
					m_strBackupString.erase( m_strBackupString.begin() + m_unSelectStartPos, m_strBackupString.begin() + m_unSelectEndPos );

					m_strCurrentString = m_strBackupString;

					m_unSelectStartPos = m_unSelectEndPos = ( m_unWritePos > m_unCaretPos ? m_unWritePos = m_unCaretPos : m_unCaretPos = m_unWritePos );

					m_bTextSelecting = false;
				}
				else
				{
					LPSTR buf = CharPrev( &m_strCurrentString[ 0 ], &m_strCurrentString[ m_unCaretPos ] );
					UINT unPos = static_cast<UINT>( &m_strCurrentString[ m_unCaretPos ] - buf );

					if( unPos != 0 && m_unPrimaryLanguage == ZIme::KEY_THAI )
						unPos = 1;

					m_unCaretPos -= unPos;
					m_unWritePos = m_unCaretPos;

					m_strCurrentString.erase( m_unWritePos, unPos );
					m_strBackupString = m_strCurrentString;
				}
			}
		}
		break;

	case VK_DELETE:
		{
			if( m_unCaretPos <= static_cast<int>( m_strCurrentString.size() ) )
			{
				// if selection region is valid state, delete string in region
				if( m_unSelectStartPos != m_unSelectEndPos )
				{
					m_strBackupString.erase( m_strBackupString.begin() + m_unSelectStartPos, m_strBackupString.begin() + m_unSelectEndPos );

					m_strCurrentString = m_strBackupString;

					m_unSelectStartPos = m_unSelectEndPos = ( m_unWritePos > m_unCaretPos ? m_unWritePos = m_unCaretPos : m_unCaretPos = m_unWritePos );

					m_bTextSelecting = false;
				}
				else
				{
					LPSTR buf = CharNext( &m_strCurrentString[ m_unCaretPos ] );
					UINT unPos = static_cast<UINT>( buf - &m_strCurrentString[ m_unCaretPos ] );

					if( unPos != 0 && m_unPrimaryLanguage == ZIme::KEY_THAI )
						unPos = 1;

					m_strCurrentString.erase( m_unCaretPos, unPos );
					m_strBackupString = m_strCurrentString;
				}
			}
		}
		break;

	case VK_RETURN:
		{
			// Return Key
		}
		break;

	case VK_INSERT: 
		{
			// Toggle Insert State
			if( m_bInsert )
			{
				m_bInsert = false;
			}
			else                 
			{
				m_bInsert = true;
			}
		}
		break;
	}

	// Ctrl + V : Paste
	if( ( m_bCtrlPushed && ( wParam == 86 ) ) || ( m_bShiftPushed && ( wParam == 45 ) ) )
	{
		PasteString();
	}

	// Ctrl + C : Copy
	if( ( m_bCtrlPushed && ( wParam == 67 ) ) || ( m_bCtrlPushed && ( wParam == 45 ) ) )
	{
		CopyString();
	}

	// Ctrl + X : Cut
	if( m_bCtrlPushed && ( wParam == 88 ) )
	{
		CutString();
	}

	return true;
}

//-----------------------------------------------------------------------------------------------------
// GMIMEPACK::OnChar()
bool GMIMEPACK::OnChar( WPARAM wParam, LPARAM lParam )
{
	if( CheckAlphabet( static_cast<int>( wParam ) )
		|| CheckDigit( static_cast<int>( wParam ) )
		|| ( static_cast<UINT>( wParam == ' ' ) )
		|| wParam == ' '
		|| CheckSpecialLetter( static_cast<int>( wParam ) ) )
	{
		// if 'numeral input only' is allowed, stop processing
		if( m_bNumeralOnly ) 
		{
			if( static_cast<int>( wParam ) < '0' || static_cast<int>( wParam ) > '9' )
				return true;
		}

        if( m_bEnglishOnly )
        {
            if( static_cast<int>( wParam ) < '!' || static_cast<int>( wParam ) > '~' )
            {
                CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
		        lpMessageBox->Create(CRYLStringTable::m_strString[ 2775 ]);
                return true;
            }
        }

		// if selection region is valid state, delete string in region
		if( m_unSelectStartPos != m_unSelectEndPos )
		{
			m_strBackupString.erase( m_strBackupString.begin() + m_unSelectStartPos, m_strBackupString.begin() + m_unSelectEndPos );

			m_strCurrentString = m_strBackupString;

			m_unSelectStartPos = m_unSelectEndPos = ( m_unWritePos > m_unCaretPos ? m_unWritePos = m_unCaretPos : m_unCaretPos = m_unWritePos );

			m_bTextSelecting = false;
		}

		UINT unSize = static_cast<UINT>( m_strBackupString.size() );

		// insert character into string buffer
		if( m_unCaretPos < unSize )
		{
			//-- when caret placed in the string
			if( m_bInsert )
			{
				if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::THAILAND )
				{
					if ( !InsertThiaString( wParam, lParam ) )
						return false ;
				}
				else
				{
					m_strBackupString.insert( (std::string::size_type)m_unCaretPos, 1, static_cast<char>( wParam ) );
				}
			}
			else
			{
				LPSTR buf = CharNext( &m_strBackupString[ m_unCaretPos ] );
				unsigned int unPos = static_cast<UINT>( buf - &m_strBackupString[ m_unCaretPos ] );

				m_strBackupString.replace( m_unCaretPos, unPos, 1, static_cast<char>( wParam ) );
			}
		}                  
		else
		{
			//-- when caret placed right edge of string
			// in thia
			if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::THAILAND )
			{
				if ( !PushbackThiaString( wParam, lParam ) )
					return false ;
			}
			else
			{
				m_strBackupString.push_back( static_cast<char>( wParam ) );
			}
		}

		if( static_cast<UINT>( m_strBackupString.size() ) > m_unStringLimit )
		{
			// check for character which end of string is 1 byte or 2 bytes
			/*LPSTR buf = CharNext( &m_strBackupString[ m_strBackupString.size() ] );
			unsigned int unPos = static_cast<UINT>( buf - &m_strBackupString[ m_strBackupString.size() ] );*/

			LPSTR buf = CharPrev( &m_strBackupString[ 0 ], &m_strBackupString[ m_strBackupString.size() ] );
			UINT unPos = static_cast<UINT>( &m_strBackupString[ m_strBackupString.size() ] - buf );

			if( unPos % 2 )
			{  
				// 1 byte
				m_strBackupString.resize( m_unStringLimit );            
			}
			else
			{
				// 2 bytes
				m_strBackupString.resize( m_unStringLimit - 1 );
			}
		}

		m_unCaretPos++;

		if( m_unCaretPos > m_strBackupString.length() )
			m_unCaretPos = static_cast<UINT>( m_strBackupString.length() );

		m_unWritePos = m_unCaretPos;

		m_strCurrentString = m_strBackupString;
	}
	return true;
}

bool	GMIMEPACK::InsertThiaString( WPARAM wParam, LPARAM lParam ) 
{
	BYTE szCurrentStr = static_cast<BYTE>( wParam );

	if ( ( ( szCurrentStr >= 0xe7 && szCurrentStr <= 0xed ) || 
		   ( szCurrentStr >= 0xd4 && szCurrentStr <= 0xda ) ||
		   ( szCurrentStr == 0xd1 ) ) && ( m_unCaretPos == 0 ) )
	{
		return false ;
	}
	else
	{
		m_strBackupString.insert( (std::string::size_type)m_unCaretPos, 1, static_cast<char>( wParam ) );
	}

	return true ;
}

bool	GMIMEPACK::PushbackThiaString( WPARAM wParam, LPARAM lParam ) 
{
	unsigned int	iStrLen = static_cast<unsigned int>( m_strBackupString.length() );
	BYTE szCurrentStr = static_cast<BYTE>( wParam );
	BYTE strAlphabet  = static_cast<BYTE>( m_strBackupString[ iStrLen - 1 ] ) ;

	if ( ( szCurrentStr >= 0xe7 && szCurrentStr <= 0xed ) || 
		( szCurrentStr >= 0xd4 && szCurrentStr <= 0xda ) ||
		( szCurrentStr == 0xd1 ) )
	{
		if ( iStrLen == 0 ) 
			return false ;
		
		if ( ( strAlphabet >= 0xe7 && strAlphabet <= 0xed ) || 
			( strAlphabet >= 0xd4 && strAlphabet <= 0xda ) ||
			( strAlphabet >= 0xd1 ) )
		{
			if ( ( szCurrentStr >= 0xe8 && szCurrentStr <= 0xed ) || ( szCurrentStr == 0xd3 ) )
			{
				BYTE strUpper1  = static_cast<BYTE>( m_strBackupString[ iStrLen - 1 ] ) ;
				BYTE strUpper2  = static_cast<BYTE>( m_strBackupString[ iStrLen - 2 ] ) ;
				if ( ( strUpper1 >= 0xe7 && strUpper1 <= 0xed ) || ( strUpper1 >= 0xd4 && strUpper1 <= 0xda ) ||( strUpper1 == 0xd1 ) )
				{
					if ( ( strUpper2 >= 0xe7 && strUpper2 <= 0xed ) || ( strUpper2 >= 0xd4 && strUpper2 <= 0xda ) ||( strUpper2 == 0xd1 ) )
					{
						return false ;
					}
					else
					{
						m_strBackupString.push_back( static_cast<char>( wParam ) );
					}
				}
				else
				{
					return false ;
				}
			}
			else
			{
				return false ;
			}
		}
		else
		{
			m_strBackupString.push_back( static_cast<char>( wParam ) );
		}
	}
	else
	{
		m_strBackupString.push_back( static_cast<char>( wParam ) );
	}

	return true ;
}

//-----------------------------------------------------------------------------------------------------
// GMIMEPACK::OnInputLangChange()
bool GMIMEPACK::OnInputLangChange( WPARAM wParam, LPARAM lParam )
{
	HKL hKeyboardLayout = (HKL)lParam;

	// Check keyboard layout
	if( hKeyboardLayout == 0 ) 
		hKeyboardLayout = ::GetKeyboardLayout( 0 );

	switch( LOWORD( hKeyboardLayout ) )
	{
		// Korean
	case ZIme::LID_KOREAN:				
		{
			m_unPrimaryLanguage = ZIme::KEY_KOREAN;
			m_strLanguageName = "KOREAN";
		}
		break;

		// Japanese
	case ZIme::LID_JAPANESE:
		{
			m_unPrimaryLanguage = ZIme::KEY_JAPANESE;
			m_strLanguageName = "JAPANESE";
		}
		break;

		// Traditional Chinese(대만) 
	case ZIme::LID_TRADITIONAL_CHINESE:
		{
			m_unPrimaryLanguage = ZIme::KEY_TRADITIONAL_CHINESE;
			m_strLanguageName = "TRADITIONAL CHINESE";

			switch( HIWORD( hKeyboardLayout ) )
			{
			case ZIme::LID_CHANGJIE:
				{
					m_unSubLanguage = ZIme::SUB_KEY_CHANJIE_TRADITIONAL_CHINESE;
					m_strLanguageName += " ( ChangJie ) ";
				}
				break;

			case ZIme::LID_US_KEYBOARD:
				{
					m_unSubLanguage = ZIme::SUB_KEY_US_KEYBOARD_TRADITIONAL_CHINESE;
					m_strLanguageName += " ( US Keyboard ) ";
				}
				break;

			case ZIme::LID_USYAMI:
				{
					m_unSubLanguage = ZIme::SUB_KEY_USYAMI_TRADITIONAL_CHINESE;
					m_strLanguageName += " ( Usyami ) ";
				}
				break;

			default:
				{
					m_unSubLanguage = ZIme::SUB_KEY_UNKNOWN;
					m_strLanguageName += " ( Unknown ) ";
				}
				break;
			}
		}
		break;

		// Simplified Chinese(중국)
	case ZIme::LID_SIMPLIFIED_CHINESE:
		{
			m_unPrimaryLanguage = ZIme::KEY_SIMPLIFIED_CHINESE;
			m_strLanguageName = "SIMPLIFIED CHINESE";
		}
		break;

	case ZIme::LID_THAI:
		{
			m_unPrimaryLanguage = ZIme::KEY_THAI;
			m_strLanguageName = "THAI";
		}
		break;

	default:
		{
			m_unPrimaryLanguage = ZIme::KEY_UNKNOWN_LANGUAGE;
			m_strLanguageName = "UNKNOWN LANGUAGE";
		}
		break;
	}

	return true;
}

//-----------------------------------------------------------------------------------------------------
// GMIMEPACK::OnImeStartComposition()
bool GMIMEPACK::OnImeStartComposition( WPARAM wParam, LPARAM lParam )
{
	// if selection region is valid state, delete string in region
	if( m_unSelectStartPos != m_unSelectEndPos )
	{
		m_strBackupString.erase( m_strBackupString.begin() + m_unSelectStartPos, m_strBackupString.begin() + m_unSelectEndPos );

		m_strCurrentString = m_strBackupString;

		m_unSelectStartPos = m_unSelectEndPos = ( m_unWritePos > m_unCaretPos ? m_unWritePos = m_unCaretPos : m_unCaretPos = m_unWritePos );

		m_bTextSelecting = false;
	}

	return m_bUseCustomImeWindow ? true : false;
}

//-----------------------------------------------------------------------------------------------------
// GMIMEPACK::OnImeEndComposition()
bool GMIMEPACK::OnImeEndComposition( WPARAM wParam, LPARAM lParam )
{
	m_strCurrentComposingString.clear();
	m_strAttribute.clear();
	m_vtClause.clear();

	return m_bUseCustomImeWindow ? true : false;
}

//-----------------------------------------------------------------------------------------------------
// GMIMEPACK::OnImeComposition()
bool GMIMEPACK::OnImeComposition( WPARAM wParam, LPARAM lParam )
{
	// get ime handle
	m_hImc			= ImmGetContext( m_hWnd );

	// composing process
	if( lParam & GCS_COMPSTR )
	{
		OnGcsCompStr( lParam );
	}

	// end of composing
	if( lParam & GCS_RESULTSTR )
	{
		OnGcsResultStr( lParam );
	}

	if( lParam & GCS_CURSORPOS )
	{
	}

	// when a string where place on composition window was completely deleted
	if( lParam == 0 )
	{  
		m_unComposingLength = 0;

		// restore current string
		m_strCurrentString = m_strBackupString;

		// clear string attribute
		m_strAttribute.clear();

		m_unCaretPos = m_unWritePos;
	}

	// Release IME handle
	ImmReleaseContext( m_hWnd, m_hImc );

	return m_bUseCustomImeWindow ? true : false;
}

//-----------------------------------------------------------------------------------------------------
// GMIMEPACK::OnImeNotify()
bool GMIMEPACK::OnImeNotify( WPARAM wParam, LPARAM lParam )
{
	// get ime handle
	m_hImc			= ImmGetContext( m_hWnd );

	switch( wParam )
	{
	case IMN_SETCONVERSIONMODE:
		{
			// check OS version, and do not call the conversion status win32 functions if it's version was 'Win ME' 
			//OSVERSIONINFO osVersion;
			//ZeroMemory( &osVersion, sizeof( OSVERSIONINFO ) );
			//osVersion.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );

			//if( GetVersionEx( &osVersion ) )
			//{
			//    if( osVersion.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
			//    {
			//        if( osVersion.dwMajorVersion == 4 && osVersion.dwMinorVersion == 90 )
			//            break;
			//    }
			//    else
			//    {
			//        DWORD state;

			//        ImmGetConversionStatus( m_hImc, &state, NULL );

			//        // check full width or half width
			//        if( state & IME_CMODE_FULLSHAPE )
			//        {
			//            m_bFullWidth = true;
			//        }
			//        else
			//        {
			//            m_bFullWidth = false;
			//        }
			//    }
			//}
		}
		break;

	case IMN_SETOPENSTATUS:
		break;

	case IMN_CLOSECANDIDATE:
		{  
			// close candidate window
			m_vtCandidateList.clear();
			m_unCandidateCount = 0;             
			m_unCandidateSelectedIdx = 0;       
			m_unCandidateCurrentSelectedIdx = 0;
		}
		break;

	case IMN_GUIDELINE:
		{
			// when composition window open
		}
		break;

	case IMN_PRIVATE:
		{
		}
		break;

	case IMN_OPENCANDIDATE:
	case IMN_CHANGECANDIDATE:
		{
			// process routine concerned with candidate list
			m_vtCandidateList.clear();

			UINT  unBufLen = ImmGetCandidateList( m_hImc, 0, NULL, 0 );

			if( !unBufLen )
				return true;

			LPCANDIDATELIST lpCandidateList = reinterpret_cast<LPCANDIDATELIST>( new char[ unBufLen ] );

			ImmGetCandidateList( m_hImc, 0, lpCandidateList, unBufLen );

			// calculate various value
			UINT unCandidatePageStart  = lpCandidateList->dwPageStart;
			UINT unCandidateSelection  = lpCandidateList->dwSelection;
			UINT unCandidateCount      = lpCandidateList->dwCount;

			UINT page = ( lpCandidateList->dwSelection / m_unCandidateOnceRetrieveCount ) * m_unCandidateOnceRetrieveCount;

			UINT unCurrentCandidateCount = ( lpCandidateList->dwCount - page ) > m_unCandidateOnceRetrieveCount ? m_unCandidateOnceRetrieveCount : ( lpCandidateList->dwCount - page );
			UINT unCurrentCandidateIndex = lpCandidateList->dwSelection % m_unCandidateOnceRetrieveCount;

			// get candidate list elements that current selected and it's neighborhood
			for( DWORD i = page, j = 0 ; i < page + unCurrentCandidateCount ; ++i, ++j )
			{
				LPSTR lpStr = reinterpret_cast<LPSTR>( lpCandidateList ) + lpCandidateList->dwOffset[ i ];

				m_vtCandidateList.push_back( lpStr );
			}

			// candidate status member variables setting
			m_unCandidatePageStartIdx = unCandidatePageStart;
			m_unCandidateCount = unCandidateCount;
			m_unCandidateSelectedIdx = unCandidateSelection;
			m_unCandidateCurrentSelectedIdx = unCurrentCandidateIndex;

			delete [] lpCandidateList;
			lpCandidateList = NULL;
		}
		break;
	}

	// Release IME handle
	ImmReleaseContext( m_hWnd, m_hImc );

	return m_bUseCustomImeWindow ? true : false;
}

//-----------------------------------------------------------------------------------------------------
// GMIMEPACK::OnGcsCompStr()
void GMIMEPACK::OnGcsCompStr( LPARAM lParam )
{
	if( m_bNumeralOnly ) 
	{
		return;
	}

    if( m_bEnglishOnly )
    {
        CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create(CRYLStringTable::m_strString[ 2776 ]);

        return;
    }

	// check composing string length
	m_unComposingLength = ImmGetCompositionString( m_hImc, GCS_COMPSTR, NULL, 0 );

	if( m_unComposingLength > m_unStringLimit - m_unWritePos )
	{
		if( ( m_unComposingLength - ( m_unStringLimit - m_unWritePos ) ) % 2 == 1 && m_bFullWidth )
		{
			m_unComposingLength = m_unStringLimit - m_unWritePos - 1;
		}
		else
		{
			m_unComposingLength = m_unStringLimit - m_unWritePos;
		}
	}

	// allocate the temporary string buffer
	char* pcComposingString = new char[ m_unComposingLength + 1 ];
	ZeroMemory( pcComposingString, m_unComposingLength + 1 );

	// get the composing string from composition window
	ImmGetCompositionString( m_hImc, GCS_COMPSTR, pcComposingString, m_unComposingLength );

	// cut the end of string.
	pcComposingString[ m_unComposingLength ] = 0;

	if(m_unPrimaryLanguage == ZIme::KEY_SIMPLIFIED_CHINESE)
	{
		int ibackup = (int)m_strBackupString.size();
		int icur = (int)m_strCurrentString.size();
		int ico = (int)m_strCurrentComposingString.size();
		string oldcur = m_strCurrentString;

		// backup string size non zero
		if(m_strBackupString.size())
		{
			m_strCurrentString = m_strBackupString;
			if(m_strCurrentString.size() < m_unWritePos)
			{
				m_unCaretPos=m_unWritePos = m_strCurrentString.size();
			}
		}
		else
		{
			m_strCurrentString.clear();
			m_unWritePos=0;
		}

		// insert composing string
		if(pcComposingString[0])
		{
			m_strCurrentString.insert( m_unWritePos, pcComposingString );

			// backup current composing string
			m_strCurrentComposingString = pcComposingString;
		}
		else
		{
			m_strCurrentComposingString.clear();
		}
	}
	else
	{
		m_strCurrentString = m_strBackupString;

		// insert composing string
		m_strCurrentString.insert( m_unWritePos, pcComposingString );

		// backup current composing string
		m_strCurrentComposingString = pcComposingString;
	}

	delete [] pcComposingString;

	// resize string if it is over the limit length
	if( m_strCurrentString.size() > m_unStringLimit )
		m_strCurrentString.resize( m_unStringLimit );

	// if keyboard layout is traditional chinese, stop processing
	if( m_unPrimaryLanguage == ZIme::KEY_TRADITIONAL_CHINESE )
		return;

	// check cursor position
	DWORD dwCursorPosInComposition = ImmGetCompositionString( m_hImc, GCS_CURSORPOS, NULL, 0 );

	if( dwCursorPosInComposition > m_unComposingLength )
		dwCursorPosInComposition = m_unComposingLength;

	if( m_unWritePos + dwCursorPosInComposition <= m_unStringLimit )
		m_unCaretPos = m_unWritePos + dwCursorPosInComposition;

	// save the String Attribute.
	UINT unAttributeLength = ImmGetCompositionString( m_hImc, GCS_COMPATTR, NULL, 0 );

	char* pcComposingAttribute = new char[ unAttributeLength + 1 ];
	ZeroMemory( pcComposingAttribute, unAttributeLength );

	ImmGetCompositionString( m_hImc, GCS_COMPATTR, pcComposingAttribute, unAttributeLength );

	pcComposingAttribute[ unAttributeLength ] = 0;

	m_strAttribute.assign( pcComposingAttribute, unAttributeLength );

	delete [] pcComposingAttribute;

	// save the clause data
	UINT unClauseLength = ImmGetCompositionString( m_hImc, GCS_COMPCLAUSE, NULL, 0 );

	DWORD32* pdw32Clause = new DWORD32[ unClauseLength / sizeof( DWORD32 ) ];
	ZeroMemory( pdw32Clause, unClauseLength );

	ImmGetCompositionString( m_hImc, GCS_COMPCLAUSE, pdw32Clause, unClauseLength );

	m_vtClause.clear();     // clear clause buffer

	for( UINT i = 0 ; i < ( unClauseLength / sizeof( DWORD32 ) ); ++i )
	{
		m_vtClause.push_back( pdw32Clause[ i ] );
	}

	delete [] pdw32Clause;
}

//-----------------------------------------------------------------------------------------------------
// GMIMEPACK::OnGcsResultStr()
void GMIMEPACK::OnGcsResultStr( LPARAM lParam )
{
	if( m_bNumeralOnly ) 
	{
		return;
	}

    if( m_bEnglishOnly )
    {
        CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create(CRYLStringTable::m_strString[ 2776 ]);

        return;
    }

	// check composed string length
	m_unComposingLength = ImmGetCompositionString( m_hImc, GCS_RESULTSTR, NULL, 0 );

	if( m_unComposingLength > m_unStringLimit - m_unWritePos )
	{
		if( ( m_unComposingLength - ( m_unStringLimit - m_unWritePos ) ) % 2 == 1 && m_bFullWidth )
		{
			m_unComposingLength = m_unStringLimit - m_unWritePos - 1;
		}
		else
		{
			m_unComposingLength = m_unStringLimit - m_unWritePos;
		}
	}

	char* pcComposingString = new char[ m_unComposingLength + 1 ];
	ZeroMemory( pcComposingString, m_unComposingLength + 1 );

	// get the composing string from composition window
	ImmGetCompositionString( m_hImc, GCS_RESULTSTR, pcComposingString, m_unComposingLength );

	pcComposingString[ m_unComposingLength ] = 0;

	// if allows 'numeral input only', do not get other character
	if( m_bNumeralOnly || m_bEnglishOnly ) 
	{
		if( pcComposingString[ 0 ] < '0' && pcComposingString[ 0 ] > '9' )
		{
			delete [] pcComposingString;

			m_unComposingLength = 0;

			// backup complete string
			m_strBackupString = m_strCurrentString;

			return;
		}

	}

	if(m_unPrimaryLanguage == ZIme::KEY_SIMPLIFIED_CHINESE)
	{
		if( pcComposingString[ 0 ] )
		{
			// insert composing string
			if(m_strBackupString.size() > 0)
				m_strCurrentString = m_strBackupString;

			m_strCurrentString.insert( m_unWritePos, pcComposingString );

			// resize string if it is over the limit length
			if( m_strCurrentString.size() > m_unStringLimit )
				m_strCurrentString.resize( m_unStringLimit );

			if( m_unCaretPos > m_unStringLimit )
				m_unCaretPos = m_unStringLimit;

			// control write position
			m_unWritePos += m_unComposingLength;
			m_unCaretPos = m_unWritePos;
		}
	}
	else
	{
		if( pcComposingString[ 0 ] )
		{
			// insert composing string
			m_strCurrentString = m_strBackupString;
			m_strCurrentString.insert( m_unWritePos, pcComposingString );

			// resize string if it is over the limit length
			if( m_strCurrentString.size() > m_unStringLimit )
				m_strCurrentString.resize( m_unStringLimit );

			if( m_unCaretPos > m_unStringLimit )
				m_unCaretPos = m_unStringLimit;

			// control write position
			m_unWritePos += m_unComposingLength;
			m_unCaretPos = m_unWritePos;
		}
	}

	delete [] pcComposingString;

	m_unComposingLength = 0;

	// backup complete string
	m_strBackupString = m_strCurrentString;

	// clear buffer
	m_strCurrentComposingString.clear();
	m_strAttribute.clear();
	m_vtClause.clear();

	if( !m_bTextSelecting )
		m_unSelectStartPos = m_unSelectEndPos = m_unWritePos;

	// when processing end, if there exist remain composing string...
	if( m_unPrimaryLanguage != ZIme::KEY_TRADITIONAL_CHINESE && ImmGetCompositionString( m_hImc, GCS_COMPSTR, NULL, 0 ) )
	{
		SendMessage( m_hWnd, WM_IME_COMPOSITION, 0, GCS_COMPREADSTR | GCS_COMPREADATTR | GCS_COMPREADCLAUSE | GCS_COMPSTR 
			| GCS_COMPATTR | GCS_COMPCLAUSE | GCS_CURSORPOS | GCS_DELTASTART );
	}
}

//-----------------------------------------------------------------------------------------------------
// GMIMEPACK::SetString()
void GMIMEPACK::SetString( char* str )
{
	ClearString();

	m_strBackupString = m_strCurrentString = str;
	m_unWritePos = m_unCaretPos = static_cast<UINT>( m_strBackupString.length() );

	m_unSelectStartPos = m_unSelectEndPos = 0;
	m_bTextSelecting = false;
}

//-----------------------------------------------------------------------------------------------------
// GMIMEPACK::SetStringLimit()
void GMIMEPACK::SetStringLimit( UINT unStringLimit )
{
	m_unStringLimit = unStringLimit;

	m_strCurrentString.reserve( unStringLimit * 2 );            // allocate double size of string limit
	m_strCurrentComposingString.reserve( unStringLimit );
	m_strBackupString.reserve( unStringLimit );
	m_strAttribute.reserve( unStringLimit );
	m_vtClause.reserve( unStringLimit / sizeof( DWORD32 ) );
}

//-----------------------------------------------------------------------------------------------------
// GMIMEPACK::SetEnglishOnly()
void GMIMEPACK::SetEnglishOnly( bool bOnly )
{
    m_bEnglishOnly = bOnly;

	if( bOnly )
	{
        m_hImc			= ImmGetContext( m_hWnd );

		DWORD state = 0;
		DWORD sentence = 0;

		state |= IME_CMODE_ALPHANUMERIC;
        //state |= IME_CMODE_NOCONVERSION;

        //sentence |= IME_SMODE_CONVERSATION;
        sentence |= IME_SMODE_AUTOMATIC;

		ImmSetConversionStatus( m_hImc, state, sentence );

		ImmReleaseContext( m_hWnd, m_hImc );
	}
	else
	{
		m_hImc			= ImmGetContext( m_hWnd );

		DWORD state = 0;
		DWORD sentence = 0;                 

		state |= IME_CMODE_NATIVE;
		state |= IME_CMODE_ROMAN;

		sentence |= IME_SMODE_PHRASEPREDICT;

		ImmSetConversionStatus( m_hImc, state, sentence );

		ImmReleaseContext( m_hWnd, m_hImc );
	}
}

//-----------------------------------------------------------------------------------------------------
// GMIMEPACK::SetStatusWindowPosition()
void GMIMEPACK::SetStatusWindowPosition( POINT ptPos )
{
	m_hImc			= ImmGetContext( m_hWnd	);

	POINT ptTemp;

	ImmGetStatusWindowPos( m_hImc, &ptTemp );

	if( !ImmSetStatusWindowPos( m_hImc, &ptPos ) )
	{
		CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create("Modify status window position failed");
	}

	ImmReleaseContext( m_hWnd, m_hImc );
}

//-----------------------------------------------------------------------------------------------------
// GMIMEPACK::SetCompositionWindowPosition()
void GMIMEPACK::SetCompositionWindowPosition( POINT ptPos )
{
	m_hImc			= ImmGetContext( m_hWnd );

	ImmGetCompositionWindow( m_hImc, m_lpCompositionForm );

	m_lpCompositionForm->dwStyle = CFS_FORCE_POSITION;
	m_lpCompositionForm->ptCurrentPos = ptPos;

	if( !ImmSetCompositionWindow( m_hImc, m_lpCompositionForm ) )
	{
		CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create("Modify composition window position failed");
	}

	ImmReleaseContext( m_hWnd, m_hImc );   
}

//-----------------------------------------------------------------------------------------------------
// GMIMEPACK::SetCandidateWindowPosition()
void GMIMEPACK::SetCandidatewindowPosition( POINT ptPos )
{
	m_hImc			= ImmGetContext( m_hWnd );

	ImmGetCandidateWindow( m_hImc, 0, m_lpCandidateForm );

	m_lpCandidateForm->dwStyle = CFS_CANDIDATEPOS;
	m_lpCandidateForm->ptCurrentPos = ptPos;

	if( !ImmSetCandidateWindow( m_hImc, m_lpCandidateForm ) )
	{
		CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create("Modify candidate window position failed");
	}

	ImmReleaseContext( m_hWnd, m_hImc );
}

//-----------------------------------------------------------------------------------------------------
// GMIMEPACK::SetCandidateListPrev()
void GMIMEPACK::SetCandidateListPrev()
{
	if( m_vtCandidateList.empty() )
	{
		// get ime handle
		m_hImc			= ImmGetContext( m_hWnd );

		UINT unTempIdx = m_unCandidatePageStartIdx + m_unCandidateOnceRetrieveCount;

		if( unTempIdx > m_unCandidateCount )
			unTempIdx = m_unCandidateCount;

		ImmNotifyIME( m_hImc, NI_SETCANDIDATE_PAGESTART, CPS_CONVERT, unTempIdx );

		ImmReleaseContext( m_hWnd, m_hImc );
	}
}

//-----------------------------------------------------------------------------------------------------
// GMIMEPACK::SetCandidateListNext()
void GMIMEPACK::SetCandidateListNext()
{
	if( m_vtCandidateList.empty() )
	{
		// get ime handle
		m_hImc			= ImmGetContext( m_hWnd );

		int nTempIdx = static_cast<int>( m_unCandidatePageStartIdx - m_unCandidateOnceRetrieveCount );

		if( nTempIdx <  0 )
			nTempIdx = 0;

		ImmNotifyIME( m_hImc, NI_SETCANDIDATE_PAGESTART, CPS_CONVERT, nTempIdx );

		ImmReleaseContext( m_hWnd, m_hImc );
	}
}

//-----------------------------------------------------------------------------------------------------
// GMIMEPACK::ClearString()
void GMIMEPACK::ClearString()
{  
	m_strCurrentString.clear();
	m_strCurrentComposingString.clear();
	m_strBackupString.clear();
	m_strAttribute.clear();
	m_vtClause.clear();
	m_vtCandidateList.clear();
}

//-----------------------------------------------------------------------------------------------------
// GMIMEPACK::CopyString()
void GMIMEPACK::CopyString()
{
	if( m_unSelectStartPos != m_unSelectEndPos )
	{
		m_strCopyString.clear();

		char* pTempStr = new char[ ( m_unSelectEndPos - m_unSelectStartPos ) + 1 ];

		m_strBackupString.copy( pTempStr, m_unSelectEndPos - m_unSelectStartPos, m_unSelectStartPos );

		pTempStr[ ( m_unSelectEndPos - m_unSelectStartPos ) ] = 0;

		m_strCopyString = pTempStr;

		delete [] pTempStr;
	}
	else
	{
		// if there's no selection region, copy whole string
		m_strCopyString.clear();

		m_strCopyString = m_strBackupString;
	}

    HGLOBAL hMem = GlobalAlloc( GHND, m_strCopyString.length() + 1 );
        
    char* pClipString = (char*)GlobalLock( hMem );
    memcpy( pClipString, m_strCopyString.c_str(), m_strCopyString.length() );
    GlobalUnlock( hMem );

    if( OpenClipboard( m_hWnd ) )
    {

        EmptyClipboard();
        SetClipboardData( CF_TEXT, hMem );
        CloseClipboard();
    }
}

//-----------------------------------------------------------------------------------------------------
// GMIMEPACK::PasteString()
void GMIMEPACK::PasteString()
{
	if( m_strCurrentComposingString.length() )
	{
		// get ime handle
		m_hImc			= ImmGetContext( m_hWnd );

		ImmNotifyIME( m_hImc, NI_COMPOSITIONSTR, CPS_COMPLETE, 0 );

		ImmReleaseContext( m_hWnd, m_hImc );
	}

    if( IsClipboardFormatAvailable( CF_TEXT ) )
    {
        OpenClipboard( m_hWnd );
        HGLOBAL hMem = GetClipboardData( CF_TEXT );
        char* pClipString = (char*)GlobalLock( hMem );
        m_strCopyString = pClipString;
        GlobalUnlock( hMem );
        CloseClipboard();
    }

	// if selection region is valid state, delete string in region
	if( m_unSelectStartPos != m_unSelectEndPos )
	{
		m_strBackupString.erase( m_strBackupString.begin() + m_unSelectStartPos, m_strBackupString.begin() + m_unSelectEndPos );

		m_strCurrentString = m_strBackupString;

		m_unSelectStartPos = m_unSelectEndPos = ( m_unWritePos > m_unCaretPos ? m_unWritePos = m_unCaretPos : m_unCaretPos = m_unWritePos );

		m_bTextSelecting = false;
	}

	if( m_strBackupString.length() + m_strCopyString.length() < m_unStringLimit )
	{
		m_strBackupString.insert( m_unWritePos, m_strCopyString.c_str() );

		m_unCaretPos += static_cast<UINT>( m_strCopyString.length() );
	}
	else
	{
		m_strBackupString.insert( m_strBackupString.begin() + m_unWritePos, m_strCopyString.begin(), m_strCopyString.begin() + ( m_unStringLimit - m_strBackupString.length() ) );

		m_unCaretPos += ( m_unStringLimit - m_strBackupString.length() );
	}

	m_strCurrentString = m_strBackupString;

	m_unWritePos = m_unCaretPos;

	m_unSelectStartPos = m_unSelectEndPos = m_unWritePos;
}

//-----------------------------------------------------------------------------------------------------
// GMIMEPACK::CutString()
void GMIMEPACK::CutString()
{
	if( m_unSelectStartPos != m_unSelectEndPos )
	{
		m_strCopyString.clear();

		char* pTempStr = new char[ m_unSelectEndPos - m_unSelectStartPos + 1 ];

		m_strBackupString.copy( pTempStr, m_unSelectEndPos - m_unSelectStartPos, m_unSelectStartPos );

		pTempStr[ m_unSelectEndPos - m_unSelectStartPos ] = 0;

		m_strCopyString = pTempStr;

		delete [] pTempStr;

		m_strBackupString.erase( m_strBackupString.begin() + m_unSelectStartPos, m_strBackupString.begin() + m_unSelectEndPos );

		m_strCurrentString = m_strBackupString;

		if( m_unWritePos > m_unCaretPos )
		{
			m_unWritePos = m_unCaretPos;
		}
		else
		{
			m_unCaretPos = m_unWritePos;
		}

		m_unSelectStartPos = m_unSelectEndPos = m_unWritePos;

        HGLOBAL hMem = GlobalAlloc( GHND, m_strCopyString.length() + 1 );
        
        char* pClipString = (char*)GlobalLock( hMem );
        memcpy( pClipString, m_strCopyString.c_str(), m_strCopyString.length() );
        GlobalUnlock( hMem );

        if( OpenClipboard( m_hWnd ) )
        {

            EmptyClipboard();
            SetClipboardData( CF_TEXT, hMem );
            CloseClipboard();
        }
	}
}

//-----------------------------------------------------------------------------------------------------
// GMIMEPACK::GetCurrentStringWidth()
UINT GMIMEPACK::GetCurrentStringWidth( UINT unCurrentPos )
{
	SIZE  Size;
	HDC   hDC = GetDC( m_hWnd );

	GetTextExtentPoint32( hDC, m_strCurrentString.c_str(), unCurrentPos, &Size );

	ReleaseDC( m_hWnd, hDC );

	return Size.cx;
}

//-----------------------------------------------------------------------------------------------------
// GMIMEPACK::SelectCandidateString()
void GMIMEPACK::SelectCandidateString( UINT unIdx )
{
	if( m_vtCandidateList.empty() )
	{
		// get ime handle
		m_hImc			= ImmGetContext( m_hWnd );

		UINT unTempIdx = m_unCandidatePageStartIdx + m_unCandidateOnceRetrieveCount;

		if( unTempIdx > m_unCandidateCount )
			unTempIdx = m_unCandidateCount;

		ImmNotifyIME( m_hImc, NI_SELECTCANDIDATESTR, 0, m_unCandidatePageStartIdx + unIdx );

		ImmReleaseContext( m_hWnd, m_hImc );
	}
}