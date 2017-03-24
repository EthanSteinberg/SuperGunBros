cmake_minimum_required(VERSION 3.6)
project(SuperGunBros)

find_package(OpenGL)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -g -Wno-unused-parameter")


if(APPLE)
    set(GLFW_LIBRARY glfw3 portaudio sndfile pthread)
	set(OS_DIR osx)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo")
else()
	set(OS_DIR linux)
    set(GLFW_LIBRARY glfw portaudio sndfile pthread)
endif()


set(LIBRARY_FILES
	lib/glad/src/glad.c
	lib/png/lodepng.cpp
)

set(WEAPON_SOURCES
        src/gameobjects/weapons/gun.cpp
        src/gameobjects/weapons/bullet.cpp


		src/gameobjects/weapons/pistol/pistol.cpp
		src/gameobjects/weapons/pistol/pistolbullet.cpp

		src/gameobjects/weapons/rocket/rocket.cpp
		src/gameobjects/weapons/rocket/rocketbullet.cpp

		src/gameobjects/weapons/piercer/pierce.cpp
		src/gameobjects/weapons/piercer/piercebullet.cpp

		src/gameobjects/weapons/ricochet/bounce.cpp
		src/gameobjects/weapons/ricochet/bouncebullet.cpp

		src/gameobjects/weapons/flame/flame.cpp
		src/gameobjects/weapons/flame/flamebullet.cpp
    )

set(SOURCE_FILES
		src/main.cpp

		src/rendering/render.cpp
		src/rendering/renderlist.cpp
		src/rendering/camera.cpp

		src/screens/menuscreen.cpp
		src/screens/readyscreen.cpp
		src/screens/gamescreen.cpp

		"src/${OS_DIR}/gamepad.cpp"

		#src/physicsengine.cpp

		src/soundthread.cpp

		src/gameobjects/player/player.cpp
		src/gameobjects/player/statuseffects/burningeffect.cpp

		src/gameobjects/level/boxspawn.cpp
		src/gameobjects/level/effectfield.cpp
		src/gameobjects/level/level.cpp

		src/rectangle.cpp
		src/point.cpp

		${WEAPON_SOURCES}
		src/gameobjects/weapons/weaponbox.cpp
		src/gameobjects/weapons/explosion.cpp

	)

add_executable(SuperGunBros ${SOURCE_FILES} ${LIBRARY_FILES})

target_link_libraries(SuperGunBros ${GLFW_LIBRARY} ${OPENGL_LIBRARIES})
include_directories(lib/glad/include)
include_directories(lib/rapidjson)
include_directories(lib/png)
include_directories(src)