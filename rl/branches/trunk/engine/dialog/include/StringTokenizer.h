/* This source file is part of Rastullahs Lockenpracht.
 * Copyright (C) 2003-2004 Team Pantheon. http://www.team-pantheon.de
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the Perl Artistic License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  Perl Artistic License for more details.
 *
 *  You should have received a copy of the Perl Artistic License
 *  along with this program; if not you can get it here
 *  http://www.perldoc.com/perl5.6/Artistic.html.
 */

#ifndef __Rl_DialogStringTokenizer_H__
#define __Rl_DialogStringTokenizer_H__

//--	StringTokenizer.h
#include "DialogPrerequisites.h"

/* Used for Graphmaster, unchanged from original jAlice Source*/

#include <string>
#include <sstream>

using namespace std;

namespace rl
{
	class _RlDialogExport StringTokenizer
	{
	public:
		StringTokenizer(const string &data, const char *delimiter): str(data.c_str()), delim(delimiter), opt(false) { }
		~StringTokenizer() { }
	
		bool hasMoreTokens();
		string nextToken();
		string getString();
	private:
		const char *str;
		const char *delim;
		bool opt;
	};
}
#endif
