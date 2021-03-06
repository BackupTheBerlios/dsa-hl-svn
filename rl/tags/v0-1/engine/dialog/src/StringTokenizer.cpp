/* This source file is part of Rastullahs Lockenpracht.
 * Copyright (C) 2003-2005 Team Pantheon. http://www.team-pantheon.de
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the Clarified Artistic License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  Clarified Artistic License for more details.
 *
 *  You should have received a copy of the Clarified Artistic License
 *  along with this program; if not you can get it here
 *  http://www.jpaulmorrison.com/fbp/artistic2.htm.
 */

#include "StringTokenizer.h"

//--	StringTokenizer.cpp

#include <cstring>

using namespace std;

//---------------------------------------------------------//
//
//	NOTES
//
//	Need to add support for multiple delimiters (specified
//	by a string, rather than a single character). Would
//	rather not use find_first_not_of and find_last_not_of
//	functions, but rather the c-style string functions.
//
//	Maybe for multiple delimiters, the STL functions will
//	work better, but we'll see...
//
//	strtok() supports multiple delimiter characters, but it
//	also destroys the original string, and is most likely
//	not thread safe.
//
//	nextToken() shows an interesting workaround. Despite
//	the original stl::string appearing to have no artifacts
//	of the null bytes in the string, the c-strings get hung
//	up on them when the string is reused. So, after making
//	the byte null, it needs to be restored so that things
//	work normally.
//
//---------------------------------------------------------//
namespace rl
{
	bool StringTokenizer::hasMoreTokens() 
	{
		if (str.find_first_of(delim) == CeGuiString::npos)
			return false;
		else
			return true;
		//while ( str.length() > 0 ) 
		//{
		//	register char ch = *str;

		//	if ( ch == '\0' ) 
		//	{ 
		//		str = NULL; 
		//		return false; 
		//	}	//--	d'oh!
		//	else if ( strchr(delim, ch) == NULL ) 
		//	{ 
		//		opt = true; 
		//		return true; 
		//	}
		//	else ++str;
		//}
		//return false;
	}

	CeGuiString StringTokenizer::nextToken() 
	{
		str.c_str();
		CeGuiString::size_type pos = str.find_first_of(delim);
		
		if (pos == CeGuiString::npos)
		{
			str = "";
			return "";
		}
		else
		{
			CeGuiString token = str.substr(0, pos);
			token.c_str();
			str = str.substr(pos+1);
			str.c_str();
			return token;
		}
		//if ( str == NULL ) return "";
		//if ( !opt )
		//	if ( !hasMoreTokens() ) return "";

		//opt = false;
		////--	then using slightly more optimal approach
		//const char *end = strpbrk(str, delim);
		//if ( end != NULL ) 
		//{
		//	int size = (end - str) / sizeof(char);
		//	string token(str, size);
		//	str = end + 1;
		//	return token;
		//} else {
		//	string token(str);
		//	str = NULL;
		//	return token;
		//}
	}

	CeGuiString StringTokenizer::getString() 
	{
		return str;
	}
}//Namespace rl ends
//--	end-of-file
