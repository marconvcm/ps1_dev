cmake_minimum_required(VERSION 3.21)

project(
	hello_pong
	LANGUAGES    C CXX ASM
	VERSION      1.0.0
	DESCRIPTION  "hello_pong"
	HOMEPAGE_URL "http://lameguy64.net/?page=psn00bsdk"
)

file(GLOB _sources *.c)
file(GLOB _lib_sources libs/*.c)

psn00bsdk_add_executable(hello_pong GPREL ${_sources} ${_lib_sources})

#region images
psn00bsdk_target_incbin(hello_pong PRIVATE tim_ball16c assets/ball16c.tim)
psn00bsdk_target_incbin(hello_pong PRIVATE tim_game_title assets/game_title.tim)
psn00bsdk_target_incbin(hello_pong PRIVATE tim_main_texture assets/main_texture.tim)
#endregion

psn00bsdk_add_cd_image(
	iso         # Target name
	hello_pong # Output file name (= hello_pong.bin + hello_pong.cue)
	iso.xml     # Path to config file
	DEPENDS hello_pong system.cnf
)
