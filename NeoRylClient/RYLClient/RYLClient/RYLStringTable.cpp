#include <winsock2.h>
#include "RYLStringTable.h"
#include "Windows.h"
#include "ScriptEngine.h"
#include "RYLNetworkData.h"
#include <string>
#include <d3d8types.h>

#include <Creature/Character/CharacterClass.h>
#include "GMMemory.h"

#define		THIAFONT_SIZE 7

unsigned long	CRYLStringTable::m_dwStringCount = 0 ;
char			CRYLStringTable::m_strString[ 8192 ][ 1024 ] ;
unsigned long	CRYLStringTable::m_dwTextSize[ 129 ] ;

GameRYL::ServiceNation	CRYLStringTable::m_eInternationalCode = GameRYL::KOREA;		// 국가 코드
GameRYL::ServerType		CRYLStringTable::m_eServerType = GameRYL::SERVER_TEST;		// 서버 타입
char					CRYLStringTable::m_szGameGuardCode[MAX_GAMEGUARD_CODE_LEN];	// 게임 가드 초기화 코드

static void		ScriptErrorMessage( const char *msg )
{
	MessageBox(NULL, msg, "Script Error", MB_OK);
}

static void		AddString( const char *strMessage )
{
	if ( strMessage )
	{
		strcpy( CRYLStringTable::m_strString[ CRYLStringTable::m_dwStringCount ], strMessage ) ;
		CRYLStringTable::m_dwStringCount++;
	}
}

static void AddStringCount( int nCount, const char *strMessage )
{
	if ( strMessage )
	{
		strcpy( CRYLStringTable::m_strString[ nCount ], strMessage ) ;
		CRYLStringTable::m_dwStringCount = nCount + 1 ;
	}
}

static void RylNation(int nInternationalCode, int nServerType, const char* szRYLGameGuardCode)
{
	CRYLStringTable::m_eInternationalCode = static_cast<GameRYL::ServiceNation>(nInternationalCode);
	CRYLStringTable::m_eServerType = static_cast<GameRYL::ServerType>(nServerType);

    _snprintf(CRYLStringTable::m_szGameGuardCode, 
        CRYLStringTable::MAX_GAMEGUARD_CODE_LEN, szRYLGameGuardCode);

	// 강제로 RYLYouxi로 셋팅
	CRYLStringTable::m_eInternationalCode = GameRYL::GLOBAL;
	CRYLStringTable::m_eServerType = GameRYL::SERVER_REGULAR;
	strcpy(CRYLStringTable::m_szGameGuardCode, "RYLYouxi");

    CRYLStringTable::m_szGameGuardCode[CRYLStringTable::MAX_GAMEGUARD_CODE_LEN - 1] = 0;        
}

bool	CRYLStringTable::Load( const char *strScriptFile, const char *strMacroFile )
{
	m_dwStringCount = 0 ;

	_SE_SetMessageFunction( ScriptErrorMessage ) ;
	SCRIPT Script = _SE_Create( strScriptFile ) ;

	if ( !Script ) 
		return false ;

	_SE_RegisterFunction( Script, AddString, T_VOID, "AddString", T_STRING, 0 ) ;
	_SE_RegisterFunction( Script, AddStringCount, T_VOID, "AddString", T_INT, T_STRING, 0 ) ;
    _SE_RegisterFunction( Script, RylNation, T_VOID, "RylNation", T_INT, T_INT, T_STRING, 0 ) ;

	_SE_Execute( Script ) ;
	_SE_Destroy( Script ) ;


	// edith 2009.05.30 매크로 파일을 로드한다.
	// CRYLStringTable::m_strString[581] ~ CRYLStringTable::m_strString[589]
	FILE* fp = fopen(strMacroFile, "rt");

	if(fp == NULL)
		return true;

	char strTemp[256];
	for(int i = 0; i < 9; ++i)
	{
		ZeroMemory(strTemp, 256);
		fgets(strTemp, 256, fp);
		if(strlen(strTemp) == 0)
			break;

		if(strncmp(strTemp, "KEY", 3) != 0)
			continue;

		strTemp[255] = 0;
		int len = strlen(strTemp);
		int start = 0;
		int end = 0;
		for(int j = 4; j < len; ++j)
		{
			if(strTemp[j] == '=')
				continue;
			else if(strTemp[j] == ' ')
				continue;

			if(start == 0)
				start = j;

			// 줄바꿈 문자 0으로 초기화
			if(strTemp[j] == '\n')
			{
				strTemp[j] = 0;
				end = j;
				break;
			}
		}

		ZeroMemory(CRYLStringTable::m_strString[581+i], 1024);
		strncpy(CRYLStringTable::m_strString[581+i], &strTemp[start], end-start);
	}

	fclose(fp);
	return true ;
}

