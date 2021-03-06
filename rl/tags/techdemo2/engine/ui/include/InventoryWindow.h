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

#ifndef __InventoryWindow_H__
#define __InventoryWindow_H__

#include "UiPrerequisites.h"
#include "CeGuiWindow.h"
#include "Inventory.h"
#include "GameTask.h"

namespace rl {

	class Creature;

	class _RlUiExport InventoryArrangeTask : public GameTask
	{
	public:
		InventoryArrangeTask();
		~InventoryArrangeTask();
		
		void run(Ogre::Real elapsedTime);
	};

	class _RlUiExport InventoryWindow : public CeGuiWindow
	{
	public:
		InventoryWindow();
		~InventoryWindow();

		// Flag f�r den ArrangeTask - FIXME nach private umziehen, wenn 
		CEGUI::DragContainer* mDroppedItem;

		/**
		* Gibt dem Fenster das anzuzeigende Inventar
		*/
		void setInventory(Inventory* inventory);

		/**
		* Methode f�r das OnMouseOver Event
		*/
		bool handleMouseEnters(const CEGUI::EventArgs &args);
		
		/**
		* Methode f�r das OnMouseLeaves Event
		*/
		bool handleMouseLeaves(const CEGUI::EventArgs &args);

		/**
		* Methode f�r das OnMouseClick Event
		*/
		bool handleMouseClicked(const CEGUI::EventArgs &args);

		/**
		* 
		*/
		void update();

		/**
		* �bergibt �nderungen ans Inventar
		*/
		void updateInventory();

		/**
		* Passt die Position eines gedroppten Items an, da CEGUI Bug, braucht man 
		* daf�r ne externe Methode
		*/
		void updateItemPosition();

		// FIXME, wenn BUG im CEGUI behoben ist: entfernen

		/**
		* Schaut nach, ob das Item an der Position gedroppt werden kann.
		*/
		bool isFreeInContainer(Item* item, pair<int,int> kaestechenPos, Item* container);


		Item* getGroundItem();

	private:
		Inventory* mInventory;

		// Farbschema f�r die Fenster und Itemhintergr�nde
		const CeGuiString mColorAccept;
		const CeGuiString mColorReject;
		const CeGuiString mColorNormal;
		const CeGuiString mColorItemNormal;
		const CeGuiString mColorItemSelected;

		// TODO : Ersetze durch etwas dynamisches
		Item* mGroundItem;

		// Der Knoten an dem das zu redernde Item angebracht wird;
		Ogre::SceneNode* mInventoryItemNode;
		// Entity des jeweils zu rendernden Items
		Ogre::Entity* mRenderItemEntity;
		// 128*128 Textur f�r das Item-view Fenster
		Ogre::RenderTexture* mRenderTexture;
		// Viewport des RenderToTexture Features
		Ogre::Viewport* mRenderViewport;
		// Die Camera, die das Item filmt
		Actor* mCameraActor;

		CEGUI::Texture* mTexture;
		CEGUI::Imageset* mImageSet;

        InventoryArrangeTask* mInventoryArrangeTask;

		// Das Item-Renderfenster
		CEGUI::StaticImage* mItemRenderImage;


		// Das R�stungs- und Behinderungswert Fenster
		CEGUI::StaticImage* mArmorValueWindow;
		CEGUI::StaticText* mArmorValue;
		CEGUI::StaticText* mHandicapValue;

		// Das Gewichtsfenster
		CEGUI::StaticImage* mWeightWindow;
		CEGUI::StaticText* mTotalWeight;
		

		// Das Item-Beschreibungsfenster
		CEGUI::StaticText* mDescription;

		// Das jeweils aktive Fenster, das im Beschreibungsfenster angew�hlt ist
		CEGUI::Window* mActiveItemWindow;

		// Das Fenster, in das der Rucksackcontainer soll
		CEGUI::TabControl* mContainerTabs;

		// Die Containerslots (besonders, da alle Arten von Items akzeptiert)
		std::list<CEGUI::Window*> mContainerContents;

		std::list<CEGUI::Window*> mContainers;

