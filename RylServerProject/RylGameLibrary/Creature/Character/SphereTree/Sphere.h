#ifndef __SPHERE_H__
#define __SPHERE_H__

#include <math.h>
#include <Creature/CreatureStructure.h>

//======================================================================================
// 3차원 좌표 구조체
/*
#ifndef EPSILON
#define EPSILON		0.001
#endif

struct sVector3
{
	union
	{
		struct
		{
			float x;
			float y;
			float z;
		};

		float v[3];
	};

	sVector3()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	sVector3( float fX, float fY, float fZ )
	{
		x = fX;
		y = fY;
		z = fZ;
	}

	float Length() const
	{
		return sqrtf( x*x + y*y + z*z );
	}

	float Distance( const sVector3& inPos ) const
	{
		sVector3 tempVector( inPos.x - x, inPos.y - y, inPos.z - z );
		return tempVector.Length();
	}

	float Distance2( const sVector3& inPos ) const
	{
		return ( (inPos.x - x) * (inPos.x - x) + (inPos.y - y) * (inPos.y - y) + (inPos.z - z) * (inPos.z - z) );
	}

	bool operator == ( const sVector3& inPos ) const
	{
		if ( fabs(x - inPos.x) <= EPSILON && fabs(y - inPos.y) <= EPSILON && fabs(z - inPos.z) <= EPSILON )
		{
			return true;
		}

		return false;
	}

	bool operator != ( const sVector3& inPos ) const
	{
		if ( fabs(x - inPos.x) > EPSILON || fabs(y - inPos.y) > EPSILON || fabs(z - inPos.z) > EPSILON )
		{
			return true;
		}

		return false;
	}
};
*/

//======================================================================================
// 기본 구 클래스
class CSphere
{
public:

	CSphere();
	CSphere( float fX, float fY, float fZ, float fRadius );
	CSphere( const Position& inPos, float fRadius );

	// ===============================================================
	void	Set( float fX, float fY, float fZ, float fRadius );
	void	Set( const Position& inPos, float fRadius );
	void	SetRadius( float fRadius );

	float			GetX() const			{ return m_Center.m_fPointX;	}
	float			GetY() const			{ return m_Center.m_fPointY;	}
	float			GetZ() const			{ return m_Center.m_fPointZ;	}
	const Position&	GetCenter()	const		{ return m_Center;				}
	float			GetRadius()	const		{ return m_fRadius;				}
	float			GetRadius2() const		{ return m_fRadius2;			}

	// ===============================================================
	// 3차원 좌표가 구안에 있는지 검사
	// 구의 반지름 + fDistance 안에 있는지 검사를 한다.
	bool	InSphere( float fX, float fY, float fZ, float fDistance );
	bool	InSphere( const Position& inPos, float fDistance );

	// 2차원 좌표가 구안에 있는지 검사
	// 구의 반지름 + fDistance 안에 있는지 검사를 한다.
	bool	InSphereXY( float fX, float fY, float fDistance );
	bool	InSphereXY( const Position& inPos, float fDistance );

protected:

	Position	m_Center;		// 구의 중심 좌표

private:

	float		m_fRadius;		// 구의 반지름
	float		m_fRadius2;		// 구의 반지름 제곲 값
};

inline
CSphere::CSphere() :
	m_fRadius(0), m_fRadius2(0)
{
}

inline
CSphere::CSphere( float fX, float fY, float fZ, float fRadius ) :
	m_fRadius(fRadius), m_fRadius2(fRadius * fRadius)
{
}

inline
CSphere::CSphere( const Position& inPos, float fRadius ) :
	m_Center(inPos), m_fRadius(fRadius), m_fRadius2(fRadius * fRadius)
{
}

inline void
CSphere::Set( float fX, float fY, float fZ, float fRadius )
{
	m_Center.m_fPointX = fX;
	m_Center.m_fPointY = fY;
	m_Center.m_fPointZ = fZ;

	m_fRadius = fRadius;
	m_fRadius2 = fRadius * fRadius;
}

inline void
CSphere::Set( const Position& inPos, float fRadius )
{
	m_Center = inPos;

	m_fRadius = fRadius;
	m_fRadius2 = fRadius * fRadius;
}

inline void
CSphere::SetRadius( float fRadius )
{
	m_fRadius = fRadius;
	m_fRadius2 = fRadius * fRadius;
}

inline bool
CSphere::InSphere( float fX, float fY, float fZ, float fDistance )
{
	float dx = fX - m_Center.m_fPointX;
	float dy = fY - m_Center.m_fPointY;
	float dz = fZ - m_Center.m_fPointZ;
	float dist  = sqrtf( dx*dx + dy*dy + dz*dz );

	if ( dist < (m_fRadius + fDistance) ) return true;
	return false;
}

inline bool
CSphere::InSphere( const Position& inPos, float fDistance )
{
	float dx = inPos.m_fPointX - m_Center.m_fPointX;
	float dy = inPos.m_fPointY - m_Center.m_fPointY;
	float dz = inPos.m_fPointZ - m_Center.m_fPointZ;
	float dist  = sqrtf( dx*dx + dy*dy + dz*dz );

	if ( dist < (m_fRadius + fDistance) ) return true;
	return false;
}

inline bool
CSphere::InSphereXY( float fX, float fY, float fDistance )
{
	float dx = fX - m_Center.m_fPointX;
	float dy = fY - m_Center.m_fPointY;
	float dist  = sqrtf( dx*dx + dy*dy );

	if ( dist < (m_fRadius + fDistance) ) return true;
	return false;
}

inline bool
CSphere::InSphereXY( const Position& inPos, float fDistance )
{
	float dx = inPos.m_fPointX - m_Center.m_fPointX;
	float dy = inPos.m_fPointY - m_Center.m_fPointY;
	float dist  = sqrtf( dx*dx + dy*dy );

	if ( dist < (m_fRadius + fDistance) ) return true;
	return false;
}

#endif	__SPHERE_H__