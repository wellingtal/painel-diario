//-----------------------------------------------------------------------------------------------------
// File Name: GMIMEPACK.h ( Zergra's IME )
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// Date: 2003. 12. 23. ( Tue )
//-----------------------------------------------------------------------------------------------------
#ifndef __GM_IME_PACK_CLASS__
#define __GM_IME_PACK_CLASS__

#pragma comment(lib, "imm32.lib")
#include<string>
#include<vector>
using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
// class IMEPACK
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
class GMIMEPACK
{
//-- member variables --//
private:
                HWND        m_hWnd;                         // window handle
                HIMC        m_hImc;                         // input context handle

                //-----------------------------------------------------------------------------------------------------
                // concerned with basic IME input
                string      m_strCurrentString;             // string which is combined with composing string in composition window
                string      m_strCurrentComposingString;    // string which is composing in composition window
                string      m_strBackupString;              // string which is not combined with composing string
                string      m_strAttribute;                 // current string's attribute buffer ( 0: not selected, 1: current selecting, 2: selected )
        vector<DWORD32>     m_vtClause;                     // current string's clause vector;
         
                UINT        m_unStringLimit;                // limit length of string

                UINT        m_unComposingLength;            // length of composing string in compositino window

                UINT        m_unCaretPos;                   // caret position
                UINT        m_unWritePos;                   // writing position in backup string

                bool        m_bInsert;                      // true: insert mode, false: overwrite mode
                bool        m_bFullWidth;                   // current character width( true: 2 bytes, false: 1 byte )

                bool        m_bEnglishOnly;                 // allow only english input
                bool        m_bNumeralOnly;                 // allow only number input
                bool        m_bHalfWidthOnly;               // allow only half width

                UINT        m_unPrimaryLanguage;            // primary language identifier
                UINT        m_unSubLanguage;                // sub language identifier
                string      m_strLanguageName;              // language name

        LPCOMPOSITIONFORM   m_lpCompositionForm;            // contains style and position information for a composition window

                bool        m_bUseCustomImeWindow;          // will use custom composition window and candidate window?

                  //-----------------------------------------------------------------------------------------------------
                  // concerned with copy & paste, cut
        static  string      m_strCopyString;                // custom string clipboard ( ¡ØCAUTION¡Ø THIS IS STATIC MEMBER!!! Every IME Instance share this string )

                UINT        m_unSelectStartPos;             // caret position where selection start ( smaller than end pos )
                UINT        m_unSelectEndPos;               // caret position where selection end   ( larger than start pos )
                bool        m_bShiftPushed;                 // is shift key pushing?
                bool        m_bCtrlPushed;                  // is control key pushing?
                bool        m_bTextSelecting;               // is text selecting?

                  //-----------------------------------------------------------------------------------------------------
                  // concerned with candidate process
        vector<string>      m_vtCandidateList;              // candidate list
                UINT        m_unCandidateOnceRetrieveCount; // number of candidate elements that retrieve at once
                UINT        m_unCandidateCount;             // number of candidate elements
                UINT        m_unCandidatePageStartIdx;      // index of first element in current candidate list buffer
                UINT        m_unCandidateSelectedIdx;       // index of candidate element that selected in total candidate list count
                UINT        m_unCandidateCurrentSelectedIdx;// index of candidate element that selected in current candidate list buffer

        LPCANDIDATEFORM     m_lpCandidateForm;              // contains position information for a candidate window

//-- constructor, destructor --//
public:                    
                            GMIMEPACK();
                            GMIMEPACK( HWND hWnd, UINT unStringLimit = 128 );
                            ~GMIMEPACK();

//-- public member functions --//
public:
                //-----------------------------------------------------------------------------------------------------
                // initialize
                void		InitIme( HWND hMainWnd, UINT unStringLimit );

