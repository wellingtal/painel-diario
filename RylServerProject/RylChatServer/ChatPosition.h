/*

#ifndef _CCHAT_POSITION_H_
#define _CCHAT_POSITION_H_

#include <cmath>

class CChatPosition
{
public:

    typedef unsigned short  position_t;
    typedef unsigned int    range_t;

    CChatPosition();    
    CChatPosition(position_t posX, position_t posY, position_t posZ);

    inline range_t GetSquaredRange(CChatPosition& ChatPosition);
    inline range_t GetRange(CChatPosition& ChatPosition);
    
    position_t GetXPos() const { return m_posX; }
    position_t GetYPos() const { return m_posY; }
    position_t GetZPos() const { return m_posZ; }

    void SetPos(position_t posX, position_t posY, position_t posZ) { m_posX = posX; m_posY = posY; m_posZ = posZ; }
    void GetPos(position_t& posX, position_t& posY, position_t& posZ) { posX = m_posX; posY = m_posY; posZ = m_posZ; }

private:

    position_t  m_posX;
    position_t  m_posY;
    position_t  m_posZ;
};

inline CChatPosition::range_t CChatPosition::GetSquaredRange(CChatPosition& ChatPosition)
{
    return (m_posX - ChatPosition.GetXPos()) * (m_posX - ChatPosition.GetXPos()) +
        (m_posY - ChatPosition.GetYPos()) * (m_posY - ChatPosition.GetYPos()) +
        (m_posZ - ChatPosition.GetZPos()) * (m_posZ - ChatPosition.GetZPos());
}

inline CChatPosition::range_t CChatPosition::GetRange(CChatPosition& ChatPosition)
{
    return static_cast<CChatPosition::range_t>(std::sqrt(static_cast<float>(GetSquaredRange(ChatPosition))));
}

#endif

*/