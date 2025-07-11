/** 
 *  @file		NFTokenizer.h
 *  @brief		Tokenizer 클래스
 *  @remarks	
 *  @author		강동명(edith2580@gmail.com)
 *  @date		2009-04-02
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

namespace Nave { 

	/** 
	 *  @class			NFTokenizerA
	 *  @brief			멀티바이트용 Tokenizer
	 *  @remarks      
	 *					Nave::String tempStr = L"01|02|03|04|05|06|07|08|09|10|11|12";		\r\n
	 *																						\r\n
	 *					Nave::NFTokenizer strtok = Nave::NFTokenizer(tempStr, L"|");		\r\n
	 *																						\r\n
	 *					LOG_IMPORTANT((L"Number Of Tokens: %d"), strtok.CountTokens());		\r\n
	 *					LOG_IMPORTANT((L"String: %s"), strtok.RemainingString().c_str());	\r\n
	 *																						\r\n
	 *					int cnt = strtok.CountTokens();										\r\n
	 *					Nave::String finalString = L"";										\r\n
	 *																						\r\n
	 *					for(int i = 0; i < cnt; i++)										\r\n
	 *					{																	\r\n
	 *						Nave::String tempStr = L"";										\r\n
	 *																						\r\n
	 *						LOG_IMPORTANT((L"Token[%d] ----> [%s]"), i, strtok.NextToken().c_str());\r\n
	 *						LOG_IMPORTANT((L"Token Count : %d"), strtok.CountTokens());		\r\n
	 *																						\r\n
	 *						finalString += tempStr;											\r\n
	 *					}																	\r\n
	 *																						\r\n
	 *					LOG_IMPORTANT((L"Final String : %s"), finalString.c_str());
	 *                
	 *  @par          
	 *  @author  Edith
	 *  @date    2009-04-05
	 */
	class NFTokenizerA
	{
	public:
		NFTokenizerA(const std::string& _str, const std::string& _delim);
		~NFTokenizerA(){};

		int				CountTokens();
		bool			HasMoreTokens();
		std::string		NextToken();
		int				NextIntToken();
		double			NextFloatToken();
		std::string		NextToken(const std::string& delim);
		std::string&	RemainingString();
		std::string		FilterNextToken(const std::string& filterStr);

	private:
		std::string		token_str;
		std::string		delim;
	};


	/** 
	 *  @class			NFTokenizerW
	 *  @brief			유니코드용 Tokenizer
	 *  @remarks      
	 *					Nave::String tempStr = L"01|02|03|04|05|06|07|08|09|10|11|12";		\r\n
	 *																						\r\n
	 *					Nave::NFTokenizer strtok = Nave::NFTokenizer(tempStr, L"|");		\r\n
	 *																						\r\n
	 *					LOG_IMPORTANT((L"Number Of Tokens: %d"), strtok.CountTokens());		\r\n
	 *					LOG_IMPORTANT((L"String: %s"), strtok.RemainingString().c_str());	\r\n
	 *																						\r\n
	 *					int cnt = strtok.CountTokens();										\r\n
	 *					Nave::String finalString = L"";										\r\n
	 *																						\r\n
	 *					for(int i = 0; i < cnt; i++)										\r\n
	 *					{																	\r\n
	 *						Nave::String tempStr = L"";										\r\n
	 *																						\r\n
	 *						LOG_IMPORTANT((L"Token[%d] ----> [%s]"), i, strtok.NextToken().c_str());\r\n
	 *						LOG_IMPORTANT((L"Token Count : %d"), strtok.CountTokens());		\r\n
	 *																						\r\n
	 *						finalString += tempStr;											\r\n
	 *					}																	\r\n
	 *																						\r\n
	 *					LOG_IMPORTANT((L"Final String : %s"), finalString.c_str());
	 *  @par          
	 *  @author  Edith
	 *  @date    2009-04-05
	 */
	class NFTokenizerW
	{
	public:
		NFTokenizerW(const std::wstring& _str, const std::wstring& _delim);
		~NFTokenizerW(){};

		int				CountTokens();
		bool			HasMoreTokens();
		std::wstring	NextToken();
		int				NextIntToken();
		double			NextFloatToken();
		std::wstring	NextToken(const std::wstring& delim);
		std::wstring&	RemainingString();
		std::wstring	FilterNextToken(const std::wstring& filterStr);

	private:
		std::wstring	token_str;
		std::wstring	delim;
	};

	typedef NFTokenizerW		NFTokenizer;

} 