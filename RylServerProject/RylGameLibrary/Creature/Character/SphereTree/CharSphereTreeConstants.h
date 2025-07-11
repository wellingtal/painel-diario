#ifndef __SPHERE_TREE_CONST_H__
#define __SPHERE_TREE_CONST_H__

namespace SphereConst
{
	enum eSphereNodeFlag
	{
		SNF_ROOT_NODE   = (1<<0),	// this is the root node
		SNF_SUPERSPHERE	= (1<<1),	// this is a supersphere, allocated and deleted by us
		SNF_ACTOR_NODE	= (1<<2),	// this is real actor node
		SNF_ROOT_TREE   = (1<<3),	// member of the root tree
		SNF_LEAF_TREE   = (1<<4),	// member of the leaf node tree
		SNF_RECOMPUTE   = (1<<5),	// needs recomputed bounding sphere
		SNF_INTEGRATE   = (1<<6)	// needs to be reintegrated into tree
	};

	enum eViewState
	{
		VS_INSIDE,					// completely inside the frustum.
		VS_PARTIAL,					// partially inside and partially outside the frustum.
		VS_OUTSIDE					// completely outside the frustum
	};

	enum eConst
	{
		CHAR_RADIUS			= 2,		// 캐릭터 반경 2m
		MAX_SPHERE_NODE		= 2000,		// MAX 2000 명까지
		DEFAULT_ROOT_SIZE	= 256,		// Root node 의 사이즈
		DEFAULT_LEAF_SIZE	= 64,		// Leaf node 의 사이즈
		DEFAULT_GRAVY		= 15		// 부모원과 자식원의 반지름 차
	};
};

#endif	__SPHERE_TREE_CONST_H__