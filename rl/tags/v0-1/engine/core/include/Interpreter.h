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

#ifndef __Interpreter_H__
#define __Interpreter_H__

#include <OgreSingleton.h>

#include "CorePrerequisites.h"

namespace rl {

class ScriptObject;

class _RlCoreExport Interpreter 
{
	public:
		virtual ~Interpreter() {};

		virtual bool execute(Ogre::String command) = 0;
		virtual void initializeInterpreter() = 0;

		virtual void setScript( const Ogre::String& instname,
		    const Ogre::String& scriptname,
		    const Ogre::String& classname,
		    int argc, const CeGuiString args[] = 0 ) = 0;
		virtual void callFunction( const Ogre::String& instname,
		    const Ogre::String& funcname, int argc,
		    const CeGuiString args[] = 0 ) = 0;
		virtual int callIntegerFunction( const Ogre::String& instname,
		    const Ogre::String& funcname, int argc,
		    const CeGuiString args[] = 0 ) = 0;

		virtual ScriptObject* getScriptObject( const Ogre::String& name ) = 0;

		virtual void registerScriptObject( ScriptObject* obj,
		    const Ogre::String& instname ) = 0;
		virtual void unregisterScriptObject( const Ogre::String& instname ) = 0;
		virtual void addSearchPath(const Ogre::String& path) = 0;
};

}
#endif

