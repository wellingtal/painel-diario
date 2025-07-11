#ifndef _REBALANCELIB_CONTAINER_CHECKER_H_
#define _REBALANCELIB_CONTAINER_CHECKER_H_

#include <Item/ItemStructure.h>
#include <Network/Packet/PacketStruct/CharItemPacketStruct.h>

namespace RebalanceLib
{

class CItemChecker
{
public:

    CItemChecker(TakeType::TakeSource eTakeSource) : m_eTakeSource(eTakeSource) { }

    virtual void Set(Item::ItemPos itemPos, int nXSize, int nYSize) = 0;
    virtual bool Test(Item::ItemPos itemPos, int nXSize, int nYSize) = 0;
    virtual bool FindEmptyPos(Item::ItemPos& foundPos, int nXSize, int nYSize) = 0;	

protected:

    TakeType::TakeSource m_eTakeSource;
};


template<int nListSize>
class CItemListChecker : public CItemChecker
{
public:

    CItemListChecker(TakeType::TakeSource eTakeSource) : CItemChecker(eTakeSource)
    { 
        memset(m_Set, 0, sizeof(char) * nListSize);
    }

    // CItemListChecker에서 nXSize, nYSize는 무의미하다. 제대로 된 값이 아니더라도 상관없다.
    virtual void Set(Item::ItemPos itemPos, int nXSize, int nYSize)
    {
        if(itemPos.m_cIndex < nListSize) 
        {
            m_Set[itemPos.m_cIndex] = 1; 
        }
    }

    virtual bool Test(Item::ItemPos itemPos, int nXSize, int nYSize)
    {
        return (itemPos.m_cIndex < nListSize && 0 == m_Set[itemPos.m_cIndex]);        
    }

    virtual bool FindEmptyPos(Item::ItemPos& foundPos, int nXSize, int nYSize)
    {
        for(int nIndex = 0; nIndex < nListSize; ++nIndex)
        {
            foundPos.SetPos(nIndex);
            if(Test(foundPos, nXSize, nYSize))
            {
                foundPos.m_cPos = static_cast<unsigned short>(m_eTakeSource);
                return true;
            }
        }

        return false;
    }

    void Fill()  { memset(m_Set, 1, sizeof(char) * nListSize); }
    void Clear() { memset(m_Set, 0, sizeof(char) * nListSize); }


private:

    char m_Set[nListSize];
};


template<int tX, int tY, int tZ>
class CItemArrayChecker : public CItemChecker
{
public:

    CItemArrayChecker(TakeType::TakeSource eTakeSource) : CItemChecker(eTakeSource)
    { 
        memset(m_Set, 0, sizeof(char) * tX * tY * tZ);
    }

    virtual void Set(Item::ItemPos itemPos, int nXSize, int nYSize)
    {
        int nZ = itemPos.GetZIndex();

        if(nZ < tZ)
        {
            int nY = itemPos.GetYIndex();
            int nX = itemPos.GetXIndex();

			int nMaxY = min(static_cast<long>(nY + nYSize), static_cast<long>(tY));
			int nMaxX = min(static_cast<long>(nX + nXSize), static_cast<long>(tX));

            for(nY = itemPos.GetYIndex(); nY < nMaxY; ++nY)
            {
                for(nX = itemPos.GetXIndex(); nX < nMaxX; ++nX) 
                { 
                    m_Set[nX][nY][nZ] = 1; 
                }
            }
        }
    }

    virtual bool Test(Item::ItemPos itemPos, int nXSize, int nYSize)
    {
        int nZ = itemPos.GetZIndex();
        int nY = itemPos.GetYIndex();
        int nX = itemPos.GetXIndex();

        int nMaxY = nY + nYSize;
        int nMaxX = nX + nXSize;

        if(tZ < nZ || tY < nMaxY || tX < nMaxX) 
        {
            return false; 
        }

        for(nY = itemPos.GetYIndex(); nY < nMaxY; ++nY)
        {
            for(nX = itemPos.GetXIndex(); nX < nMaxX; ++nX)
            {
                if(0 != m_Set[nX][nY][nZ]) 
                {
                    return false; 
                }
            }
        }

        return true;
    }

    virtual bool FindEmptyPos(Item::ItemPos& foundPos, int nXSize, int nYSize)
    {
        for(int nZ = 0; nZ < tZ; ++nZ)
        {
            for(int nY = 0; nY < tY; ++nY)
            {
                for(int nX = 0; nX < tX; ++nX)
                {                    
                    foundPos.SetPos(nX, nY, nZ);
                    if(Test(foundPos, nXSize, nYSize)) 
                    {
                        foundPos.m_cPos = static_cast<unsigned short>(m_eTakeSource);
                        return true; 
                    }
                }
            }
        }

        return false;
    }

	virtual bool FindQuestEmptyPos(Item::ItemPos& foundPos, int nXSize, int nYSize)
	{		
		for(int nY = 0; nY < tY; ++nY)
		{
			for(int nX = 0; nX < tX; ++nX)
			{                    
				foundPos.SetPos(nX, nY, 3);
				if(Test(foundPos, nXSize, nYSize)) 
				{
					foundPos.m_cPos = static_cast<unsigned short>(m_eTakeSource);
					return true; 
				}
			}
		}

		return false;
	}

    void FillTab(int nTabNum)
    {
        if(nTabNum < tZ)
        {
            for(int nY = 0; nY < tY; ++nY)
            {
                for(int nX = 0; nX < tX; ++nX)
                {
                    m_Set[nX][nY][nTabNum] = 1;
                }
            }
        }
        
    }

    void Fill()  { memset(m_Set, 1, sizeof(char) * tX * tY * tZ); }
    void Clear() { memset(m_Set, 0, sizeof(char) * tX * tY * tZ); }

private:

    char m_Set[tX][tY][tZ];
};


}

#endif