		CEGUI::Window* mGroundTab;
		CEGUI::Window* mGroundContainer;

		const std::pair<int,int> mGroundDimension;

		// Alle verschiedenen Inventoryslots
		CEGUI::StaticImage* mRingLeft;
		CEGUI::StaticImage* mRingRight;
		CEGUI::StaticImage* mHandLeft;
		CEGUI::StaticImage* mHandRight;
		CEGUI::StaticImage* mGloves;
		CEGUI::StaticImage* mBraceletLeft;
		CEGUI::StaticImage* mBraceletRight;
		CEGUI::StaticImage* mArmor;
		CEGUI::StaticImage* mCape;
		CEGUI::StaticImage* mBracers;
		CEGUI::StaticImage* mBackpack;
		CEGUI::StaticImage* mBelt;
		CEGUI::StaticImage* mNecklace;
		CEGUI::StaticImage* mHelmet;
		CEGUI::StaticImage* mTrousers;
		CEGUI::StaticImage* mShinbone;
		CEGUI::StaticImage* mBoots;

		CEGUI::Point mPosDraggedTo;
		CEGUI::Window* mContainerDraggedTo;
		CEGUI::DragContainer* mOldItemInSlot;

		/** 
		* Erzeugt eine Itemrepr�sentation im Inventarfenster
		*/
		CEGUI::DragContainer* createItem(Item* item, CEGUI::Window* parent, CEGUI::UVector2 position = CEGUI::UVector2(CEGUI::cegui_reldim(0.0f), CEGUI::cegui_reldim(0.0f)));

		/**
		* Schliesst das Fenster bei Klick auf X
		*/
		bool handleClose();

		void initRenderToTexture();

		/**
		* L�dt die Rtt-Camera
		*/
		void loadCamera();
		void unloadCamera();

		/**
		* Rendert das Item in das Vorschaufenster im Inventar
		*/
		void renderItem(Item* item);

		/**
		* Initiiert die Slots des Inventars
		*/
		void initSlots();

		/**
		* F�llt die Slots im InventarFenster mit den Items aus dem Inventar
		*/
		void fillSlots();

		/**
		* Erzeugt ein Tab f�r den Container, und befuellt ihn nach Inhalt des container - Items
		*/
		void createAndFillContainer(Item* container);

		/**
		* entfernt ein ContainerTab mitsamt Inhalt aus dem Inventar
		*/
		void removeContainerAndContent(Item* container);

		/**
		* Entleert die Slots im InventarFenster wieder
		*/
		void emptySlots();


		void refreshTabs();

		/**
		* Sucht unter den Containern dasjenige, das zum gesuchten Container geh�rt
		*/
		CEGUI::Window* findContainer(Item* container);

		/**
		*
		* Entfernt alle Unterfenster
		*/
		void emptySlot(CEGUI::Window* slot);
		/** 
		* Gibt dem Slot die F�higkeit Dragcontainer zu akzeptieren
		*/
		void addDropListener(CEGUI::Window* slot);

		/*!
		* f�gt dem Inventar den Container f�r den Rucksack zu,
		* gibt ihm die n�tige Funktionalit�t f�r drag&drop
		*/
		void initBackpack(std::pair<int,int> dim);


		/**
		* erzeugt f�r alle Items im Inventar, die Containerfunktionaltiaet haben, ein Containertab
		* im Inventar
		*/
		void createContainerWindows();

		/**
		* �berpr�ft, ob das gedroppte Item vom Slot akzeptiert wird oder nicht
		* @return true: Item wird akzeptiert
		*         false: Item wird nicht akzeptiert
		*/ 
		bool checkTypeAccepted(CEGUI::Window* window, CEGUI::DragContainer* draggedItem);


		/**
		* Errechnet die K�stchenposition, an der das Item im Container gedroppt wurde
		*/
		std::pair<int,int> calculateNewPosition(const CEGUI::DragDropEventArgs& ddea);

		bool handleDragEnter(const CEGUI::EventArgs& args);
		bool handleDragLeave(const CEGUI::EventArgs& args);
		bool handleDragDropped(const CEGUI::EventArgs& args);
	};
}
#endif
