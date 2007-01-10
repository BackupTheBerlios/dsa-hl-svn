//@+leo-ver=4-encoding=iso-8859-1.
//@+node:@file PersonHandlerGenerator.h
//@+at
// PersonHandlerGenerator.h: Implementation der 
// Heldenerschaffung
// 
// $Revision: 1.22 $
// $Date: 2004/10/17 19:12:40 $
//@-at
//@@c

//@<< header >>
//@+node:<< header >>
//@+at
// Copyright (c) 2002-2004, Zdenko Denny Vrandecic and 
// contributors.
// All rights reserved.
// (The license follows closely the new 
// BSD-OpenSource-License)
// 
// Redistribution and use in source and binary forms, with 
// or without
// modification, are permitted provided that the following 
// conditions are met:
// Redistributions of source code must retain the above 
// copyright notice, this list of conditions and the 
// following disclaimer.
// Redistributions in binary form must reproduce the above 
// copyright notice, this list of conditions and the 
// following disclaimer in the documentation
// and/or other materials provided with the distribution.
// Neither the name of the copyrightholder or one of the 
// contributers to this source may be used to endorse or 
// promote products derived from this software without 
// specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
// CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED 
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
// PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS 
// BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
// (INCLUDING, BUT  NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// If you use this source, please inform me, so I can keep 
// track of how useful it really is. That would be very 
// appreciated.
// 
// The data distributed with this code is copyrighted by 
// Fantasy Production, Erkrath, Germany, and *not* subject 
// to the here given license. Distributing and using this 
// data is granted by FanPro for the purpose of using 
// computer driven tools for the role playing game "Das 
// Schwarze Auge" / "The Dark Eye". Any other use is only 
// allowed with prior written permission by FanPro.
//@-at
//@@c

#ifdef MSVC
#pragma warning(disable:4786 4503)
#endif

//@-node:<< header >>
//@nl

#if !defined(PERSONHANDLERGENERATOR_H)
#define PERSONHANDLERGENERATOR_H

//@<<  includes >>
//@+node:<<  includes >>
#include "PersonHandler.h"
#include "Regeln.h"
//@nonl
//@-node:<<  includes >>
//@nl

namespace DSA {

	//@  << class PersonHandlerGenerator >>
	//@+node:<< class PersonHandlerGenerator >>
	class PersonHandlerGenerator : public PersonHandler {
	public:
	  PersonHandlerGenerator();
	  virtual ~PersonHandlerGenerator() {}
	  virtual Person* P() { return person_.get(); }
	  virtual void P(std::auto_ptr<Person> person);
	  virtual std::auto_ptr<Person> HolePerson() { return person_; }
	
	  virtual std::string HandlerArt() const { return "Generator"; }
	
	  virtual bool NameSetzbar() { return true; }
	  virtual void Name(const std::string& name);
	  virtual bool IDSetzbar() { return true; }
	  virtual void ID(const std::string& id);
	  virtual void Rasse_(Rasse rasse) { Rasse_(rasse, false); };
	  virtual void Rasse_(Rasse rasse, bool reGen);
	  virtual bool RasseWaehlbar() { return true; }
	  virtual void Kultur_(Kultur kultur) { Kultur_(kultur, false); }
	  virtual void Kultur_(Kultur kultur, bool reGen);
	  virtual bool KulturWaehlbar() { return true; }
	  virtual void Profession_(Profession profession) { Profession_(profession, false); }
	  virtual void Profession_(Profession profession, bool reGen);
	  virtual bool ProfessionWaehlbar() { return true; }
	
	  virtual bool Steigerbar(Eigenschaftsart);
	  virtual bool Senkbar(Eigenschaftsart);
	  virtual void Steiger(Eigenschaftsart);
	  virtual void Senke(Eigenschaftsart);
	
	  virtual void BerechneEigenschaftenMaxima();
	
	  virtual bool Steigerbar(AbgeleiteterWertArt);
	  virtual bool Senkbar(AbgeleiteterWertArt);
	  virtual void Steiger(AbgeleiteterWertArt);
	  virtual void Senke(AbgeleiteterWertArt);
	
