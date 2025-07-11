#ifndef __VIRTUAL_AREA_CONSTANTS_H__
#define __VIRTUAL_AREA_CONSTANTS_H__

#pragma once

// ��� ���� ����
#include <Creature/CreatureStructure.h>
#include <Creature/Character/CharacterClass.h>

namespace VirtualArea
{
	enum VirtualAreaType
	{
		// MapIndex �� ���� Flag �� VID(VirtualAreaID) �� ����Ѵ�.
		BGSERVERMAP		=	0x8000,
		DUELMAP			=	0x4000, 
		DUNGEON			=	0x2000
	};

	enum MapType
	{
		FRAG			= 0,		// ���ھ� ���� ���� ��
		STATUE			= 1,		// ���� ���� ���� ��

		MAX_MAP_TYPE
	};

	enum MoveType
	{
		TYPE_PLAYER		= 0,		// ���� ������
		TYPE_SPECTATOR	= 1,		// ���� ������

		MAX_ENTER_TYPE
	};

	enum Status
	{
		START_WAITING		= 0,		// GameStart() �� ȣ���ϱ� ������...
		GAME_PLAYING		= 1,		// ���� ���� ����
		GAME_RESTING		= 2,		// ������ ������ �޽� ����
		MOVEZONE_WAITING	= 3,		// ������ ������, �� �̵��ɶ������� ����

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
		
		MILLISEC_PER_MINUTE				= 60000,		// 1�� (60000 ms)
		MILEAGE_PER_MINUTE_FOR_WIN		= 20,			// 1�� ����޴� ���� ��ġ (�̱� ���)
		MILEAGE_PER_MINUTE_FOR_DRAW		= 10,			// 1�� ����޴� ���� ��ġ (��� ���)

		MOVEZONE_WAIT_TIME				= 10000,		// 10�� �� ���̵�
	};


	// �ʱⰪ
	const unsigned char		DefaultMaxCharacterNumOfNation[MAX_MAP_TYPE]	= { 6, 6 };		// 60��,	120��
	const unsigned short	DefaultTargetScore[MAX_MAP_TYPE]				= { 500, 6 };		// 500��,	6��
	const unsigned char		DefaultLimitMin[MAX_MAP_TYPE]					= { 5, 5 };		// 25��,	50��
	const unsigned char		DefaultRestMin[MAX_MAP_TYPE]					= { 5, 10 };		// 5��,		10��

	// BATTLE_SERVER ����� ������ ��ǥ
	const POS				RespawnPos[CClass::MAX_RACE][MAX_LOBBY_RESPAWN_POS] =
	{	// Human
		{ { 2165, 1135, 1005 },		{ 1727, 1135, 1005 } },

		// Akhan
		{ { 2119, 1132, 1841 },		{ 1683, 1132, 1841 } }
	};

	static const char*		ms_szVirtualAreaScriptFileName		= "./Script/Game/VirtualAreaScript.txt";
}

#endif	// __VIRTUAL_AREA_CONSTANTS_H__