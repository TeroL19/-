cmake_minimum_required(VERSION 3.20)
project(ArkanoidProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake")

set(SDL2_DIR "C:/vcpkg/installed/x64-windows/share/sdl2")
set(SDL2_ttf_DIR "C:/vcpkg/installed/x64-windows/share/sdl2_ttf")

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)

include_directories(include)
include_directories(${SDL2_INCLUDE_DIRS})

file(GLOB_RECURSE SRC_FILES
    "src/*.cpp"
    "Arkanoid.cpp"
)

add_executable(Arkanoid ${SRC_FILES})
target_link_libraries(Arkanoid PRIVATE SDL2::SDL2 SDL2_ttf::SDL2_ttf)

if (WIN32)
    add_custom_command(TARGET Arkanoid POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "C:/vcpkg/installed/x64-windows/bin/SDL2.dll"
            "C:/vcpkg/installed/x64-windows/bin/SDL2_ttf.dll"
            $<TARGET_FILE_DIR:Arkanoid>)
endif()

