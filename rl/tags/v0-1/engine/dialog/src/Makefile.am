INCLUDES = $(RUBY_CFLAGS) $(OGRE_CFLAGS) $(CEGUI_CFLAGS) $(STLPORT_CFLAGS) -I../include -I../../common/include -I../../ui/include -I../../core/include -I../../rules/include 

lib_LTLIBRARIES = libRlDialog.la
libRlDialog_la_LDFLAGS = $(RUBY_LIBS) $(OGRE_LIBS) $(LIBS) $(CEGUI_LIBS) 
libRlDialog_la_SOURCES = \
	AimlCoreComponent.cpp \
	AimlDocumentImplXerces.cpp \
	AimlNode.cpp \
	AimlNodeImplXerces.cpp \
	AimlParser.cpp \
    AimlParserImplXerces.cpp \
	AimlProcessorManager.cpp \
	BotParser.cpp \
	DialogCharacter.cpp \
	DialogResponse.cpp \
	DialogScriptObject.cpp \
	DialogSubsystem.cpp \
	Graphmaster.cpp \
	Match.cpp \
	NaturalLanguageProcessor.cpp \
	Nodemaster.cpp \
	Predicates.cpp \
	StringTokenizer.cpp \
	Utils.cpp \
	processors/ConditionProcessor.cpp \
	processors/GetProcessor.cpp \
	processors/SetProcessor.cpp \
	processors/SraiProcessor.cpp
