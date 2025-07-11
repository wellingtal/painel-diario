#ifndef _RYL_SERVERLIB_PACKET_DISPATHCH_TABLE_H_
#define _RYL_SERVERLIB_PACKET_DISPATHCH_TABLE_H_

#include <vector>
#include <utility>

template<typename FunctionType>
class CPacketDispatchTable
{
public:

    typedef std::pair<unsigned long, FunctionType>  DispatchType;
    typedef std::vector<DispatchType>               DispatchMap;

    FunctionType GetDispatch(unsigned long dwCmd);

protected:

    struct PredFirstPairOnly
    {
        template<typename Ty1, typename Ty2>
            bool operator () (const std::pair<Ty1, Ty2>& lhs, const std::pair<Ty1, Ty2>& rhs)
        {
            return lhs.first < rhs.first;
        }
    };

    CPacketDispatchTable(size_t nReservedSize) { m_DispatchMap.reserve(nReservedSize); }
    
    bool AddDispatch(unsigned long dwCmd, FunctionType fnDispatch);
    void RemoveDispatch(unsigned long dwCmd);

private:

    DispatchMap         m_DispatchMap;
    PredFirstPairOnly   m_PredFirstPairOnly;
};





template<typename FunctionType>
FunctionType CPacketDispatchTable<FunctionType>::GetDispatch(unsigned long dwCmd)
{
    DispatchType findVal(dwCmd, FunctionType());

    DispatchMap::iterator end = m_DispatchMap.end();
    DispatchMap::iterator pos = std::lower_bound(m_DispatchMap.begin(), 
        end, findVal, m_PredFirstPairOnly);

    return (pos != end && !m_PredFirstPairOnly(findVal, *pos)) ? pos->second : findVal.second;
}


template<typename FunctionType>
bool CPacketDispatchTable<FunctionType>::AddDispatch(unsigned long dwCmd, FunctionType lpDispatch)
{
    DispatchType addVal(dwCmd, lpDispatch);

    DispatchMap::iterator end = m_DispatchMap.end();
    DispatchMap::iterator pos = std::lower_bound(m_DispatchMap.begin(), 
        end, addVal, m_PredFirstPairOnly);

    if(pos == end || m_PredFirstPairOnly(addVal, *pos))
    {
        // 똑같은 커맨드로 삽입 시도. 그냥 넣는다.
        m_DispatchMap.insert(pos, addVal);
        return true;
    }

    return false;
}


template<typename FunctionType>
void CPacketDispatchTable<FunctionType>::RemoveDispatch(unsigned long dwCmd)
{
    DispatchType removeVal(dwCmd, FunctionType());

    DispatchMap::iterator end = m_DispatchMap.end();
    DispatchMap::iterator pos = std::lower_bound(m_DispatchMap.begin(), 
        end, removeVal, m_PredFirstPairOnly);

    if(pos != end && !m_PredFirstPairOnly(removeVal, *pos))
    {
        // 맵에서 제거한다.
        m_DispatchMap.erase(pos);
    }
}


#endif