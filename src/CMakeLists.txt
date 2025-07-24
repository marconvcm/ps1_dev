cmake_minimum_required(VERSION 3.21)

project(
	MY_PS1-game
	LANGUAGES    C CXX ASM
	VERSION      1.0.0
	DESCRIPTION  "MY_PS1 game"
	HOMEPAGE_URL "http://lameguy64.net/?page=psn00bsdk"
)

file(GLOB _sources *.c)

psn00bsdk_add_executable(my_ps1_game GPREL ${_sources})

psn00bsdk_add_cd_image(
	iso         # Target name
	my_ps1_game # Output file name (= my_ps1_game.bin + my_ps1_game.cue)
	iso.xml     # Path to config file
	DEPENDS my_ps1_game system.cnf
)