	  virtual bool ZauberAktivierbar();
	  virtual Zauberfertigkeiten AktivierbareZauber();
	  virtual bool TalentAktivierbar(const Talentgruppe gruppe = sonstige);
	  virtual void AktivierbareTalente(StringListe&, const Talentgruppe gruppe = sonstige);
	  virtual void Aktiviere(const std::string& talent);
	  virtual bool TalentSteigerbar(const std::string& talent);
	  virtual bool TalentSenkbar(const std::string& talent);
	  virtual void TalentSteigern(const std::string& talent);
	  virtual void TalentSenken(const std::string& talent);
	  virtual bool AufATSteigerbar(const std::string& kampftechnik);
	  virtual bool AufPASteigerbar(const std::string& kampftechnik);
	  virtual void AufATSteigern(const std::string& kampftechnik);
	  virtual void AufPASteigern(const std::string& kampftechnik);
	
	  virtual bool CharakteristikaHinzufuegbar(int modus0beide1vor2nach = 0);
	  virtual void HinzufuegbareCharakteristika(StringListe&, int modus0beide1vor2nach = 0);
	  virtual void FuegeHinzu(const std::string& c);
	  virtual bool CharakteristikumSteigerbar(const std::string& c);
	  virtual void CharakteristikumSteigern(const std::string& c);
	  virtual bool CharakteristikumSenkbar(const std::string& c);
	  virtual void CharakteristikumSenken(const std::string& c);
	
	  virtual bool SonderfertigkeitenHinzufuegbar();
	  virtual void HinzufuegbareSonderfertigkeiten(StringListe&);
	  virtual void FuegeHinzuSF(const std::string& sf);
	  virtual bool EntfernbarSF(const std::string& sf);
	  virtual void EntferneSF(const std::string& sf);
	  virtual bool SteigerbarSF(const std::string& sf);
	  virtual void SteigerSF(const std::string& sf);
	  virtual bool SenkbarSF(const std::string& sf);
	  virtual void SenkeSF(const std::string& sf);
	
	  virtual bool AlterSetzbar() { return true; }
	  virtual int Alter();
	  virtual bool GroesseSetzbar() { return true; }
	  virtual bool GewichtSetzbar() { return true; }
	  virtual int Gewicht();
	  virtual bool HauptfarbeSetzbar() { return true; }
	  virtual bool AugenfarbeSetzbar() { return true; }
	  virtual bool GeschlechtSetzbar() { return true; }
	
	  virtual bool BeschreibungSetzbar() { return true; }
	
	  virtual void PasseAn();
	  virtual void Abschliessen();
	
	  // Generatorschnittstelle (nicht sichtbar von PersonHandler aus)
	  virtual int GP();
	  virtual int GPMax()
	    { return regeln("Start-GP"); }
	  virtual int GPGuteEigenschaften(bool schreiben = false);
	  virtual int GPGuteEigenschaftenMax()
	    { return regeln("In Eigenschaften investierbare GP"); }
	  virtual int GPSchlechteEigenschaften();
	  virtual int GPSchlechteEigenschaftenMax()
	    { return regeln("Aus Schlechten Eigenschaften gewinnbare GP"); }
	  virtual int GPNachteile();
	  virtual int GPNachteileMax()
	    { return regeln("Aus Nachteilen gewinnbare GP"); }
	  virtual int AktivierteTalente();
	  virtual int AktivierteTalenteMax()
	    { return regeln("Anzahl bei Generierung aktivierbarer Talente"); }
	  virtual int AktivierteZauber();
	  virtual int AktivierteZauberMax();
	  virtual int TalentGP();
	  virtual int TalentGPMax();
	
	  virtual bool GenerierungBeendbar();
	  virtual bool GenerierungBedingungslosBeendbar();
	  virtual std::string GenerierungBeendbarText();
	  virtual std::string GenerierungBedingungslosBeendbarText();
	    