VOID	CRYLStringTable::CreateTextSize()
{
	m_dwTextSize[0] = 0;		m_dwTextSize[1] = 6;		m_dwTextSize[2] = 6;		m_dwTextSize[3] = 6;
	m_dwTextSize[4] = 0;		m_dwTextSize[5] = 0;		m_dwTextSize[6] = 0;		m_dwTextSize[7] = 0;
	m_dwTextSize[8] = 0;		m_dwTextSize[9] = 0;		m_dwTextSize[10] = 0;		m_dwTextSize[11] = 0;
	m_dwTextSize[12] = 0;		m_dwTextSize[13] = 0;		m_dwTextSize[14] = 0;		m_dwTextSize[15] = 0;
	m_dwTextSize[16] = 0;		m_dwTextSize[17] = 0;		m_dwTextSize[18] = 0;		m_dwTextSize[19] = 0;
	m_dwTextSize[20] = 0;		m_dwTextSize[21] = 0;		m_dwTextSize[22] = 0;		m_dwTextSize[23] = 0;
	m_dwTextSize[24] = 0;		m_dwTextSize[25] = 0;		m_dwTextSize[26] = 0;		m_dwTextSize[27] = 0;
	m_dwTextSize[28] = 0;		m_dwTextSize[29] = 0;		m_dwTextSize[30] = 0;		m_dwTextSize[31] = 0;
	m_dwTextSize[32] = 4;		m_dwTextSize[33] = 4;		m_dwTextSize[34] = 4;		m_dwTextSize[35] = 6;
	m_dwTextSize[36] = 6;		m_dwTextSize[37] = 10;		m_dwTextSize[38] = 8;		m_dwTextSize[39] = 4;
	m_dwTextSize[40] = 5;		m_dwTextSize[41] = 5;		m_dwTextSize[42] = 6;		m_dwTextSize[43] = 6;
	m_dwTextSize[44] = 4;		m_dwTextSize[45] = 6;		m_dwTextSize[46] = 4;		m_dwTextSize[47] = 6;
	m_dwTextSize[48] = 6;		m_dwTextSize[49] = 6;		m_dwTextSize[50] = 6;		m_dwTextSize[51] = 6;
	m_dwTextSize[52] = 6;		m_dwTextSize[53] = 6;		m_dwTextSize[54] = 6;		m_dwTextSize[55] = 6;
	m_dwTextSize[56] = 6;		m_dwTextSize[57] = 6;		m_dwTextSize[58] = 4;		m_dwTextSize[59] = 4;
	m_dwTextSize[60] = 8;		m_dwTextSize[61] = 6;		m_dwTextSize[62] = 8;		m_dwTextSize[63] = 6;
	m_dwTextSize[64] = 12;		m_dwTextSize[65] = 8;		m_dwTextSize[66] = 8;		m_dwTextSize[67] = 9;
	m_dwTextSize[68] = 8;		m_dwTextSize[69] = 8;		m_dwTextSize[70] = 7;		m_dwTextSize[71] = 9;
	m_dwTextSize[72] = 8;		m_dwTextSize[73] = 3;		m_dwTextSize[74] = 4;		m_dwTextSize[75] = 8;
	m_dwTextSize[76] = 7;		m_dwTextSize[77] = 11;		m_dwTextSize[78] = 9;		m_dwTextSize[79] = 9;
	m_dwTextSize[80] = 8;		m_dwTextSize[81] = 9;		m_dwTextSize[82] = 8;		m_dwTextSize[83] = 8;
	m_dwTextSize[84] = 7;		m_dwTextSize[85] = 8;		m_dwTextSize[86] = 8;		m_dwTextSize[87] = 10;
	m_dwTextSize[88] = 8;		m_dwTextSize[89] = 8;		m_dwTextSize[90] = 8;		m_dwTextSize[91] = 6;
	m_dwTextSize[92] = 11;		m_dwTextSize[93] = 6;		m_dwTextSize[94] = 6;		m_dwTextSize[95] = 6;
	m_dwTextSize[96] = 4;		m_dwTextSize[97] = 7;		m_dwTextSize[98] = 7;		m_dwTextSize[99] = 7;
	m_dwTextSize[100] = 7;		m_dwTextSize[101] = 7;		m_dwTextSize[102] = 3;		m_dwTextSize[103] = 7;
	m_dwTextSize[104] = 7;		m_dwTextSize[105] = 3;		m_dwTextSize[106] = 3;		m_dwTextSize[107] = 6;
	m_dwTextSize[108] = 3;		m_dwTextSize[109] = 11;		m_dwTextSize[110] = 7;		m_dwTextSize[111] = 7;
	m_dwTextSize[112] = 7;		m_dwTextSize[113] = 7;		m_dwTextSize[114] = 4;		m_dwTextSize[115] = 7;
	m_dwTextSize[116] = 3;		m_dwTextSize[117] = 7;		m_dwTextSize[118] = 6;		m_dwTextSize[119] = 10;
	m_dwTextSize[120] = 7;		m_dwTextSize[121] = 7;		m_dwTextSize[122] = 7;		m_dwTextSize[123] = 6;
	m_dwTextSize[124] = 6;		m_dwTextSize[125] = 6;		m_dwTextSize[126] = 9;		m_dwTextSize[127] = 6;
	m_dwTextSize[128] = 12;
}

