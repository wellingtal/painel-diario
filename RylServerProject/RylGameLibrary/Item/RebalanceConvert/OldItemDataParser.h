#ifndef _OLD_ITEM_DATA_PARSER_H_
#define _OLD_ITEM_DATA_PARSER_H_

#include <vector>

// 전방 참조
namespace OldItem
{
    struct ItemInfo;
};

namespace OldItemDataParser
{
    struct ParseData
    {
        typedef bool(*ParseFunc) (OldItem::ItemInfo& itemInfo, const char* szValue);

        const char* m_szColumnName;
        ParseFunc   m_fnParseFunc;

        ParseData(const char* szColumnName, ParseFunc fnParseFunc)
            :   m_szColumnName(szColumnName), m_fnParseFunc(fnParseFunc) { }
    };

    typedef std::vector<ParseData>  ParseDataArray;

    bool SetDefaultData(ParseDataArray& parseDataArray);
    bool SetEquipData(ParseDataArray& parseDataArray);
    bool SetUseItemData(ParseDataArray& parseDataArray);
    bool SetEtcItemData(ParseDataArray& parseDataArray);
};


#endif