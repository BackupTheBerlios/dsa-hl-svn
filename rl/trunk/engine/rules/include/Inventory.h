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

#ifndef __INVENTORY_H__
#define __INVENTORY_H__

#include "Item.h"
#include "Weapon.h"
#include "Container.h"
#include "RulesPrerequisites.h"



namespace rl {
class _RlRulesExport Inventory;

typedef vector<Item*> ContainerColumn;
typedef vector<ContainerColumn> ContainerLayout;

/**
 * @brief Verwaltet das Inventar des Charakters
 */
class _RlRulesExport Inventory
{
public:
	Inventory();
	virtual ~Inventory();

	vector<Item*> getAllItems();

	set<Item*> getAllItemsInBackpack();

	vector< vector<Item*> > getBackpackLayout();

	/**
	 * @return Die errechnete Behinderung
	 **/
	int getCurrentBe();

	/**
	 * @brief f�gt das Item dem Inventar hinzu, wenn Platz ist,
	 * gibt sonst false zur�ck
	 * @param item Das Item, dass aufgenommen werden soll
	 **/
	bool addItem(Item* item);

	void addItemToBackpack(Item* item);

	void removeItemFromBackpack(Item* item);

	bool isFreeInBackpack(Item* item, pair<int,int> posKaestchen);

	void addWeapon(Weapon* weapon);

	Weapon* getWeapon(int weaponId);
	Weapon* removeWeapon(int weaponId);
	void switchToWeapon(int weaponId);

	void setItemBackpackPosition(Item* item, int xPosDraggedTo, int yPosDraggedTo);

	void removeItemFromSlots(Item* item);

	Item* createItem(const CeGuiString& name, const CeGuiString& description, const CeGuiString& imageName, Item::ItemType type, pair<int,int> size);

	//Ring links
	Item* getRingLeft();
	//Ring rechts
	Item* getRingRight();
	//in Hand links
	Item* getHandLeft();
	//in Hand rechts
	Item* getHandRight();
	//Handschuhe
	Item* getGloves();
	//Armreif links
	Item* getBraceletLeft();
	//Armreif rechts
	Item* getBraceletRight();
	//Oberkoerper Ruestung
	Item* getArmor();
	//Umhang
	Item* getCape();
	//Armschienen
	Item* getBracers();
	//Oberkoerper Ruecken (Rucksack, Schwertscheiden)
	Item* getBackpack();
	//Guertel  (+Slots fuer Guerteltaschen+Scheiden)
	Item* getBelt();
	//Halskette
	Item* getNecklace();
	//Stirnband, Helm, Diadem
	Item* getHelmet();
	//Hose
	Item* getTrousers();
	//Beinschienen
	Item* getShinbone();
	//Stiefel
	Item* getBoots();

	Item* removeRingLeft();
	Item* removeRingRight();
	Item* removeHandLeft();
	Item* removeHandRight();
	Item* removeGloves();
	Item* removeBraceletLeft();
	Item* removeBraceletRight();
	Item* removeArmor();
	Item* removeCape();
	Item* removeBracers();
	Item* removeBackpack();
	Item* removeBelt();
	Item* removeNecklace();
	Item* removeHelmet();
	Item* removeTrousers();
	Item* removeShinbone();
	Item* removeBoots();

	void setRingLeft(Item* item);
	void setRingRight(Item* item);
	void setHandLeft(Item* item);
	void setHandRight(Item* item);
	void setGloves(Item* item);
	void setBraceletLeft(Item* item);
	void setBraceletRight(Item* item);
	void setBracers(Item* item);
	void setArmor(Item* item);
	void setCape(Item* item);
	void setBackpack(Item* item);
	void setBelt(Item* item);
	void setNecklace(Item* item);
	void setHelmet(Item* item);
	void setTrousers(Item* item);
	void setShinbone(Item* item);
	void setBoots(Item* item);

private:

	/** @brief Liefert die n�chste freie Position f�r den Gegenstand zurueck.
	 *  @param space Die Ausma�e des Gegenstandes.
	 *  @return Die Anfangsposition des Platzesm an den das Item gesetzt werden kann.
	 *  @exception IllegalStateException Es gibt nicht genug Platz f�r das Item.
	 **/
	pair<int,int> findPositionWithEnoughSpace(pair<int,int> space);

	/**
	* �berpr�ft die Stelle im Inventar auf genug Platz f�r ein Item der Gr��e space
	*/
	bool checkSpace(int xStart, int yStart, pair<int,int> space);

	Item* mRingLeft;
	Item* mRingRight;
	Item* mHandLeft;
	Item* mHandRight;
	Item* mGloves;
	Item* mBraceletLeft;
	Item* mBraceletRight;
	Item* mArmor;
	Item* mCape;
	Item* mBracers;
	Item* mBackpack;
	Item* mBelt;
	Item* mNecklace;
	Item* mHelmet;
	Item* mTrousers;
	Item* mShinbone;
	Item* mBoots;

	ContainerLayout mBackpackLayout;
	pair<int,int> mBackpackDimension;

	// Bef�llt die Slots mit Nullpointern
	void initSlots();
};

}
#endif