char *CRYLStringTable::MakeWordBreak( const char *strString, unsigned long dwWidth )
{
	static char	strWordBreak[ 4096 ];
	WCHAR		strWideString[ 4096 ] = {0, };
	int			nBufferSize = 4096 ;
	int			nWideStringLen ;
	long		i ;

	memset(strWordBreak, 0, 4096);

	MultiByteToWideChar( CP_ACP, 0, strString, strlen( strString ) + 1, strWideString, nBufferSize ) ;
	nWideStringLen = wcslen( strWideString ) ;

	unsigned long dwStringWidth = 0 ;
	unsigned long dwSrcPos		= 0 ;
	unsigned long dwDstPos		= 0 ;

	WCHAR	strWideBreak[ 256 ] ;
	char	strBreak[ 256 ] ;

	for ( i = 0; i < nWideStringLen ; i ++ )
	{
		if ( ( dwSrcPos <= ( unsigned long )i ) && ( i < 4096 ) )
		{
			if ( strWideString[ i ] == 0x005c )		// "\"
			{
				ZeroMemory( strWideBreak, sizeof( strWideBreak ) ) ;
				wcsncpy( strWideBreak, &strWideString[ dwSrcPos ], i - dwSrcPos ) ;

				if ( strWideString[ i + 1 ] == 0x005c ) 
					i += 2 ; 
				else 
					++ i ;

				if ( strWideString[ i ] < 128 )
					dwStringWidth = m_dwTextSize[ strWideString[ i ] ] ;
				else
				{
					if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::THAILAND )
					{
						BYTE byString = (BYTE)strString[ i ] ;

						if ( byString >= 0xa1 && byString <= 0xfb )
							dwStringWidth += THIAFONT_SIZE ;
					}
					else
					{
						dwStringWidth = m_dwTextSize[ 128 ] ;
					}
				}

				dwSrcPos = i ;
				WideCharToMultiByte( CP_ACP, 0, strWideBreak, wcslen( strWideBreak ) + 1, strBreak, 256, NULL, NULL ) ;

				strcpy( &strWordBreak[ dwDstPos ], strBreak ) ;
				dwDstPos = strlen( strWordBreak ) ;
				strWordBreak[ dwDstPos ] = '\\' ;	
				dwDstPos ++ ;

				continue ;
			} 
			else if ( strWideString[ i ] < 128 )
			{
				dwStringWidth += m_dwTextSize[ strWideString[ i ] ] ;
			} 
			else
			{
				if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::THAILAND )
				{
					BYTE byString = (BYTE)strString[ i ] ;

					if ( byString >= 0xa1 && byString <= 0xfb )
						dwStringWidth += THIAFONT_SIZE ;
				}
				else
				{
					dwStringWidth += m_dwTextSize[ 128 ] ;
				}
			}

			if ( dwStringWidth > dwWidth )
			{
				i -- ;
				ZeroMemory( strWideBreak, sizeof( strWideBreak ) ) ;

				if ( strWideString[ i ] == ' ' )
				{
					i -- ;
					wcsncpy( strWideBreak, &strWideString[ dwSrcPos ], i - dwSrcPos + 1 ) ;
					i += 2 ;

					if ( strWideString[ i ] < 128 )
						dwStringWidth = m_dwTextSize[ strWideString[ i ] ] ;
					else
					{
						if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::THAILAND )
						{
							BYTE byString = (BYTE)strString[ i ] ;

							if ( byString >= 0xa1 && byString <= 0xfb )
								dwStringWidth += THIAFONT_SIZE ;
						}
						else
						{
							dwStringWidth = m_dwTextSize[ 128 ] ;
						}
					}
				} 
				else
				{
					wcsncpy( strWideBreak, &strWideString[ dwSrcPos ], i - dwSrcPos + 1 ) ;
					i ++ ;

					if ( strWideString[ i ] < 128 )
						dwStringWidth = m_dwTextSize[ strWideString[ i ] ] ;
					else
					{
						if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::THAILAND )
						{
							BYTE byString = (BYTE)strString[ i ] ;

							if ( byString >= 0xa1 && byString <= 0xfb )
								dwStringWidth += THIAFONT_SIZE ;
						}
						else
						{
							dwStringWidth = m_dwTextSize[ 128 ] ;
						}
					}
				}

				dwSrcPos = i ;
				WideCharToMultiByte( CP_ACP, 0, strWideBreak, wcslen( strWideBreak ) + 1, strBreak, 256, NULL, NULL ) ;

				strcpy( &strWordBreak[ dwDstPos ], strBreak ) ;
				dwDstPos = strlen( strWordBreak ) ;
				strWordBreak[ dwDstPos ] = '\\' ;	
				dwDstPos ++ ;
			}
		} 
		else
		{
			break ;
		}
	}

	if ( dwSrcPos < ( unsigned long )nWideStringLen )
	{
		if ( ( nWideStringLen < 4096 ) )
		{
			ZeroMemory( strWideBreak, sizeof( strWideBreak ) ) ;
			if ( strWideString[ dwSrcPos ] == ' ' )
			{
				if ( dwSrcPos + 1 < ( unsigned long )nWideStringLen )
				{
					dwSrcPos ++ ;
				}
				wcsncpy( strWideBreak, &strWideString[ dwSrcPos ], nWideStringLen - dwSrcPos + 1 ) ;
			} 
			else
			{
				wcsncpy( strWideBreak, &strWideString[ dwSrcPos ], nWideStringLen - dwSrcPos + 1 ) ;
			}

			WideCharToMultiByte( CP_ACP, 0, strWideBreak, wcslen( strWideBreak ) + 1, strBreak, 256, NULL, NULL ) ;
			strcpy( &strWordBreak[ dwDstPos ], strBreak ) ;
		}
	}

	return strWordBreak ;
}

