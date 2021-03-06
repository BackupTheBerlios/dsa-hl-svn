PROJECT(RL)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2 FATAL_ERROR)

# Package and version information
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Rastullahs Lockenpracht ist ein auf DSA Regeln basierendes (Rollen-)Spiel")
SET(CPACK_PACKAGE_VENDOR "Team Pantheon")
SET(CPACK_PACKAGE_CONTACT "contact@rastullahs-lockenpracht.de")

SET(RL_MAJOR_VERSION 0)
SET(RL_MINOR_VERSION 3)
SET(RL_PATCH_VERSION 0)

SET(CPACK_PACKAGE_VERSION_MAJOR ${RL_MAJOR_VERSION})
SET(CPACK_PACKAGE_VERSION_MINOR ${RL_MINOR_VERSION})
SET(CPACK_PACKAGE_VERSION_PATCH ${RL_PATCH_VERSION})

IF(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
	SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
ELSEIF(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i686")
	SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE i386)
ENDIF(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")

SET(CPACK_SOURCE_GENERATOR "TBZ2;ZIP")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME
"${CMAKE_PROJECT_NAME}-${RL_MAJOR_VERSION}.${RL_MINOR_VERSION}.${RL_PATCH_VERSION}")

IF(WIN32 AND NOT UNIX)
	# TEMPLATE - Not Tested!
	# There is a bug in NSI that does not handle full unix paths properly. Make
	# sure there is at least one set of four (4) backlasshes.
	SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
	SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\RL_setup.exe")
	SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} RL")
	SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.rastullahs-lockenpracht.de")
	SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.wiki.team-pantheon.de")
	SET(CPACK_NSIS_CONTACT "contact@rastullahs-lockenpracht.de")
	SET(CPACK_NSIS_MODIFY_PATH ON)
ELSE(WIN32 AND NOT UNIX)
	SET(CPACK_STRIP_FILES "bin/RL")
	SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF(WIN32 AND NOT UNIX)

#Enable absolute paths
SET(CPACK_SET_DESTDIR "ON")

# Include required cmake macros
INCLUDE(FeatureSummary)
INCLUDE(InstallRequiredSystemLibraries)
INCLUDE(CPack)

# Add directory with RL's own macro definitions
# (checked before ${CMAKE_ROOT}/Modules)
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# Set convenience variables for Rastullah's own include directories
SET(RL_AI_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/engine/ai/include INTERNAL)
SET(RL_COMMON_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/engine/common/include INTERNAL)
SET(RL_CORE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/engine/core/include INTERNAL)
SET(RL_RULES_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/engine/rules/include INTERNAL)
SET(RL_SCRIPT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/engine/script/include INTERNAL)
SET(RL_UI_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/engine/ui/include INTERNAL)

# Check for required components
FIND_PACKAGE(PkgConfig)
FIND_PACKAGE(XercesC REQUIRED)
FIND_PACKAGE(OGRE 1.6 REQUIRED)
FIND_PACKAGE(OIS REQUIRED)
FIND_PACKAGE(CEGUI REQUIRED)
FIND_PACKAGE(CEGUIOGRE 1.6 REQUIRED)
FIND_PACKAGE(FMODEx REQUIRED)
FIND_PACKAGE(OgreNewt 2.0 REQUIRED)
FIND_PACKAGE(OpenSteer REQUIRED)
FIND_PACKAGE(MeshMagick 0.5 REQUIRED)
FIND_PACKAGE(Ruby REQUIRED)
FIND_PACKAGE(Boost 1.34 REQUIRED filesystem)
FIND_PACKAGE(SWIG REQUIRED)
IF (NOT SWIG_DIR MATCHES -rl$)
	SET_FEATURE_INFO(SWIG "" "" "You use an unsupported version of SWIG.")
ENDIF(NOT SWIG_DIR MATCHES -rl$)

# Compile settings for build types
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -DDEBUG")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DDEBUG")

# Path options
SET(BINDIR "bin" CACHE STRING "Where to install binaries")
SET(DATAROOTDIR "${CMAKE_INSTALL_PREFIX}/share" CACHE STRING "Sets the root of data directories to a non-default location")
SET(DATADIRNAME "rastullah" CACHE STRING "change the name of the directory for the read-only architecture-independent game data")

# Game options
SET(WITH_FMOD4 ON CACHE BOOL "Enable FMOD 4.xx (FMODex) sound support")
SET(WITH_OPENAL OFF CACHE BOOL "Enable OpenAL sound support (unsupported)")

# Include subdirectories
ADD_SUBDIRECTORY(engine)

PRINT_ENABLED_FEATURES()