                //-----------------------------------------------------------------------------------------------------
                // hook IME message
                bool        GetIMEMessage( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

                //-----------------------------------------------------------------------------------------------------
                // open/close ime
		        void		OpenIme();
		        void		CloseIme();
        
//-- priave member functions --// 
private:
                //-----------------------------------------------------------------------------------------------------
                // process module for each message 
                bool        OnKeyDown(WPARAM wParam, LPARAM lParam);                // WM_KEYDOWN
                bool        OnChar(WPARAM wParam, LPARAM lParam);                   // WM_CHAR
                bool        OnInputLangChange(WPARAM wParam, LPARAM lParam);		// WM_INPULANGCHANGE
                bool        OnImeStartComposition(WPARAM wParam, LPARAM lParam);	// WM_IME_STARTCOMPOSITION
                bool        OnImeEndComposition(WPARAM wParam, LPARAM lParam);		// WM_IME_ENDCOMPOSITION
                bool        OnImeComposition(WPARAM wParam, LPARAM lParam);			// WM_IME_COMPOSITION
                bool        OnImeNotify(WPARAM wParam, LPARAM lParam);			    // WM_IME_NOTIFY

                void        OnGcsCompStr( LPARAM lParam );                          // WM_IME_COMPOSITION, GCS_COMPSTR
                void        OnGcsResultStr( LPARAM lParam );                        // WM_IME_COMPOSITION, GCS_RESULTSTR
                                                                                                      
//-- support functions --//
public:
                //-----------------------------------------------------------------------------------------------------
                // get something
inline  const   char*       GetString()                                         { return m_strCurrentString.c_str(); }
inline  const   string      GetSubString( unsigned int unStartPos, unsigned int unEndPos )      { return m_strCurrentString.substr( unStartPos, unEndPos ); }
inline  const   UINT        GetStringLength()                                   { return static_cast<UINT>( m_strCurrentString.length() ); }
inline  const   char*       GetBackupString()                                   { return m_strBackupString.c_str(); }
inline  const   UINT        GetBackupStrinLength()                              { return static_cast<UINT>( m_strBackupString.length() ); }
inline  const   char*       GetCurrentComposingString()                         { return m_strCurrentComposingString.c_str(); }
inline  const   UINT        GetCurrentComposingStringLength()                   { return static_cast<UINT>( m_strCurrentComposingString.length() ); }
inline  const   UINT        GetStringLimit()                                    { return m_unStringLimit; }
inline  const   UINT        GetCaretPos()                                       { return m_unCaretPos; }
inline  const   UINT        GetWritePos()                                       { return m_unWritePos; }

inline  const   char*       GetAttribute()                                      { return m_strAttribute.c_str(); }
inline  const   vector<DWORD32>&  GetClause()                                   { return m_vtClause; }

inline  const   bool        GetInsertMode()                                     { return m_bInsert; }

inline          bool        GetIsNumeralOnly()                                  { return m_bNumeralOnly; }
inline          bool        GetIsHalfWidthOnly()                                { return m_bHalfWidthOnly; }

inline  const   UINT        GetPrimaryLanguageID()                              { return m_unPrimaryLanguage; }
inline  const   UINT        GetSubLanguageID()                                  { return m_unSubLanguage; }
inline  const   char*       GetLanguageName()                                   { return m_strLanguageName.c_str(); }

inline  const   vector<string>&   GetCandidateList()                            { return m_vtCandidateList; }
inline  const   UINT        GetCandidateOnceRetrieveCount()                     { return m_unCandidateOnceRetrieveCount; }
inline  const   UINT        GetCandidateCount()                                 { return m_unCandidateCount; }
inline  const   UINT        GetCandidateSelectedIdx()                           { return m_unCandidateSelectedIdx; }
inline  const   UINT        GetCandidateCurrentSelectedIdx()                    { return m_unCandidateCurrentSelectedIdx; }

inline  const   bool        GetUseCustomWindow()                                { return m_bUseCustomImeWindow; }

inline  const   char*       GetCopiedString()                                   { return m_strCopyString.c_str(); }
inline  const   UINT        GetCopiedStringLength()                             { return static_cast<UINT>( m_strCopyString.length() ); }
inline  const   bool        GetTextSelecting()                                  { return m_bTextSelecting; }
inline  const   UINT        GetSelectStartPos()                                 { return m_unSelectStartPos; }
inline  const   UINT        GetSelectEndPos()                                   { return m_unSelectEndPos; }

                  //-----------------------------------------------------------------------------------------------------
                  // set something
                void        SetString( char* str );
inline          void        SetCaretPos( UINT unCaretPos )                      { m_unCaretPos = m_unWritePos = unCaretPos; }
inline          void        SetInsertMode( bool bInsert )                       { m_bInsert = bInsert; }

                void        SetStringLimit( UINT unStringLimit );

                void        SetEnglishOnly( bool bOnly );
inline          void        SetNumeralOnly( bool bOnly )                        { m_bNumeralOnly = bOnly; }//SetEnglishOnly( bOnly ); }
inline          void        SetHalfWidthOnly( bool bOnly )                      { m_bHalfWidthOnly = bOnly; SetEnglishOnly( bOnly ); }

                void        SetStatusWindowPosition( POINT ptPos );
                void        SetCompositionWindowPosition( POINT ptPos );
                void        SetCandidatewindowPosition( POINT ptPos );

inline          void        SetCandidateOnceRetrieveCount( UINT unCount )       { m_unCandidateOnceRetrieveCount = unCount; }
                void        SetCandidateListPrev();
                void        SetCandidateListNext();

inline          void        SetUseCustomWindow( bool bUse )                     { m_bUseCustomImeWindow = bUse; }
                //-----------------------------------------------------------------------------------------------------
                // sub function
                void        ClearString();

                void        CopyString();
                void        PasteString();
                void        CutString();

                UINT        GetCurrentStringWidth( UINT unCurrentPos );

                void        SelectCandidateString( UINT unIdx );

				bool		PushbackThiaString( WPARAM wParam, LPARAM lParam ) ;
				bool		InsertThiaString( WPARAM wParam, LPARAM lParam ) ;
};

#endif //__GM_IME_PACK_CLASS__