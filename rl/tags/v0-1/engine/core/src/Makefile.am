INCLUDES = $(RUBY_CFLAGS) $(OGRE_CFLAGS) $(CEGUI_CFLAGS) $(STLPORT_CFLAGS) -I../include -I../../common/include -I../../sound/include

lib_LTLIBRARIES = libRlCore.la
libRlCore_la_LDFLAGS = -L../../sound/src -lRlSound $(OGRE_LIBS) $(CEGUI_LIBS) $(RUBY_LIBS) $(LIBS)
libRlCore_la_SOURCES = \
	Actor.cpp \
	ActorControlledObject.cpp \
	ActorManager.cpp \
	Animation.cpp \
	AnimationListener.cpp \
	AnimationManager.cpp \
	CameraObject.cpp \
	ConfigurationManager.cpp \
	ConfigurationManagerLinux.cpp \
	ConfigurationManagerWin32.cpp \
	ConfigurationManagerMac.cpp \
	CoreSubsystem.cpp \
	DeletionPropagator.cpp \
	DotSceneLoader.cpp \
	DotSceneOctreeWorld.cpp \
	GameAreaEvent.cpp \
	GameAreaEventSource.cpp \
	GameAreaListener.cpp \
	GameAreaTypes.cpp \
	GameEventManager.cpp \
	GameLoop.cpp \
	LightObject.cpp \
	ListenerObject.cpp \
	MeshObject.cpp \
	ParticleSystemObject.cpp \
	PhysicsContactListener.cpp \
	PhysicalThing.cpp \
	PhysicsGenericContactCallback.cpp \
	PhysicsManager.cpp \
	PhysicsMaterialRaycast.cpp \
	Primitive.cpp \
	RubyInterpreter.cpp \
	ScriptObject.cpp \
	ScriptObjectRepository.cpp \
	SoundObject.cpp \
	SoundPlaylist.cpp \
	TrackAnimation.cpp \
	World.cpp 		 

