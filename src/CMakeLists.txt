cmake_minimum_required(VERSION 3.21)

project(
	my_ps1_game
	LANGUAGES    C CXX ASM
	VERSION      1.0.0
	DESCRIPTION  "my_ps1_game"
	HOMEPAGE_URL "http://lameguy64.net/?page=psn00bsdk"
)

file(GLOB _sources *.c)
file(GLOB _lib_sources libs/*.c)

psn00bsdk_add_executable(my_ps1_game GPREL ${_sources} ${_lib_sources})

psn00bsdk_add_cd_image(
	iso         # Target name
	my_ps1_game # Output file name (= my_ps1_game.bin + my_ps1_game.cue)
	iso.xml     # Path to config file
	DEPENDS my_ps1_game system.cnf
)
