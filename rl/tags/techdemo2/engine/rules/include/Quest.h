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

#ifndef __QUEST_H__
#define __QUEST_H__

#include "RulesPrerequisites.h"
#include <vector>


namespace rl {

class _RlRulesExport Quest;
class _RlRulesExport QuestBook;

typedef std::vector<Quest*> QuestVector;

#undef SUCCEEDED
/**
 * @brief Verwaltet einen Quest oder Subquest
 */
class _RlRulesExport Quest
{
public:
	enum State
	{
		OPEN = 0,
		ASSIGNED,
		FAILED,
		SUCCEEDED,
		COMPLETED
	};

	static CeGuiString STATE_NAMES[5];
	static CeGuiString KNOWN_NAMES[2];

	/**
	 * @param id eine einzigartige ID, um den Quest wiederzufinden
	 * @param name Titel des Quests
	 * @param description Lange Beschreibung, die im Tagebuch angezeigt wird
	 */
	Quest(const CeGuiString id, const CeGuiString name, const CeGuiString description);
	virtual ~Quest();

	/**
	 * @return die Quest-ID
	 */
	const CeGuiString getId();

	/**
	 * @return der Titel des Quests
	 */
	const CeGuiString getName();

	/**
	 * @return die Questbeschreibung
	 */
	const CeGuiString getDescription();

	/**
	 * @param partsToDo Anzahl der zu erledigenden Teile des Quests
	 */
	void setPartsToDo(int partsToDo);

	/**
	 * @return die Anzahl der zu erledigenden Teile des Quests oder, falls der 
	 * Quest Teilquests enthaelt, die Anzahl der Teilquests
	 */
	int getPartsToDo();

	/**
	 * @param partsDone Anzahl der erledigten Teile des Quests
	 */
	void setPartsDone(int partsDone);

	/**
	 * @return die Anzahl der zu erledigenden Teile des Quests oder, falls der 
	 * Quest Teilquests enthaelt, die Anzahl der Teilquests
	 */
	int getPartsDone();

	/**
	 * Erhoeht die geschafften gleichen Teile eines Quests
	 * 
	 * @parts wieviele Teile wurden erledigt (default 1)
	 */
	void increasePartsDone(int parts = 1);

	/**
	 * Verringert die geschafften gleichen Teile eines Quests
	 * 
	 * @parts wieviele weniger Teile wurden erledigt (default 1)
	 */
	void decreasePartsDone(int parts = 1);


	/**
	 * @return Status des Quests
	 */
	Quest::State getState();

	/**
	 * @return Status des Quests (als String)
	 */
	const CeGuiString getStateName();
	const CeGuiString getKnownName();

	/**
	 * @param Status des Quests
	 */
	void setState(Quest::State state);

	void setKnown(bool known);
	bool isKnown();

	/**
	 * @return <code>true</code>, wenn dieser Quest Subquests hat
	 *		   <code>false</code>, wenn dieser Quest atomar ist oder aus gleichen Teilen 
	 *         (getPartsTodo() > 1) besteht
	 */
	bool hasSubquests();

	/**
	 * @return die Subquests dieses Quests
	 */
	QuestVector getSubquests();

	/**
	 * Fuegt einen Subquest hinzu
	 * @param quest der Subquest
	 */
	void addSubquest(Quest* quest);

	Quest* getParent();
	
	void setQuestBook(QuestBook* questBook);

	static Quest::State getStateFromName(const CeGuiString stateName);
	static bool getKnownFromName(const CeGuiString knownName);
private:

	static const int KNOWN = 1;
	static const int UNKNOWN = 0;

	QuestVector mSubquests;
	CeGuiString mId;
	CeGuiString mName;
	CeGuiString mDescription;
	int mPartsToDo;
	int mPartsDone;
	bool mKnown;

	State mState;
	Quest* mParent;
	QuestBook* mQuestBook;

	void checkDone();
	void setParent(Quest* parentQuest);
	void notify( int reason );
};

}
#endif
