//================================================================
//
//	Name : RYLImageManager.h 
//	Desc : RYL UI Image Manager Class
//  Date : 2003. 09. 19
//
//================================================================
#ifndef __RYLIMAGEMANAGER_H__
#define __RYLIMAGEMANAGER_H__

#include "RYLImageCabinet.h"
#include <boost/pool/object_pool.hpp>

class CGMList;

class CRYLImageManager
{
public:

    static HRESULT			 CreateInstance() ;
	static HRESULT	 		 DeleteInstance() ;
	static BOOL		 		 IsInited() ;
	static CRYLImageManager* Instance() ;
	static CRYLImageManager* s_pInstance ;
	static CRYLImageCabinet* s_pDefaultImageCabinet ;

	VOID	Clear() ;
	VOID	CreateImageCabinet( CRYLImageCabinet **ppImageCabinet, LPSTR pFileName = NULL ) ;

	CRYLImageCabinet* GetImageCabinet( INT Index ) ;
	CRYLImageCabinet* GetImageCabinet( LPSTR pFileName ) ;

    CRYLImage* CreateRYLImage();
    void DeleteRYLImage(CRYLImage* lpRYLImage);

protected:

    CRYLImageManager();
	virtual ~CRYLImageManager();

private:

    CGMList *m_pDataList;
    boost::object_pool<CRYLImage>           m_RYLImagePool;
    boost::object_pool<CRYLImageCabinet>    m_RYLImageCabinetPool;
};

#endif