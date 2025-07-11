#include "stdafx.h"

#include <winsock2.h>
#include <windows.h>
#include "RegFunctions.h"

#include <fstream>

bool Registry::WriteString(const char *FileName_In, const char *Section_In,
								const char *KeyName_In, const char *Value_In)
{
	return 0 != WritePrivateProfileString(Section_In, KeyName_In, Value_In, FileName_In);
}

unsigned long Registry::ReadInt(const char *FileName_In, const char *Section_In, const char *KeyName_In)
{
	return static_cast<unsigned long>(GetPrivateProfileInt(Section_In, KeyName_In, 0, FileName_In));
}

bool Registry::ReadString(const char *FileName_In, const char *Section_In,
								 const char *KeyName_In, char *Buffer_Out, int nBufferSize)
{
	return 0 != GetPrivateProfileString(Section_In, KeyName_In, 
		NULL, Buffer_Out, nBufferSize, FileName_In);
}

bool Registry::WriteStruct(const char *FileName_In, const char *Section_In,
								const char *KeyName_In, void *Value_In, const int Size_in)
{
	return 0 != WritePrivateProfileStruct(Section_In, KeyName_In, Value_In, Size_in, FileName_In);
}

bool Registry::ReadStruct(const char *FileName_In, const char *Section_In,
								 const char *KeyName_In, void *Value_Out, const int Size_in)
{
	return 0 != GetPrivateProfileStruct(Section_In, KeyName_In, Value_Out, Size_in, FileName_In);    
}



Registry::CSetupFile::CSetupFile()
{


}

Registry::CSetupFile::CSetupFile(const char* szFileName)
{
    Open(szFileName);
}

Registry::CSetupFile::~CSetupFile()
{

}

inline unsigned long sdbmHash(const unsigned char *str)
{
    unsigned long hash = 0;
    int c;

	while (c = *str++) { hash = c + (hash << 6) + (hash << 16) - hash; }
    return hash;
}

bool Registry::CSetupFile::Open(const char* szFileName)
{
    std::string fileLine;
    std::string szSection;
    std::string szKey;
    std::string szValue;

    fileLine.reserve(4096);
    szSection.reserve(1024);
    szKey.reserve(1024);
    szValue.reserve(4096);

    std::fstream file(szFileName, std::ios_base::in);

    std::string::size_type nPosition    = std::string::npos;
    std::string::size_type nEndPosition = std::string::npos;
    std::string::size_type nDataPosition    = std::string::npos;
    std::string::size_type nDataEndPosition = std::string::npos;

    unsigned long dwSectionValue = 0;

    if(file.is_open())
    {
        while(std::getline(file, fileLine))
        {
            nPosition = fileLine.find_first_not_of(" \r\n\t");

            if(std::string::npos != nPosition)
            {
                if('[' == fileLine[nPosition])
                {
                    // ¼½¼Ç¸íÀÓ
                    nEndPosition = fileLine.find_first_of(']', nPosition);
                    if(std::string::npos != nEndPosition)
                    {
                        szSection.assign(fileLine.begin() + nPosition + 1,
                            fileLine.begin() + nEndPosition);

                        dwSectionValue = sdbmHash(reinterpret_cast<const unsigned char*>(szSection.c_str()));
                    }                    
                }
                else if(0 != fileLine.compare(nPosition, sizeof(char) * 2, "//"))
                {
                    // ÁÖ¼® ¾Æ´Ô
                    nEndPosition = fileLine.find_first_of('=', nPosition);
                
                    if(std::string::npos != nEndPosition)
                    {
                        // nEndPosition ÁÂ¿ì·Î Key - Value PairÀÓ.
                        nDataEndPosition = fileLine.find_last_not_of("= \t", nEndPosition);

                        if(std::string::npos != nDataEndPosition)
                        {
                            // Å° ÀÐ¾î¿È.
                            szKey.assign(fileLine.begin() + nPosition,
                                fileLine.begin() + nDataEndPosition + 1);

                            nDataPosition = fileLine.find_first_not_of("= \t", nEndPosition);
                            
                            if(std::string::npos != nDataPosition)
                            {
                                nDataEndPosition = fileLine.find_last_not_of(" \r\n\t");

                                if(std::string::npos != nDataEndPosition)
                                {
                                    // °ª ÀÐ¾î¿È
                                    m_SetupMap.insert(SetupMap::value_type(
                                        sdbmHash(reinterpret_cast<const unsigned char*>(szKey.c_str())) + dwSectionValue, 
                                        Data(szSection, szKey, 
                                        std::string(fileLine.begin() + nDataPosition, fileLine.begin() + nDataEndPosition + 1))));
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return false;
}

void Registry::CSetupFile::Clear()
{
    m_SetupMap.clear();
}

const char*  Registry::CSetupFile::GetString(const char* szSection, const char* szKey, 
                                             const char* szDefaultValue)
{
    DWORD dwHashValue = sdbmHash(reinterpret_cast<const unsigned char*>(szSection))
        + sdbmHash(reinterpret_cast<const unsigned char*>(szKey));

    std::pair<SetupMap::iterator, SetupMap::iterator> pos_pair = m_SetupMap.equal_range(dwHashValue);

    for(; pos_pair.first != pos_pair.second; ++pos_pair.first)
    {
        Data& data = pos_pair.first->second;

        if(0 == data.m_szSection.compare(szSection) &&
            0 == data.m_szKey.compare(szKey))
        {
            return data.m_szValue.c_str();
        }
    }

    return szDefaultValue;
}

unsigned int Registry::CSetupFile::GetInt(const char* szSection, const char* szKey,
                                          unsigned int nDefaultValue)
{
    const char* szStr = GetString(szSection, szKey);

    if(0 != szStr)
    {
        return atoi(szStr);
    }

    return nDefaultValue;
}

