project("WaveSabre")

cmake_minimum_required(VERSION 3.11)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(MSVC)
	# disable exceptions globally (will be added back for VSTs)
	string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

	if(${CMAKE_VS_PLATFORM_NAME} STREQUAL "x64")
		add_compile_definitions($<$<CONFIG:MinSizeRel>:NEED_MAINCRTSTARTUP>)
	endif()
endif()

set(VSTSDK3_DIR "./Vst3.x/" CACHE PATH "VSTSDK location")

# shared code
add_subdirectory(MSVCRT)
add_subdirectory(WaveSabreCore)
add_subdirectory(WaveSabrePlayerLib)
add_subdirectory(WaveSabreVstLib)

# binaries
add_subdirectory(Tests/PlayerTest)
add_subdirectory(WaveSabreStandAlonePlayer)
add_subdirectory(Vsts)