	  std::string gptext_;
	  int Hinzu(const std::string& text, int gp);
	    
	private:
	  std::auto_ptr<Person> person_;
	  std::string AutoID(const std::string& name);
	  bool autoID;
	  Talente talentebasis_;
	    
	  std::string talentbegabung_;
	  std::string talentgruppenbegabung_;
	  Talente begabtetalentegruppe_;
	  Kampftechniken begabtekampftechnikengruppe_;
	  std::string talentunfaehigkeit_;
	  std::string talentgruppenunfaehigkeit_;
	  Talente unfaehigetalentegruppe_;
	  Kampftechniken unfaehigekampftechnikengruppe_;
	  
	  Sprachen sprachenbasis_;
	  Schriften schriftenbasis_;
	  Kampftechniken kampftechnikenbasis_;
	  Zauberfertigkeiten zauberbasis_;
	  
	  Voraussetzungen voraussetzungen_;
	  Charakteristika chareigen_;
	  Charakteristika charbasis_;
	  Charakteristika charfinal_;
	  Sonderfertigkeiten sfeigen_;
	  Sonderfertigkeiten sfbasis_;
	  StringListe sfverbilligt_;
	  Sonderfertigkeiten sffinal_;
	  std::multimap<std::string, Voraussetzung> voraussetzungenliste_;
	
	  Modifikator& SOStartwert();
	
	  void Entferne(Modifikatoren modifikatoren, const std::string& grund);
	  void Setze(Modifikatoren modifikatoren, const std::string& grund);
	  void Entferne(Voraussetzungen voraussetzungen, const std::string& grund);
	  void Setze(Voraussetzungen voraussetzungen, const std::string& grund);
	  void ErzeugeVoraussetzungen();
	  void Entferne(Talente talente);
	  void Setze(Talente talente, bool reGen = false);
	  void Entferne(Kampftechniken kampftechniken);
	  void Setze(Kampftechniken kampftechniken, bool reGen = false);
	  void Entferne(Sprachen sprachen);
	  void Setze(Sprachen sprachen, bool reGen = false);
	  void Entferne(Schriften schriften);
	  void Setze(Schriften schriften, bool reGen = false);
	  void Entferne(Zauberfertigkeiten zauber);
	  void Setze(Zauberfertigkeiten zauber, bool reGen = false);
	  void SetzeMuttersprache(const StringListe& sprachen, bool muttersprache);
	  void EntferneMuttersprache(const StringListe& sprachen);
	  
	  // gibt zur�ck, wieviel GP die Charakteristika beitragen
	  int BerechneCharFinal(const bool nurNachteileGP = false, const bool reGen = false);
	  // dito, f�r die SF
	  int BerechneSFFinal(const bool reGen = false);
	
	  int AktivierungsKosten(const std::string& talent);
	
	  // R�ckgabewert: wenn zweites true dann Sprache, sonst Schrift.
	  // das erste bool bezeichnet, ob �berhaupt was gefunden wurde.
	  // Der eingegebene String wird zudem getrennt von dem Namen und
	  // nur der Name zur�ckgegeben (aus Sprachen kennen: Garethi wird Garethi
	  // mit R�ckgabewert true, aus Lesen / Schreiben: Chrmk Chrmk mit false)
	  std::pair<bool, bool> SpracheOderSchrift(std::string& spracheOderSchrift);
	
	};
	
	//@-node:<< class PersonHandlerGenerator >>
	//@nl
  
}

#endif // !defined(PERSONHANDLERGENERATOR_H)

