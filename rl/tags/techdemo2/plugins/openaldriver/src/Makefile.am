INCLUDES = $(OGRE_CFLAGS) $(CEGUI_CFLAGS) $(RUBY_CFLAGS)  -I../include \
	-I../../../engine/ui/include -I../../../engine/core/include -I../../../engine/multimedia/include \
	-I../../../engine/common/include

lib_LTLIBRARIES = libRlOalDriver.la
libRlOalDriver_la_LDFLAGS = $(RUBY_LIBS) $(OGRE_LIBS) $(CEGUI_LIBS) $(LIBS) \
	-L../../../engine/common/src -L../../../engine/core/src -L../../../engine/ui/src \
	-L../../../engine/multimedia/src -L../../../engine/rules/src -L../../../engine/dialog/src \
	-lRlCommon -lRlCore -lRlMultimedia -lRlUi -lRlRules -lRlDialog -lruby1.8
libRlOalDriver_la_SOURCES = \
	OalDriver.cpp \
	OpenALConfig.cpp \
	OpenALDriverDllMain.cpp \
	OalListener.cpp \
	OalSoundChannel.cpp \
	OalSoundSample.cpp \
	OalSoundStream.cpp
	