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

#ifndef __Rl_DialogUtils_H__
#define __Rl_DialogUtils_H__

//--	Utils.h
#include "DialogPrerequisites.h"

/* Some helper functions, don't know where they should belong to */

//#include <string>

//using namespace std;
namespace rl
{
	CeGuiString toUpper(const CeGuiString &str);
	CeGuiString toLower(const CeGuiString &str);
	CeGuiString trim(const CeGuiString &str);
	bool checkHeadWS(const CeGuiString &);
	bool checkTailWS(const CeGuiString &);
	CeGuiString normalise(const CeGuiString &);
}
#endif
