cmake_minimum_required(VERSION 4.0)
project(blobPlatformer)

set(CMAKE_CXX_STANDARD 23)

add_executable(blobPlatformer main.cpp)

target_include_directories(blobPlatformer PRIVATE
        C:/raylib/include
)

target_link_directories(blobPlatformer PRIVATE
        C:/raylib/lib
)

target_link_libraries(blobPlatformer PRIVATE
        raylib
        opengl32
        gdi32
        winmm
)