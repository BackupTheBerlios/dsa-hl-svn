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

#include "CompositeEffect.h"

namespace rl
{
	CompositeEffect::CompositeEffect() : Effect()
	{
	}

	CompositeEffect::~CompositeEffect()
	{
		disable();
		for (Effects::iterator it = mEffects.begin(); it != mEffects.end(); it++)
		{
			delete (*it);
		}
	}

	void CompositeEffect::addEffect(rl::Effect *effect)
	{
		mEffects.insert(effect);
	}

	bool CompositeEffect::isAlive()
	{
		bool alive = false;
		for (Effects::iterator it = mEffects.begin(); it != mEffects.end(); it++)
		{
			if ((*it)->isAlive()) alive = true;
		}
		return alive;
	}

	void CompositeEffect::apply()
	{
		for (Effects::const_iterator it = mEffects.begin(); it != mEffects.end(); it++)
		{
			(*it)->enable();
		}
	}

	void CompositeEffect::remove()
	{
		for (Effects::const_iterator it = mEffects.begin(); it != mEffects.end(); it++)
		{
			(*it)->disable();
		}
	}
}