char*		CRYLStringTable::MakePrintGold(unsigned long dwGold)
{
	static char strGold[16];
	char		strTemp[16];
	sprintf(strTemp, "%u", dwGold);
	ZeroMemory(strGold, sizeof(strGold));

	unsigned long dwLen = strlen(strTemp);
	unsigned long dwTemp1 = dwLen / 3;
	unsigned long dwTemp2 = dwLen % 3;

	if (!dwTemp2 && dwTemp1) dwTemp1--;

	unsigned long dwCount = 0, dwDotCount = 0;

	for (unsigned long i = dwLen + dwTemp1; i > 0; i--)
	{
		dwDotCount++;
		if ((dwDotCount % 4) == 0)
		{
			dwDotCount = 0;
			strGold[i - 1] = ',';
		} 
		else
		{
			strGold[i - 1] = strTemp[dwLen - dwCount - 1];
			dwCount++;
		}
	}

	return strGold;
}

char*		CRYLStringTable::MakePrintGoldAbleMinus(long lGold)
{
	if (0 <= lGold)
	{
		return MakePrintGold(static_cast<unsigned long>(lGold));
	}

	static char strGold[16];
	char		strTemp[16];
	sprintf(strTemp, "%u", abs(lGold));
	ZeroMemory(strGold, sizeof(strGold));

	unsigned long dwLen = strlen(strTemp);
	unsigned long dwTemp1 = dwLen / 3;
	unsigned long dwTemp2 = dwLen % 3;

	if (!dwTemp2 && dwTemp1) dwTemp1--;

	unsigned long dwCount = 0, dwDotCount = 0;

	for (unsigned long i = dwLen + dwTemp1; i > 0; i--)
	{
		dwDotCount++;
		if ((dwDotCount % 4) == 0)
		{
			dwDotCount = 0;
			strGold[i] = ',';
		} 
		else
		{
			strGold[i] = strTemp[dwLen - dwCount - 1];
			dwCount++;
		}
	}

	strGold[0] = '-';

	return strGold;
}

