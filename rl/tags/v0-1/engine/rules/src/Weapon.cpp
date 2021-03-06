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

#include "Weapon.h"

using namespace std;

namespace rl
{
	Weapon::Weapon(const CeGuiString& weaponName, const CeGuiString& description)
		: Item(weaponName, description)
	{
	}

	Weapon::~Weapon(void)
	{
	}

	void Weapon::setTp(int d6, int d20, int mod)
	{
		mTp.first = d6;
		mTp.second = d20;
		mTp.third = mod;
	}

	const Tripel<int>& Weapon::getTp() const
	{
		return mTp;
	}

	void Weapon::setTpKk(int base, int step)
	{
		mTpKk.first = base;
		mTpKk.second = step;
	}

	const pair<int, int>& Weapon::getTpKk() const
	{
		return mTpKk;
	}

	void Weapon::setBf(int newBf)
	{
		mBf = newBf;
	}

	int Weapon::getBf()
	{
		return mBf;
	}

	void Weapon::setIni(int newIni)
	{
		mIni = newIni;
	}

	int Weapon::getIni()
	{
		return mIni;
	}

	void Weapon::setWm(pair<int, int>& newWm)
	{
		mWm = newWm;
	}

	const pair<int, int>& Weapon::getWm() const
	{
		return mWm;
	}

	void Weapon::setDk(Weapon::Distanzklasse newDk)
	{
		mDk = newDk;
	}

	Weapon::Distanzklasse Weapon::getDk()
	{
		return mDk;
	}

	void Weapon::setKampftechnik(const CeGuiString& newKampftechnik)
	{
		mKampftechnik = newKampftechnik;
	}

	const CeGuiString& Weapon::getKampftechnik() const
	{
		return mKampftechnik;
	}
}

