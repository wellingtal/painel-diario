#ifndef _REGISTRY_FUNCTIONS_H_
#define _REGISTRY_FUNCTIONS_H_

#include <map>
#include <string>

namespace Registry	
{
	bool WriteString(const char *FileName_In, const char *Section_In, 
		const char *KeyName_In, const char *Value_In);

	unsigned long ReadInt(const char *FileName_In, 
		const char *Section_In, const char *KeyName_In);

	bool ReadString(const char *FileName_In, const char *Section_In,
		const char *KeyName_In, char *Buffer_Out, int nBufferSize);

	bool WriteStruct(const char *FileName_In, const char *Section_In,
		const char *KeyName_In, void *Value_In, const int Size_in);

	bool ReadStruct(const char *FileName_In, const char *Section_In,
		const char *KeyName_In, void *Value_Out, const int Size_in);

    class CSetupFile
    {
    public:

        CSetupFile();
        CSetupFile(const char* szFileName);
        ~CSetupFile();
        
        bool Open(const char* szFileName);
        void Clear();

        const char*  GetString(const char* szSection, const char* szKey, const char* szDefaultValue = 0);
        unsigned int GetInt(const char* szSection, const char* szKey, unsigned int nDefaultValue = 0);

    private:

        struct Data
        {
            std::string m_szSection;
            std::string m_szKey;
            std::string m_szValue;

            Data(const std::string& szSection, const std::string& szKey, const std::string& szValue)
                :   m_szSection(szSection), m_szKey(szKey), m_szValue(szValue)
            {

            }
        };

        typedef std::multimap<unsigned long, Data> SetupMap;

        SetupMap m_SetupMap;
    };
};

#endif