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


        src/gameobjects/weapons/pistol.cpp
        src/gameobjects/weapons/pistolbullet.cpp

        src/gameobjects/weapons/rocket.cpp
        src/gameobjects/weapons/rocketbullet.cpp

        src/gameobjects/weapons/pierce.cpp
        src/gameobjects/weapons/piercebullet.cpp

        src/gameobjects/weapons/bounce.cpp
        src/gameobjects/weapons/bouncebullet.cpp

        src/gameobjects/weapons/flame.cpp
        src/gameobjects/weapons/flamebullet.cpp
    )

set(SOURCE_FILES
    ${WEAPON_SOURCES}
	src/main.cpp
		src/rendering/renderlist.cpp
		src/rendering/render.cpp
		src/screens/menuscreen.cpp
		src/screens/readyscreen.cpp
		src/screens/gamescreen.cpp
	"src/${OS_DIR}/gamepad.cpp"
		src/gameobjects/player.cpp
	src/physicsengine.cpp
		src/gameobjects/player.cpp
    src/rectangle.cpp
		src/gameobjects/level/level.cpp
		src/gameobjects/weapons/weaponbox.cpp
		src/gameobjects/explosion.cpp
    src/point.cpp
		src/rendering/camera.cpp
	src/magic.h
		src/gameobjects/level/boxspawn.cpp
		src/soundthread.cpp
		src/gameobjects/gameobject.h)

add_executable(SuperGunBros ${SOURCE_FILES} ${LIBRARY_FILES})

target_link_libraries(SuperGunBros ${GLFW_LIBRARY} ${OPENGL_LIBRARIES})
include_directories(lib/glad/include)
include_directories(lib/rapidjson)
include_directories(lib/png)
include_directories(src)