// GUITooltipItem.h: interface for the CGUITooltipItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUITOOLTIPITEM_H__A1D87897_CA5C_4253_B84C_8164BA57605C__INCLUDED_)
#define AFX_GUITOOLTIPITEM_H__A1D87897_CA5C_4253_B84C_8164BA57605C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GUITooltip.h"
#include <vector>

using namespace std;

class CRYLPicture ;
class CRYLStringGrid ;
class CItemInstance ;

class CGUITooltipItem : public CGUITooltip  
{
protected:

	CItemInstance*  m_lpItem;
	CRYLPicture*    m_pTooltipImg ;
	CRYLStringGrid*	m_pTooltipList ;

public:

	CGUITooltipItem();
	~CGUITooltipItem();

	void Render(LPDIRECT3DDEVICE8 lpD3DDevice);
	void SetItem(CItemInstance* lpItem) { m_lpItem = lpItem; }
	
	BOOL GetIsTooltip(void);
	void DestroyTooltip(void);

protected:

	void CreateTooltip( const RECT& rt ) ;    

    void GetEquipTooltipPos(short &sPosX, short &sPosY, short &sLength, short &sLine);
	void GetBookTooltipPos(short &sPosX, short &sPosY, short &sLength, short &sLine);
	void GetEtcTooltipPos(short &sPosX, short &sPosY, short &sLength, short &sLine);
	void GetRuneTooltipPos(short &sPosX, short &sPosY, short &sLength, short &sLine);

	void RenderEquip(LPDIRECT3DDEVICE8 lpD3DDevice);
	void RenderSkill(LPDIRECT3DDEVICE8 lpD3DDevice);
	void RenderBook(LPDIRECT3DDEVICE8 lpD3DDevice);
	void RenderEtc(LPDIRECT3DDEVICE8 lpD3DDevice);
	void RenderRune(LPDIRECT3DDEVICE8 lpD3DDevice);

	void MakeSkillBookDesc( unsigned short wWidth ) ;
	void MakeEtcDesc( unsigned short wWidth ) ;
	void MakeEquipDesc( unsigned short wWidth, LPDIRECT3DDEVICE8 lpD3DDevice ) ;
       
    unsigned long GetGradeColor(const Item::Attribute::Type eType);
	unsigned long GetGradeColor(const Item::EquipType::Grade eGrade);

	bool ClassCheck(unsigned long dwClassType, unsigned char cClass);
};

#endif // !defined(AFX_GUITOOLTIPITEM_H__A1D87897_CA5C_4253_B84C_8164BA57605C__INCLUDED_)
