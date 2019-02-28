project("WaveSabre" CXX CSharp)

cmake_minimum_required(VERSION 3.13)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# C#/.NET defaults
set(CMAKE_CSharp_FLAGS "/langversion:default")
set(CMAKE_DOTNET_TARGET_FRAMEWORK_VERSION "v4.6.1")

# shared code
add_subdirectory(WaveSabreCore)
add_subdirectory(WaveSabrePlayerLib)
add_subdirectory(WaveSabreVstLib)
add_subdirectory(WaveSabreConvert)

# binaries
add_subdirectory(Tests/PlayerTest)
add_subdirectory(WaveSabreStandAlonePlayer)
add_subdirectory(Vsts)
add_subdirectory(ConvertTheFuck)
