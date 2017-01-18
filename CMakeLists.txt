cmake_minimum_required(VERSION 3.6)
project(SuperGunBros)

find_package(OpenGL)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -g -Wno-unused-parameter")


if(APPLE)
    set(GLFW_LIBRARY glfw3)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo")
else()
    set(GLFW_LIBRARY glfw)
endif()


set(LIBRARY_FILES
	lib/glad/src/glad.c
	lib/png/lodepng.cpp
)

set(SOURCE_FILES
	src/main.cpp
	src/renderlist.cpp
	src/render.cpp
	src/menuscreen.cpp
	src/readyscreen.cpp
	src/gamescreen.cpp
	src/gameoverscreen.cpp
	src/gamepad.cpp
    src/player.cpp)


add_executable(SuperGunBros ${SOURCE_FILES} ${LIBRARY_FILES})

target_link_libraries(SuperGunBros ${GLFW_LIBRARY} ${OPENGL_LIBRARIES})
include_directories(lib/glad/include)
include_directories(lib/json)
include_directories(lib/png)