char*	
//문자열의 길이가 dwWidth 이상인지 체크해서 넘어가면 넘어간 부분에 대해서 ... 처리한다.
CRYLStringTable::CheckStringWidth( LPCTSTR lpStr, unsigned long dwWidth ) 
{
	static char		strResult[ 1024 ] ;
	char			strEtc[ 5 ] = "...\0";
	int				nWideStringLen, nStrPoint ;
	DWORD			dwlenEtc ;
	DWORD			dwStringWidth = 0 ;
	INT				nBufferSize = 1024;
	static WCHAR	strWideString[ 1024] ;

	memset( strResult, 0, sizeof( char ) * 1024 );

	strncpy( strResult, lpStr, 1024 );
	dwlenEtc = GetStringWidth( strEtc );



	MultiByteToWideChar( CP_ACP, 0, lpStr, strlen( lpStr ) + 1, strWideString, nBufferSize ) ;
	nWideStringLen = wcslen( strWideString ) ;

	nStrPoint = 0 ;
	for ( INT i = 0; i < nWideStringLen ; i ++ )
	{
		// 태국어 
		if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::THAILAND )
		{
			if ( strWideString[ i ] < 128 )
				dwStringWidth += m_dwTextSize[ strWideString[ i ] ] ;
			else 
				dwStringWidth += 6 ;
		}
		else
		{
			if ( strWideString[ i ] < 128 )
				dwStringWidth += m_dwTextSize[ strWideString[ i ] ] ;
			else
				dwStringWidth += m_dwTextSize[ 128 ] ;
		}
		if( dwStringWidth + dwlenEtc > dwWidth )
		{
			if( nStrPoint == 0 )
				nStrPoint = max( (i-1), 0 ) ;
			if( dwStringWidth  > dwWidth )
			{
				strcpy( &(strResult[nStrPoint]), strEtc );
				break;
			}
		}
	}

	return strResult;
}


