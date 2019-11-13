project("WaveSabre")

cmake_minimum_required(VERSION 3.11)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(MSVC)
	# disable exceptions globally (will be added back for VSTs)
	string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
endif()

if (NOT WIN32)
	# we need libsndfile
	find_package(PkgConfig)
	pkg_check_modules(SNDFILE REQUIRED sndfile)
	add_definitions(-DUSE_SNDFILE)
endif()

# shared code
add_subdirectory(WaveSabreCore)
if(WIN32)
	add_subdirectory(WaveSabrePlayerLib)
	add_subdirectory(WaveSabreVstLib)
endif()

# binaries
if(WIN32)
	add_subdirectory(Tests/PlayerTest)
	add_subdirectory(WaveSabreStandAlonePlayer)
	add_subdirectory(Vsts)
endif()
