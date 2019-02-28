add_library(WaveSabrePlayerLib
	src/CriticalSection.cpp
	src/DirectSoundRenderThread.cpp
	src/IPlayer.cpp
	src/PreRenderPlayer.cpp
	src/RealtimePlayer.cpp
	src/SongRenderer.cpp
	src/SongRenderer.Track.cpp
	src/WavWriter.cpp)

target_link_libraries(WaveSabrePlayerLib
	WaveSabreCore
	winmm.lib
	dsound.lib)

target_include_directories(WaveSabrePlayerLib PUBLIC include)