INT		CRYLStringTable::GetStringWidth( LPCTSTR lpStr )
{
	DWORD			dwStringWidth = 0 ;
	INT				nWideStringLen ;
	INT				nBufferSize = 16000 ;
	static WCHAR	strWideString[ 16000 ] ;

	MultiByteToWideChar( CP_ACP, 0, lpStr, strlen( lpStr ) + 1, strWideString, nBufferSize ) ;
	nWideStringLen = wcslen( strWideString ) ;

	for ( INT i = 0; i < nWideStringLen ; i ++ )
	{
/*
		// 태국어 
		if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::THAILAND )
		{
			if ( strWideString[ i ] < 128 )
				dwStringWidth += m_dwTextSize[ strWideString[ i ] ] ;
			else 
				dwStringWidth += 6 ;
		}
		else
		{
			if ( strWideString[ i ] < 128 )
				dwStringWidth += m_dwTextSize[ strWideString[ i ] ] ;
			else
				dwStringWidth += m_dwTextSize[ 128 ] ;
		}
*/
		{
			if ( strWideString[ i ] < 128 )
				dwStringWidth += m_dwTextSize[ strWideString[ i ] ] ;
			else
			{
				if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::THAILAND )
				{
					BYTE byString = (BYTE)lpStr[ i ] ;

					if ( byString >= 0xa1 && byString <= 0xfb )
						dwStringWidth += THIAFONT_SIZE ;
				}
				else
				{
					dwStringWidth += m_dwTextSize[ 128 ] ;
				}
			}
		}

	}
	return dwStringWidth ;
}

// edith 2008.02.14 채팅창의 메시지 색
DWORD   CRYLStringTable::GetChatColor( DWORD dwCommand ) 
{
	switch( dwCommand )
	{
	case 1:		// Party Chat
		return D3DCOLOR_RGBA(141, 180, 244, 255);

	case 3:		// Guild chat
		return D3DCOLOR_RGBA(255, 229, 57, 255);

	case 7:		// shout
		return D3DCOLOR_RGBA(53, 218, 193, 255);

	case 8:		// operator general conversation
		return D3DCOLOR_RGBA(220, 220, 220, 255);

	case 9:		// operator shout ( GM CHAT COLOR)
		return D3DCOLOR_RGBA(255, 128, 64, 255);  // was 255, 128, 64, 255

	case 10:	// whisper
		return D3DCOLOR_RGBA(255, 128, 239, 255);

	case 11:	// transaction
		return D3DCOLOR_RGBA(204, 128, 255, 255);

	case 13:	// operator command hp
		return D3DCOLOR_RGBA(255, 128, 64, 255);

	case 14:	// Find a party
		return D3DCOLOR_RGBA(255, 128, 64, 255);

	case 16:	// dice
		return D3DCOLOR_RGBA(255, 128, 64, 255);

	case 128:
		return D3DCOLOR_RGBA(100, 255, 100, 255);

	case 129:
		return D3DCOLOR_RGBA(255, 0, 0, 255);

	case 130:	// NPC 대화
		return D3DCOLOR_RGBA(252, 238, 92, 255);

	case 131:
		return D3DCOLOR_RGBA(255, 120, 0, 255);

		// by sparrowhawk
	case 132:   // 파티 명령
		return D3DCOLOR_RGBA(0xFE, 0xFE, 0x80, 255);

	case 133:   // 사망
		return D3DCOLOR_RGBA(0xFE, 0, 0, 255);

	case 134:   // 리스폰
		return D3DCOLOR_RGBA(0x6F, 0xFF, 0x7B, 255);

	case 140:	// 퀘스트 정보 출력 메시지
		return D3DCOLOR_RGBA(255, 255, 0, 255);

		// by ichabod
		// 길드 관련 폰트
	case 200 :	// 길드 가입권유 및 신청시
		return D3DCOLOR_RGBA(50, 128, 100, 255);
	case 201 : // 승낙
		return D3DCOLOR_RGBA(0, 255, 255, 255);
	case 202 : // 권한 설정
		return D3DCOLOR_RGBA(0, 150, 150, 255);

		// by vincent
	case 300 :	// 길드 요새 관련 메세지
		return D3DCOLOR_RGBA( 211, 220, 44, 255 ) ;

	default:
		return D3DCOLOR_RGBA(255, 255, 255, 255);
	}
}

