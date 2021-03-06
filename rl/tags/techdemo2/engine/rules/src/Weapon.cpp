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

#include "DsaManager.h"
#include "Weapon.h"

using namespace std;

namespace rl
{
	Weapon::Weapon(const CeGuiString weaponName, const CeGuiString description)
		: Item(weaponName, description)
	{
	}

	Weapon::Weapon(const CeGuiString weaponName, const CeGuiString description, 
			const CeGuiString imageName, const CeGuiString meshName,
			Item::ItemType type, std::pair<int,int> size)
			: Item(weaponName, description, imageName, meshName, type, size)
	{
	}

	Weapon::~Weapon(void)
	{
	}


	Weapon* Weapon::clone()
	{
		Weapon* weapon = new Weapon(mName, mDescription, 
			mImageName, mMeshName, mItemType, mSize);
		weapon->setContainer(mIsContainer,mCapacity);
		weapon->setTp(mTp.first,mTp.second,mTp.third);
		weapon->setTpKk(mTpKk.first,mTpKk.second);
		weapon->setBf(mBf);
		weapon->setIni(mIni);
		weapon->setWm(mWm);
		weapon->setDk(mDk);
		weapon->setWeight(mWeight);
		weapon->setKampftechnik(mKampftechnik);
		weapon->setPrice(mPrice);

		return weapon;
	}

	const CeGuiString Weapon::getDescription() const
	{
		CeGuiString returnString = mName+ "\r\n\r\n" +
			getFormattedText(mDescription) + "\r\nSchaden: ";
		// Schadensberechnung
		if (mTp.first > 0) 
		{
			returnString += Ogre::StringConverter::toString(mTp.first) + " W6";
			if (mTp.second > 0)
			{
				returnString +=" + ";
			}
		}
		if (mTp.second > 0)
		{
			returnString += Ogre::StringConverter::toString(mTp.second) + " W20";
		}
		if (mTp.third > 0) {
			returnString += " +" + Ogre::StringConverter::toString(mTp.third);
		}
		// TP/KK
		returnString += "\r\nTP/KK: " + Ogre::StringConverter::toString(mTpKk.first) + 
			"/"+ Ogre::StringConverter::toString(mTpKk.second);
		return returnString;
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

	void Weapon::setKampftechnik(const CeGuiString newKampftechnik)
	{
		mKampftechnik = newKampftechnik;
	}

	const CeGuiString Weapon::getKampftechnik() const
	{
		return mKampftechnik;
	}

	const int Weapon::getDkDistanceToOptimum(Ogre::Real distance) const
	{
		if (DsaManager::getSingleton().isRuleActive(DsaManager::DISTANZKLASSEN))
		{
			//Man kann auch auf +/- 1 DK angreifen, siehe MBK 22
			if (DsaManager::getSingleton().isDkDistance(mDk, distance))
				return 0;

			if (DsaManager::getSingleton().isDkDistance(
						static_cast<Weapon::Distanzklasse>(mDk + 1), distance))
				return +1;

			if (DsaManager::getSingleton().isDkDistance(
						static_cast<Weapon::Distanzklasse>(mDk - 1), distance))
				return -1;

			if (DsaManager::getSingleton().isDkDistance(
						static_cast<Weapon::Distanzklasse>(mDk + 2), distance))
				return -2;

			if (DsaManager::getSingleton().isDkDistance(
						static_cast<Weapon::Distanzklasse>(mDk - 2), distance))
				return -2;

			if (DsaManager::getSingleton().isDkDistance(
						static_cast<Weapon::Distanzklasse>(mDk - 3), distance))
				return -3;

			return +3;
		}
		else
		{
			// Ohne DK-Regel
			// DK der Waffe >= Entfernung
			if (DsaManager::getSingleton().isDkDistance(mDk, distance)
				|| DsaManager::getSingleton().isDkDistance(
					static_cast<Weapon::Distanzklasse>(mDk - 1), distance)
				|| DsaManager::getSingleton().isDkDistance(
					static_cast<Weapon::Distanzklasse>(mDk - 2), distance)
				|| DsaManager::getSingleton().isDkDistance(
					static_cast<Weapon::Distanzklasse>(mDk - 3), distance))
			{
				return 0;
			}

			return +3;
		}
	}
}

