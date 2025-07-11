#ifndef __VIRTUAL_AREA_CONSTANTS_H__
#define __VIRTUAL_AREA_CONSTANTS_H__

#pragma once

// 헤더 파일 빼기
#include <Creature/CreatureStructure.h>
#include <Creature/Character/CharacterClass.h>

namespace VirtualArea
{
	enum VirtualAreaType
	{
		// MapIndex 에 들어가는 Flag 와 VID(VirtualAreaID) 에 사용한다.
		BGSERVERMAP		=	0x8000,
		DUELMAP			=	0x4000, 
		DUNGEON			=	0x2000
	};

	enum MapType
	{
		FRAG			= 0,		// 스코어 경쟁 게임 방
		STATUE			= 1,		// 석상 점령 게임 방

		MAX_MAP_TYPE
	};

	enum MoveType
	{
		TYPE_PLAYER		= 0,		// 게임 참전자
		TYPE_SPECTATOR	= 1,		// 게임 관전자

		MAX_ENTER_TYPE
	};

	enum Status
	{
		START_WAITING		= 0,		// GameStart() 를 호출하기 전까지...
		GAME_PLAYING		= 1,		// 게임 진행 상태
		GAME_RESTING		= 2,		// 게임이 끝나고 휴식 상태
		MOVEZONE_WAITING	= 3,		// 게임이 끝나고, 존 이동될때까지의 상태

		MAX_STATUS_TYPE		= 4
	};

	enum Score
	{
		FRAG_SCORE						= 1,

		HOSTILITY_STATUE_SCORE			= 0,
		NEUTRALITY_STATUE_SCORE			= 0,
		FRIENDLY_STATUE_SCORE			= 2,
		FRIENDLY_LOADING_STATUE_SCORE	= 1
	};

	enum Const
	{
		MAX_MAP_TYPE_NAME				= 32,
		MAX_FILE_NAME					= 128,
		MAX_VIRTUAL_AREA_RESPAWN_POINT	= 3,
		MAX_LOBBY_RESPAWN_POS			= 2,
		
		MILLISEC_PER_MINUTE				= 60000,		// 1분 (60000 ms)
		MILEAGE_PER_MINUTE_FOR_WIN		= 20,			// 1분 공헌메달 보상 수치 (이긴 경우)
		MILEAGE_PER_MINUTE_FOR_DRAW		= 10,			// 1분 공헌메달 보상 수치 (비긴 경우)

		MOVEZONE_WAIT_TIME				= 10000,		// 10초 후 존이동
	};


	// 초기값
	const unsigned char		DefaultMaxCharacterNumOfNation[MAX_MAP_TYPE]	= { 6, 6 };		// 60명,	120명
	const unsigned short	DefaultTargetScore[MAX_MAP_TYPE]				= { 500, 6 };		// 500점,	6점
	const unsigned char		DefaultLimitMin[MAX_MAP_TYPE]					= { 5, 5 };		// 25분,	50분
	const unsigned char		DefaultRestMin[MAX_MAP_TYPE]					= { 5, 10 };		// 5분,		10분

	// BATTLE_SERVER 대기존 리스폰 좌표
	const POS				RespawnPos[CClass::MAX_RACE][MAX_LOBBY_RESPAWN_POS] =
	{	// Human
		{ { 2165, 1135, 1005 },		{ 1727, 1135, 1005 } },

		// Akhan
		{ { 2119, 1132, 1841 },		{ 1683, 1132, 1841 } }
	};

	static const char*		ms_szVirtualAreaScriptFileName		= "./Script/Game/VirtualAreaScript.txt";
}

#endif	// __VIRTUAL_AREA_CONSTANTS_H__