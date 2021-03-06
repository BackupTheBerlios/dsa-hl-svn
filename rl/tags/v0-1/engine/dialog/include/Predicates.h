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

#ifndef __RL_DialogPredicates_H__
#define __RL_DialogPredicates_H__

#include "DialogPrerequisites.h"

//#include <string>
#include <map>

//using namespace std;

//#define HISTORY_SIZE 5

namespace rl
{
/*	typedef struct 
	{
		int size;
		int top;
		map<int, string> elements;
	} History;

	typedef struct 
	{
		int number;
		int size;
		map<int, string> elements;
	} Array;

	typedef map<pair<string, string>, History> HistoryMap;
	typedef map<pair<string, string>, Array> ArrayMap;
*/
	/**
	 * Encapsulates the access to most of the properties and attributes of a bot
	 * Later, all Attributes and Skills of a DSA-Bot should or could be encapsulated
	 * in this class, too.
	 *
	 * @author	Philipp Walser
	 */
	class _RlDialogExport Predicates
	{
	public:
		Predicates();
		virtual ~Predicates();

		void addPredicate(const CeGuiString& name, const CeGuiString& type, const CeGuiString& value);
		void clearPredicate(const CeGuiString& name, const CeGuiString& type);
		const CeGuiString& getPredicate(const CeGuiString& name, const CeGuiString& type);
		void setPredicate(const CeGuiString& name, const CeGuiString& type, const CeGuiString& value);
		
	/*	static void loadPredicates();
		static void savePredicates();
	
		static void addValue(const string &property, const string &id, const string &value);
		static string setValue(const string &property, const string &id, const string &value, int index = 1);
		static void clearValue(const string &property, const string &id, int index = 1);
		static void clearValues(const string &property, const string &id);
		static string getValue(const string &property, const string &id, int index = 1);
		static string getValues(const string &property, const string &id, const string &conjunction = " and ");
	
		static void addHistory(const string &property, const string &id, const string &value);
		static string getHistory(const string &property, const string &id, int index = 1);
	
		static void setProperty(const string &property, const string &value);
		static string getProperty(const string &property);
	
		static void specializePredicate(const string &property, const string &value, bool returnName = false);
		static void setGlobalDefault(const string &value);
		static string lookupPredicate(const string &property, const string &defaultValue);
	*/
	private:
	
		std::map<CeGuiString, CeGuiString> mPredicates;
	/*	static HistoryMap history;
		static ArrayMap predicates;
		static map<string, string> properties;
	
		static map<string, pair<string, bool> > specializedPredicates;
		static string defaultPredicate;
	*/
	};
}
#endif