//@<< Version History >>
//@+node:<< Version History >>
// $Log: PersonHandlerGenerator.h,v $
// Revision 1.22  2004/10/17 19:12:40  vrandezo
// IDs und Bezeichner getrennt - I18N vorbereitet
// ACHTUNG! Daten sichern. Das Programm ist noch reichlich fehlerhaft.
// Aber Entwicklung ohne CVS wird mir zu doof.
// Vorsicht vor allem mit Helden und RKP.
// Tausend Kleinigkeiten geaendert.
// Neue Datensaetze in basis.xdi.
//
// Revision 1.21  2004/04/12 21:28:18  vrandezo
// Extra Qualifikation entfernt
//
// Revision 1.19  2004/04/11 23:02:44  vrandezo
// - maechtig das Programm beschleunigt (Faktor wasweissich!)
// - Bug entfernt, der verhinderte, dass manche SF
// (mit Voraussetzung RK > bestimmter Wert) Personen
// hinzugefuegt werden konnten
//
// Revision 1.18  2004/04/10 20:22:41  vrandezo
// Schlechte Eigenschaften bei Re-Gen korrigiert
// Stufige VNT von RKP werden korrekt eingebunden
//
// Revision 1.17  2004/04/09 22:25:50  vrandezo
// Zauberfertigkeiten fuer Personen eintragbar
//
// Revision 1.16  2004/04/06 21:24:30  vrandezo
// GP werden bei Heldenerschaffung aufgeschl�sselt (Twelwan)
//
// Revision 1.15  2004/04/06 18:48:57  vrandezo
// Bug: altes for-scoping korrigiert
// Vorbereitung f�r GP-Aufschluesselung
//
// Revision 1.14  2004/04/06 13:57:59  vrandezo
// Begabung und Unfaehigkeit von Talenten und Talentgruppen jetzt
// nach Errata implementiert (Twel)
//
// Revision 1.13  2004/04/04 23:37:40  vrandezo
// Bug: beim Re-Generieren war der RK zu hoch
//
// Revision 1.12  2003/12/31 03:51:58  vrandezo
// Re-Generation erweitert
//
// Revision 1.11  2003/11/28 16:46:16  vrandezo
// Zeilenvorlauf am Dateiende (Warnung bei gcc)
//
// Revision 1.10  2003/10/30 00:02:44  vrandezo
// Steigerbare Ritualkenntnisse
//
// Revision 1.9  2003/10/19 21:59:07  vrandezo
// In Leo eingecheckt
//
// Revision 1.8  2003/03/25 12:41:48  vrandezo
// Begabung f�r [Talent]
//
// Revision 1.7  2003/02/23 12:36:27  vrandezo
// Charakteristika-Reiter umbenannt zu Vor-/Nachteile & SF (Tim von Umbscheiden)
// Generierung abschlie�bar, ohne alle TGP und GP verbraucht zu haben (Stefan Holzm�ller)
// das Schlie�en des Fensters bei der Auswahl von Talenten bei der Vergabe einer
//   Profession, Rasse oder Kultur w�hrend der Heldengenerierung f�hrt nicht mehr
//   zum Absturz (Stefan Holzm�ller)
// Neuer Vorteil: Fettleibig (Volker Strunk)
//
// Revision 1.6  2003/02/20 17:48:31  vrandezo
// Alter des Helden
// VN mit Voraussetzungen eintragbar
// Abgeleitete Wert als Voraussetzungen m�glich
//
// Revision 1.5  2003/02/20 13:53:22  vrandezo
// Vorteile: Beidh�ndig, Linksh�nder, Blutrausch, Kampfrausch
// weitere SF
// Eigenschaften haben Maximalwerte
//
// Revision 1.4  2003/02/20 00:21:58  vrandezo
// Sonderfertigkeiten vollst�ndig integriert
// schlafende SF
//
// Revision 1.3  2003/02/18 18:48:48  vrandezo
// SF bei Generierung
// Gel�ndekunden au�er der ersten f�r nur 2/3 des Preises
//
// Revision 1.2  2003/02/18 17:42:26  vrandezo
// verbesserte GUI f�r SF und f�r Charakteristika in Generierung
//
// Revision 1.1.1.1  2003/02/17 15:33:59  vrandezo
// Neue Verzeichnisstruktur
//
// Revision 1.18  2003/02/17 14:10:47  vrandezo
// Beschreibung eintragbar f�r Helden w�hrend der Generierung
// Entsprechende Vor- und Nachteile machen Modifikatoren statt im Basiswert mitberechnet zu werden
//
// Revision 1.17  2003/02/13 13:34:36  vrandezo
// steigerbarer VN ohne Wert in Generierung
//
// Revision 1.16  2003/02/08 00:22:54  vrandezo
// VN von RKP bei Generierung �bernommen
// mehrfache VN bei RKP geben GP
// erweitere Voraussetzungen: nicht VN
// entfernbare VN
// ausw�hlbare VN
// VN mit Voraussetzungen (nicht VN)
// Geschlecht neuerschaffener Helden ist zuf�llig
//
// Revision 1.15  2003/02/07 19:37:25  vrandezo
// Magische Professionen als Professionskategorie eingef�gt
// Voraussetzungen k�nnen sich selber �berpr�fen (Implementierung fehlt)
// Charakteristika mit Beschreibung
// Charakteristika GP, Beschreibung editierbar
// KompaktPanel-GUI f�r Charakteristika angelegt
// VN bei Genererierung hinzuf�gen
// VN Liste erstellt
// VN-GUI bei RKP erstellt
// einfache VN m�glich
//
// Revision 1.14  2003/01/26 16:49:05  vrandezo
// AT/PA jetzt bei Kampftechniken aufteilbar
//
// Revision 1.13  2003/01/26 13:42:22  vrandezo
// Voraussetzungen bei Generierung g�ltig
// Auswahl aus einer Talentgruppe m�glich bei Generierung
// Geschlecht bei Generierung implementiert
//
// Revision 1.12  2003/01/24 20:33:34  vrandezo
// �bliche Kulturen ausw�hlbar bei Heldenerschaffung
// Wert der Mutter- und Zweitsprache auch bei Modifikatoren korrekt
// Geschlecht wechselbar
//
// Revision 1.11  2003/01/19 01:05:22  vrandezo
// Mutter- und Zweitsprachen bei Heldenerschaffung vollst�ndig integriert
// Werte passen sich st�ndig der aktuellen Klugheit an
// Sprachen und Schriften in Heldenerschaffung integriert
// Bei m�glicher Talentaktivierung werden nur passende Talente angezeigt
//
// Revision 1.10  2003/01/18 18:10:42  vrandezo
// Personen k�nnen Sprachen und Schriften
// �bliche Kulturen bei Rassen hinzugef�gt
// GUI f�r m�gliche und �bliche Kulturen und f�r m�gliche Professionen ge�ndert
// Varianten ausblendbar bei der Eingabe m�glicher Kulturen und Professionen
//
// Revision 1.9  2002/12/06 15:28:01  vrandezo
// Spezies bei den Rassen einstellbar
// Kampftechniken bei PK und bei Person hinzugef�gt
//
// Revision 1.8  2002/11/03 17:48:36  vrandezo
// Haarfarbe und Augenfarbe jetzt auch bei der Heldenerschaffung
//
// Revision 1.7  2002/11/03 14:25:33  vrandezo
// Wuerfel angepasst
// Haar- und Augenfarbe hinzugef�gt bei Rassen
//
// Revision 1.6  2002/11/01 15:27:29  vrandezo
// Regeln neu implementiert
// ILader, MLader, IMLader, ILadbar und weitere Klassen entfernt
// XMLChar-Transcoding beschleunigt
//
// Revision 1.5  2002/10/18 00:50:16  vrandezo
// Talente bei der Heldenerschaffung einsetzbar
//
// Revision 1.4  2002/10/11 14:13:22  vrandezo
// Modifikatoren f�r Eigenschaften und Abgeleitete Werte eingef�hrt
//
// Revision 1.3  2002/10/02 01:28:57  vrandezo
// Erweiterung von GUI und DSA.lib
//
// Revision 1.2  2002/09/28 15:59:11  vrandezo
// Modifikatoren in GUI eingebunden
//
// Revision 1.1  2002/09/28 00:28:37  vrandezo
// Beginn des Generators mit DSA.lib und in wxWindows
//
//@nonl
//@-node:<< Version History >>
//@nl
//@-node:@file PersonHandlerGenerator.h
//@-leo
