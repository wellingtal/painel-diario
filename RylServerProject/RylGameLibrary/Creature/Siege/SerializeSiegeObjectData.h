#ifndef __SERIALIZE_SIEGEOBJECT_DATA_H__
#define __SERIALIZE_SIEGEOBJECT_DATA_H__

#include "BroadCastSiegeObjectData.h"

// 전방 참조
class CSiegeObject;

namespace BroadCastSiege
{
	class CSerializeSiegeObjectData
	{
	public:
		CSerializeSiegeObjectData();
		~CSerializeSiegeObjectData();

		// ==================================================================================
		// 초기화
		void	Initialize( const CSiegeObject& siegeObject );

		// ==================================================================================
		// Data 를 셋팅
		void	PrepareData( const CSiegeObject& siegeObject );

		void	PrepareBroadCastData( const CSiegeObject& siegeObject );
		void	PrepareDeltaData( const CSiegeObject& siegeObject );
		void	ClearDeltaData()						{ m_wDeltaBroadCastDataLen = 0;		}

		// ==================================================================================
		// Get 함수
		const CSiegeObjectData&	GetSiegeObjectData()	{ return m_LastSiegeObjectData;		}

		const char*				GetBroadCastData()		{ return m_aryBroadCastData;		}
		unsigned short			GetBroadCastDataLen()	{ return m_wBroadCastDataLen;		}

		const char*				GetDeltaData()			{ return m_aryDeltaBroadCastData;	}
		unsigned short			GetDeltaDataLen()		{ return m_wDeltaBroadCastDataLen;	}

	private:
		CSiegeObjectData	m_LastSiegeObjectData;

		unsigned short		m_wBroadCastDataLen;
		unsigned short		m_wDeltaBroadCastDataLen;

		char				m_aryBroadCastData[ MAX_SIEGEOBJECT_DATA_SIZE ];
		char				m_aryDeltaBroadCastData[ MAX_SIEGEOBJECT_DATA_SIZE ];
	};
}

#endif	__SERIALIZE_SIEGEOBJECT_DATA_H__