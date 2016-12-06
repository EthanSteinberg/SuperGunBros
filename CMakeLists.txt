cmake_minimum_required(VERSION 3.6)
project(SuperGunBros)


set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


if(APPLE)
    set(GLFW_LIBRARY glfw3)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo")
else()
    set(GLFW_LIBRARY glfw)
endif()



set(SOURCE_FILES main.cpp)
add_executable(SuperGunBros ${SOURCE_FILES})

target_link_libraries(SuperGunBros ${GLFW_LIBRARY})
