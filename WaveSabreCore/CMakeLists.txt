add_library(WaveSabreCore
	src/Adultery.cpp
	src/AllPass.cpp
	src/AllPassDelay.cpp
	src/BiquadFilter.cpp
	src/Cathedral.cpp
	src/Chamber.cpp
	src/Comb.cpp
	src/Crusher.cpp
	src/DelayBuffer.cpp
	src/Device.cpp
	src/Echo.cpp
	src/Envelope.cpp
	src/Falcon.cpp
	src/GmDls.cpp
	src/Helpers.cpp
	src/Leveller.cpp
	src/MxcsrFlagGuard.cpp
	src/ResampleBuffer.cpp
	src/SamplePlayer.cpp
	src/Scissor.cpp
	src/Slaughter.cpp
	src/Smasher.cpp
	src/Specimen.cpp
	src/StateVariableFilter.cpp
	src/SynthDevice.cpp
	src/Thunder.cpp
	src/Twister.cpp)

target_link_libraries(WaveSabreCore Msacm32.lib)
target_include_directories(WaveSabreCore PUBLIC include)
