cmake_minimum_required(VERSION 3.20)
project(GemsProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake")

set(SDL2_DIR "C:/vcpkg/installed/x64-windows/share/sdl2")

find_package(SDL2 REQUIRED)

include_directories(include)
include_directories(${SDL2_INCLUDE_DIRS})

file(GLOB_RECURSE SRC_FILES
    "src/*.cpp"
    "Gems.cpp"
)

add_executable(Gems ${SRC_FILES})
target_link_libraries(Gems PRIVATE SDL2::SDL2)

if (WIN32)
    add_custom_command(TARGET Gems POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "C:/vcpkg/installed/x64-windows/bin/SDL2.dll"
            $<TARGET_FILE_DIR:Gems>)
endif()
