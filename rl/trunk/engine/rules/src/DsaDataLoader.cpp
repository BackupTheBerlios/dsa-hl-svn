#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include "DsaDataLoader.h"

#include "DsaManager.h"
#include "Talent.h"
#include "Person.h"
#include "XmlHelper.h"
#include "Exception.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace std;

namespace rl {

	using XERCES_CPP_NAMESPACE::DOMDocument; //XXX: Warum brauche ich das unter VS 2003?

	void DsaDataLoader::loadData(string filename)
	{
		XMLPlatformUtils::Initialize();

		DOMDocument* doc = loadDataFile(filename);
		DOMElement* dataDocumentContent = XmlHelper::getChildNamed(doc->getDocumentElement(), "Inhalt");

		initializeTalente(XmlHelper::getChildNamed(dataDocumentContent, "Talente"));
		initializeKampftechniken(XmlHelper::getChildNamed(dataDocumentContent, "Kampftechniken"));
		initializePersonen(XmlHelper::getChildNamed(dataDocumentContent, "Personen"));
		
		doc->release();
		XMLPlatformUtils::Terminate();
		
	}

	DOMDocument* DsaDataLoader::loadDataFile(string filename)
	{

		XercesDOMParser* parser = new XercesDOMParser();
        parser->setValidationScheme(XercesDOMParser::Val_Always);    // optional.
        parser->setDoNamespaces(true);    // optional

/*        ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
        parser->setErrorHandler(errHandler);*/
		parser->parse(filename.c_str());
		return parser->getDocument();
	}

    void DsaDataLoader::initializeTalente(DOMElement* rootTalente)
    {
		if (rootTalente == NULL)
			return;

	    DOMNodeList* talentGruppen = rootTalente->getElementsByTagName(XMLString::transcode("Talentgruppe"));
        for (unsigned int gruppe = 0; gruppe < talentGruppen->getLength(); gruppe++)
		{
			DOMElement* gruppeData = reinterpret_cast<DOMElement*>(talentGruppen->item(gruppe));
			DOMNodeList* talenteXml = gruppeData->getElementsByTagName(XMLString::transcode("Talent"));
            int numTalent = 0;
            for (unsigned int talentIdx = 0; talentIdx < talenteXml->getLength(); talentIdx++)
            {
                Talent* t = processTalent(gruppe*100+numTalent, gruppe, reinterpret_cast<DOMElement*>(talenteXml->item(talentIdx)));
                numTalent++;
				DsaManager::getSingleton()._addTalent(t);
            }
		}
    }

    Talent* DsaDataLoader::processTalent(int id, int gruppe, DOMElement* talentXml)
    {
        const XMLCh* desc = XmlHelper::getChildNamed(talentXml, "Beschreibung")->getFirstChild()->getNodeValue();
        const XMLCh* probe = XmlHelper::getChildNamed(talentXml, "Probe")->getFirstChild()->getNodeValue();
        const XMLCh* art = XmlHelper::getChildNamed(talentXml, "Art")->getFirstChild()->getNodeValue();
		DOMElement* eBeNode = XmlHelper::getChildNamed(talentXml, "eBE");
		int ebe = EBE_KEINE_BE;
        if (eBeNode != NULL)
			ebe = getEBeFromString(XMLString::transcode(eBeNode->getFirstChild()->getNodeValue()));

        const XMLCh* name = talentXml->getAttribute(XMLString::transcode("ID"));
        string probeString = XMLString::transcode(probe);
        EigenschaftTripel eigenschaften;
		eigenschaften.first = DsaManager::getSingleton().getEigenschaftIdFromString(probeString.substr(0,2));
        eigenschaften.second = DsaManager::getSingleton().getEigenschaftIdFromString(probeString.substr(3,2));
        eigenschaften.third = DsaManager::getSingleton().getEigenschaftIdFromString(probeString.substr(6,2));
        Talent* t = new Talent(
            id, 
            XMLString::transcode(name),
            XMLString::transcode(desc),
            eigenschaften,
            ebe,
            gruppe);

        return t;
    }

	int DsaDataLoader::getEBeFromString(const string& eBeString)
	{
		if (eBeString.length() == 0)
			return EBE_KEINE_BE;

		if (!(eBeString.substr(0, 2).compare("BE")) == 0)
			Throw(InvalidArgumentException, "Ungueltige EBE-Angabe.");

		string ebe = eBeString.substr(2);
		if (ebe.compare("x2") == 0)
			return EBE_BEx2;
		if (ebe.compare("") == 0)
			return 0;
		return atoi(ebe.c_str());
	}

    void DsaDataLoader::initializeKampftechniken(DOMElement* rootKampftechniken)
    {
	}

	void DsaDataLoader::initializePersonen(DOMElement* rootPersons)
	{
		if (rootPersons == NULL)
			return;

		DOMNodeList* personenXml = 
			rootPersons->getElementsByTagName(XMLString::transcode("Person"));
		for (unsigned int idx = 0; idx < personenXml->getLength(); idx++)
			DsaManager::getSingleton()._addPerson(
				processPerson(
					idx+10000, 
					reinterpret_cast<DOMElement*>(personenXml->item(idx))));
	}

	Person* DsaDataLoader::processPerson(int id, DOMElement* personXml)
	{
		XMLCh* TALENT = XMLString::transcode("Talent");
		XMLCh* ID = XMLString::transcode("ID");
		
		string name = 
			XmlHelper::getValueAsString(XmlHelper::getChildNamed(personXml, "Name"));
		string desc = 
			XmlHelper::getValueAsString(XmlHelper::getChildNamed(personXml, "Beschreibung"));

		Person* rval = new Person(id, name, desc);
		
		// Talente, die direkt unter <Person> angeordnet sind, ergeben bereits die zusammengefassten Werte
		DOMNodeList* talente =			
			XmlHelper::getChildNamed(personXml, "Talente")->
				getElementsByTagName(TALENT);
		for (unsigned int idx = 0; idx < talente->getLength(); idx++)
		{
			DOMElement* talentXml = reinterpret_cast<DOMElement*>(talente->item(idx));
			Talent* tal = 
				DsaManager::getSingleton().getTalent(
					XMLString::transcode(talentXml->getAttribute(ID)));
			rval->addTalent(tal->getId());
			rval->setTalent(
				tal->getId(), 
				XmlHelper::getValueAsInteger(XmlHelper::getChildNamed(talentXml, "Wert")));
		}

		XMLString::release(&TALENT);
		XMLString::release(&ID);

		return rval;
	}
}