/*
ULONG	CRYLStringTable::GetStringLength( LPCTSTR strString )
{
	ULONG	i, len = strlen( strString ), countx = 0 ;

	for ( i = 0 ; i < len ; i ++ )
	{
		if ( strString[ i ] & 0x80 ) 
		{ 
			countx += m_dwTextSize[ 128 ] ; 
			i ++ ; 
		} 
		else 
		{ 
			countx += m_dwTextSize[ strString[ i ] ] ; 
		}
	}

	return countx ;
}
*/

char*	CRYLStringTable::GetJobName( unsigned long dwClass )
{
	switch (dwClass)
	{
		case CClass::Fighter :		return CRYLStringTable::m_strString[451] ;	break;
		case CClass::Rogue :		return CRYLStringTable::m_strString[452] ;	break;
		case CClass::Mage :			return CRYLStringTable::m_strString[453] ;	break;
		case CClass::Acolyte :		return CRYLStringTable::m_strString[454] ;	break;

			// 인간 1st클래스
		case CClass::Defender :		return CRYLStringTable::m_strString[455] ;	break;
		case CClass::Warrior :		return CRYLStringTable::m_strString[456] ;	break;
		case CClass::Assassin :		return CRYLStringTable::m_strString[457] ;	break;
		case CClass::Archer :		return CRYLStringTable::m_strString[458] ;	break;
		case CClass::Sorcerer :		return CRYLStringTable::m_strString[459] ;	break;
		case CClass::Enchanter :	return CRYLStringTable::m_strString[460] ;	break;
		case CClass::Priest :		return CRYLStringTable::m_strString[461] ;	break;
		case CClass::Cleric :		return CRYLStringTable::m_strString[462] ;	break;

			// 아칸 기본클래스
		case CClass::Combatant :	return CRYLStringTable::m_strString[467] ;	break;
		case CClass::Officiator :	return CRYLStringTable::m_strString[468] ;	break;

			// 아칸 1st클래스
		case CClass::Templar :		return CRYLStringTable::m_strString[469] ;	break;
		case CClass::Attacker :		return CRYLStringTable::m_strString[470] ;	break;
		case CClass::Gunner :		return CRYLStringTable::m_strString[471] ;	break;
		case CClass::RuneOff :		return CRYLStringTable::m_strString[472] ;	break;
		case CClass::LifeOff :		return CRYLStringTable::m_strString[473] ;	break;
		case CClass::ShadowOff :	return CRYLStringTable::m_strString[474] ;	break;
	}

	return CRYLStringTable::m_strString[450];
}

VOID	CRYLStringTable::EraseToken( char *strResult, char *strSource, char *szToken )
{
	int nIndex = 0 ;
	static char strTemp[ MAX_PATH ] ;
	strcpy( strTemp, strSource ) ;

	char* lpToken = strtok( strTemp, szToken ) ;

	if (NULL != lpToken)
	{
		int is = strlen(strSource);
		strcpy( (strResult + nIndex), lpToken ) ;
		nIndex += strlen(lpToken) ;

		for(int i = nIndex+2; i < is; ++i)
		{
			strResult[nIndex] = strSource[i];
			++nIndex;
		}
		strResult[nIndex] = 0;
/*
		while ( NULL != lpToken )
		{
			strcpy( (strResult + nIndex), lpToken ) ;
			nIndex += strlen(lpToken) ;
			lpToken = strtok( NULL, szToken ) ;
		}
*/
	}
	else
	{
		strcpy( strResult, strSource ) ;
	}
}

VOID	CRYLStringTable::GetFirstStringByToken( char *strResult, char *strSource, char *szToken )
{
	static char strTemp[ MAX_PATH ] ;
	strcpy( strTemp, strSource ) ;

	char* lpToken = strtok( strTemp, szToken ) ;

	if (NULL != lpToken)
	{
		strcpy( strResult, lpToken ) ;
	}
	else
	{
		strcpy( strResult, strSource ) ;
	}
}