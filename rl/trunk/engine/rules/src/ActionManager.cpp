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

#include "ActionManager.h"
#include "Action.h"
#include "Exception.h"

template <>
rl::ActionManager* Singleton<rl::ActionManager> ::ms_Singleton = 0;
namespace rl
{

	ActionVector::iterator findAction(ActionVector::iterator begin, ActionVector::iterator end, const CeGuiString& actionName)
	{
		for (ActionVector::iterator it = begin; it != end; it++)
			if ((*it)->getName().compare(actionName) == 0)
				return it;

		return end;
	}

	ActionVector::const_iterator findActionConst(ActionVector::const_iterator begin, ActionVector::const_iterator end, const CeGuiString& actionName)
	{
		for (ActionVector::const_iterator it = begin; it != end; it++)
			if ((*it)->getName().compare(actionName) == 0)
				return it;

		return end;
	}

    ActionManager& ActionManager::getSingleton(void)
    {
    return Singleton<ActionManager>::getSingleton();
    }

    ActionManager* ActionManager::getSingletonPtr(void)
    {
        return Singleton<ActionManager>::getSingletonPtr();
    }

    ActionManager::ActionManager()
    {
    }

    ActionManager::~ActionManager()
    {
    }

    void ActionManager::registerAction(Action* action)
    {
        if (action == NULL)
        {
            Throw(NullPointerException, "Parameter action ist NULL.");
        }

        mActions.push_back(action);
    }

	void ActionManager::unregisterAction(const CeGuiString& actionName)
	{
		ActionVector::iterator iter = findAction(mActions.begin(), mActions.end(), actionName);
		if (iter == mActions.end())
			Throw(InvalidArgumentException, "Aktion nicht gefunden");
		mActions.erase(iter);
	}

	Action* ActionManager::getAction(const CeGuiString& actionName) const
	{
		ActionVector::const_iterator iter = 
			findActionConst(
				mActions.begin(), 
				mActions.end(), 
				actionName);
		if (iter == mActions.end())
			Throw(InvalidArgumentException, "Aktion nicht gefunden");
		return *iter;
	}


	void ActionManager::registerInGameGlobalAction(Action* action, ActionGroup* group)
	{
		mInGameGlobalActions.push_back(action);
		action->setGroup(group);
	}

	Action* ActionManager::getInGameGlobalAction(const CeGuiString& actionName) const
	{
		ActionVector::const_iterator iter = 
			findActionConst(
				mInGameGlobalActions.begin(), 
				mInGameGlobalActions.end(), 
				actionName);
		if (iter == mInGameGlobalActions.end())
			Throw(InvalidArgumentException, "Aktion nicht gefunden");
		return *iter;
	}

	const ActionVector& ActionManager::getInGameGlobalActions()
	{
		return mInGameGlobalActions;
	}
}
