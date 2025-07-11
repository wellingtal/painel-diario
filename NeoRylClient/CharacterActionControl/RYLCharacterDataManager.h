//--------------------------------------------------------------------------------------------------------------------------
// File Name: RYLCharacterDataManager.h
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// File Desciption: contain and manage characters's data
//
// Date: 2004. 3. 26. ( Fri )
//--------------------------------------------------------------------------------------------------------------------------

#ifndef __ZRYL_CHARACTER_DATA_MANAGER_H__
#define __ZRYL_CHARACTER_DATA_MANAGER_H__

//--------------------------------------------------------------------------------------------------------------------------
// Headers
//--------------------------------------------------------------------------------------------------------------------------
#include <list>
#include <string>
using namespace std;

//--------------------------------------------------------------------------------------------------------------------------
// Enums, Structs
//--------------------------------------------------------------------------------------------------------------------------
struct CharDataNode
{
	unsigned long		m_ulCharID;             // character ID
    unsigned long       m_ulNodeID;             // node ID in list

	string              m_strName;      	    // name
	char				m_cSex;				    // sex
	char				m_cHair;		        // hair style
	char 				m_cFace;			    // face style
	char				m_cNation;		        // Carterant : 0, Mercadia : 1
    char                m_cRace;                // Human : 0, Akhan : 1
	unsigned short		m_usClass;			    // class( job? )

	unsigned long		m_ulFame;			    // famous
	unsigned long		m_ulMileage;		    // mileage
	unsigned long		m_ulGuildID;	        // guild id
	unsigned long		m_ulPartyID;		    // party id
	unsigned long		m_ulTempGID;			// 임시 길드 정보.
    unsigned long       m_ulTeamID;             // team id

	char				m_cLevel;			    // level
	long                m_lCurrHP;		        // current HP
	long                m_lCurrMP;		        // current MP	
    unsigned long       m_ulMaxHP;              // max hp
    unsigned long       m_ulMaxMP;              // max mp

    bool                m_bPeaceMode;           // peace mode setting
    int                 m_nChrPattern;          // pattern
    string              m_strChatMessage;       // stall chat message

    bool                m_bIsUsingNode;         // using state

    unsigned long       m_ulTargetCharID;       // target character id

    unsigned long       m_ulSummonMasterCharID;  // summon matser id

    string              m_strStreetStallName;   // stall name

    unsigned int	    m_dwPlayTime;		  // PlayTime

    CharDataNode() 
	:	m_ulCharID(0), m_ulNodeID(0xFFFFFFFF), m_cSex(0), m_cHair(0), m_cFace(0), m_cNation(0), m_cRace(0), m_usClass(0), 
		m_ulFame(0), m_ulMileage(0), m_ulGuildID(0), m_ulPartyID(0), m_ulTeamID(0), m_ulTempGID(0),
		m_cLevel(0), m_lCurrHP(0), m_lCurrMP(0), m_ulMaxHP(0), m_ulMaxMP(0), m_bPeaceMode(0), m_nChrPattern(0), 
		m_bIsUsingNode(false), m_ulTargetCharID(0xFFFFFFFF), m_ulSummonMasterCharID(0xFFFFFFFF), m_dwPlayTime(0)
    {
        m_strName.reserve( 128 );
        m_strName.clear();

        m_strChatMessage.reserve( 128 );
        m_strChatMessage.clear();

        m_strStreetStallName.reserve( 128 );
        m_strStreetStallName.clear();
    }

    ~CharDataNode()
    {
        m_strName.clear();
        m_strChatMessage.clear();
        m_strStreetStallName.clear();
    }

    void operator=( const CharDataNode& node )
    {
        m_ulCharID              = node.m_ulCharID;
        m_ulNodeID              = node.m_ulNodeID;                                

        m_strName               = node.m_strName;
        m_cSex                  = node.m_cSex;
        m_cHair                 = node.m_cHair;
        m_cFace                 = node.m_cFace;
        m_cNation               = node.m_cNation;
        m_cRace                 = node.m_cRace;
        m_usClass               = node.m_usClass;
                                    
        m_ulFame                = node.m_ulFame;
        m_ulMileage             = node.m_ulMileage;
        m_ulGuildID             = node.m_ulGuildID;
        m_ulPartyID             = node.m_ulPartyID;
		m_ulTempGID				= node.m_ulTempGID;
        m_ulTeamID              = node.m_ulTeamID;

        m_cLevel                = node.m_cLevel;                                
        m_lCurrHP               = node.m_lCurrHP;
        m_lCurrMP               = node.m_lCurrMP;                                
        m_ulMaxHP               = node.m_ulMaxHP;
        m_ulMaxMP               = node.m_ulMaxMP;
                                    
        m_bPeaceMode            = node.m_bPeaceMode;
        m_nChrPattern           = node.m_nChrPattern;
        m_strChatMessage        = node.m_strChatMessage;

        m_bIsUsingNode          = node.m_bIsUsingNode;

        m_ulTargetCharID        = node.m_ulTargetCharID;

        m_ulSummonMasterCharID  = node.m_ulSummonMasterCharID;

        m_strStreetStallName    = node.m_strStreetStallName;
    }
};

//--------------------------------------------------------------------------------------------------------------------------
// Consts
//--------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------
// Classes
//--------------------------------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
// class RYLCharacterDataManager
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
class RYLCharacterDataManager
{
private:                
            CharDataNode*               m_pSelfData;

            list<CharDataNode*>         m_lstCharData;
            unsigned long               m_ulCharCount;
            unsigned int                m_unMaxCash;

public:
            //--------------------------------------------------------------------------------------------------------------------------
            // Constructor, Destructor
            RYLCharacterDataManager( unsigned int unMaxCash = 30 );
            ~RYLCharacterDataManager();              

            //--------------------------------------------------------------------------------------------------------------------------
            // Init Manager
            void                        InitCharDataManager( unsigned int unCharPoolMax );
			
            //--------------------------------------------------------------------------------------------------------------------------
            // character list dealing functions
            void                        SetSelfData( CharDataNode* pNode )                      { m_pSelfData = pNode; }
            CharDataNode*               GetSelfData()                                           { return m_pSelfData; }

            CharDataNode*               AddCharData( CharDataNode* node );
            CharDataNode*               GetCharData( unsigned long ulCharID );
			CharDataNode*				GetCharData( char* szName );

			CharDataNode*				InitCharData( unsigned long ulCharID, char cRace ) ;

            void                        DeleteAllCharData();
            void                        DeleteCharData( unsigned long ulCharID );

    inline  void                        SetMaxCash( unsigned int unMaxCash )                    { m_unMaxCash = unMaxCash; }
    inline  unsigned int                GetMaxCash()                                            { return m_unMaxCash; }

            //--------------------------------------------------------------------------------------------------------------------------
            // 각종 data 변경
            void                        SwapPartyID( unsigned long ulOldPartyID, unsigned long ulNewPartyID );

            //--------------------------------------------------------------------------------------------------------------------------
            // singletone
private:
    static  RYLCharacterDataManager*   m_pInstance;

public:
    static  RYLCharacterDataManager*   Instance()
    {
        if( !m_pInstance )
        {
            m_pInstance = new RYLCharacterDataManager;
        }

        return m_pInstance;
    }
};

#endif // __ZRYL_CHARACTER_DATA_MANAGER_H__