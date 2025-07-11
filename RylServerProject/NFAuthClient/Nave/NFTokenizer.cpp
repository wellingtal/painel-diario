#include "NFTokenizer.h"

namespace Nave { 

	NFTokenizerA::NFTokenizerA(const std::string& _str, const std::string& _delim)
	{

	   if ((_str.length() == 0) || (_delim.length() == 0)) return;

	   token_str = _str;
	   delim     = _delim;

	  /*
		 Remove sequential delimiter
	  */
	   unsigned int curr_pos = 0;

	   while(true)
	   {
		  if ((curr_pos = token_str.find(delim,curr_pos)) != std::string::npos)
		  {
			 curr_pos += delim.length();

			 while(token_str.find(delim,curr_pos) == curr_pos)
			 {
				token_str.erase(curr_pos,delim.length());
			 }
		  }
		  else
		   break;
	   }

	   /*
		 Trim leading delimiter
	   */
	   if (token_str.find(delim,0) == 0)
	   {
		  token_str.erase(0,delim.length());
	   }

	   /*
		 Trim ending delimiter
	   */
	   curr_pos = 0;
	   if ((curr_pos = token_str.rfind(delim)) != std::string::npos)
	   {
		  if (curr_pos != (token_str.length() - delim.length())) return;
		  token_str.erase(token_str.length() - delim.length(),delim.length());
	   }

	}


	int NFTokenizerA::CountTokens()
	{

	   unsigned int prev_pos = 0;
	   int num_tokens        = 0;

	   if (token_str.length() > 0)
	   {
		  num_tokens = 0;

		  unsigned int curr_pos = 0;
		  while(true)
		  {
			 if ((curr_pos = token_str.find(delim,curr_pos)) != std::string::npos)
			 {
				num_tokens++;
				prev_pos  = curr_pos;
				curr_pos += delim.length();
			 }
			 else
			  break;
		  }
		  return ++num_tokens;
	   }
	   else
	   {
		  return 0;
	   }

	}

	bool NFTokenizerA::HasMoreTokens()
	{
	   return (token_str.length() > 0);
	}

	std::string NFTokenizerA::NextToken()
	{

	   if (token_str.length() == 0)
		 return "";

	   std::string  tmp_str = "";
	   unsigned int pos     = token_str.find(delim,0);

	   if (pos != std::string::npos)
	   {
		  tmp_str   = token_str.substr(0,pos);
		  token_str = token_str.substr(pos+delim.length(),token_str.length()-pos);
	   }
	   else
	   {
		  tmp_str   = token_str.substr(0,token_str.length());
		  token_str = "";
	   }

	   return tmp_str;
	}

	int NFTokenizerA::NextIntToken()
	{
	   return atoi(NextToken().c_str());
	}

	double NFTokenizerA::NextFloatToken()
	{
	   return atof(NextToken().c_str());
	}

	std::string NFTokenizerA::NextToken(const std::string& delimiter)
	{
	   if (token_str.length() == 0)
		 return "";

	   std::string  tmp_str = "";
	   unsigned int pos     = token_str.find(delimiter,0);

	   if (pos != std::string::npos)
	   {
		  tmp_str   = token_str.substr(0,pos);
		  token_str = token_str.substr(pos + delimiter.length(),token_str.length() - pos);
	   }
	   else
	   {
		  tmp_str   = token_str.substr(0,token_str.length());
		  token_str = "";
	   }

	   return tmp_str;
	}

	std::string& NFTokenizerA::RemainingString()
	{
	   return token_str;
	}

	std::string NFTokenizerA::FilterNextToken(const std::string& filterStr)
	{
	   std::string  tmp_str    = NextToken();
	   unsigned int currentPos = 0;

	   while((currentPos = tmp_str.find(filterStr,currentPos)) != std::string::npos)
	   {
		  tmp_str.erase(currentPos,filterStr.length());
	   }

	   return tmp_str;
	}


	// unicode type
	NFTokenizerW::NFTokenizerW(const std::wstring& _str, const std::wstring& _delim)
	{

	   if ((_str.length() == 0) || (_delim.length() == 0)) return;

	   token_str = _str;
	   delim     = _delim;

	  /*
		 Remove sequential delimiter
	  */
	   unsigned int curr_pos = 0;

	   while(true)
	   {
		  if ((curr_pos = token_str.find(delim,curr_pos)) != std::wstring::npos)
		  {
			 curr_pos += delim.length();

			 while(token_str.find(delim,curr_pos) == curr_pos)
			 {
				token_str.erase(curr_pos,delim.length());
			 }
		  }
		  else
		   break;
	   }

	   /*
		 Trim leading delimiter
	   */
	   if (token_str.find(delim,0) == 0)
	   {
		  token_str.erase(0,delim.length());
	   }

	   /*
		 Trim ending delimiter
	   */
	   curr_pos = 0;
	   if ((curr_pos = token_str.rfind(delim)) != std::wstring::npos)
	   {
		  if (curr_pos != (token_str.length() - delim.length())) return;
		  token_str.erase(token_str.length() - delim.length(),delim.length());
	   }

	}

	int NFTokenizerW::CountTokens()
	{
	   unsigned int prev_pos = 0;
	   int num_tokens        = 0;

	   if (token_str.length() > 0)
	   {
		  num_tokens = 0;

		  unsigned int curr_pos = 0;
		  while(true)
		  {
			 if ((curr_pos = token_str.find(delim,curr_pos)) != std::wstring::npos)
			 {
				num_tokens++;
				prev_pos  = curr_pos;
				curr_pos += delim.length();
			 }
			 else
			  break;
		  }
		  return ++num_tokens;
	   }
	   else
	   {
		  return 0;
	   }

	}

	bool NFTokenizerW::HasMoreTokens()
	{
	   return (token_str.length() > 0);
	}

	std::wstring NFTokenizerW::NextToken()
	{

	   if (token_str.length() == 0)
		 return L"";

	   std::wstring  tmp_str = L"";
	   unsigned int pos     = token_str.find(delim,0);

	   if (pos != std::wstring::npos)
	   {
		  tmp_str   = token_str.substr(0,pos);
		  token_str = token_str.substr(pos+delim.length(),token_str.length()-pos);
	   }
	   else
	   {
		  tmp_str   = token_str.substr(0,token_str.length());
		  token_str = L"";
	   }

	   return tmp_str;
	}

	int NFTokenizerW::NextIntToken()
	{
	   return _wtoi(NextToken().c_str());
	}

	double NFTokenizerW::NextFloatToken()
	{
	   return _wtof(NextToken().c_str());
	}

	std::wstring NFTokenizerW::NextToken(const std::wstring& delimiter)
	{
	   if (token_str.length() == 0)
		 return L"";

	   std::wstring  tmp_str = L"";
	   unsigned int pos     = token_str.find(delimiter,0);

	   if (pos != std::wstring::npos)
	   {
		  tmp_str   = token_str.substr(0,pos);
		  token_str = token_str.substr(pos + delimiter.length(),token_str.length() - pos);
	   }
	   else
	   {
		  tmp_str   = token_str.substr(0,token_str.length());
		  token_str = L"";
	   }

	   return tmp_str;
	}

	std::wstring& NFTokenizerW::RemainingString()
	{
	   return token_str;
	}

	std::wstring NFTokenizerW::FilterNextToken(const std::wstring& filterStr)
	{
	   std::wstring  tmp_str    = NextToken();
	   unsigned int currentPos = 0;

	   while((currentPos = tmp_str.find(filterStr,currentPos)) != std::wstring::npos)
	   {
		  tmp_str.erase(currentPos,filterStr.length());
	   }

	   return tmp_str;
	}

} 