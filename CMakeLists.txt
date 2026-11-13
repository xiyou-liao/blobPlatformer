cmake_minimum_required(VERSION 4.0)
project(blobPlatformer)

set(CMAKE_CXX_STANDARD 23)

add_executable(blobPlatformer main.cpp
        game/Entity.h
        game/Entity.cpp
        game/Map.h
        game/Map.cpp
        game/game.h
        game/game.cpp
        game/Scene.h
        game/Scene.cpp
        game/LevelA.h
        game/LevelA.cpp
        game/LevelB.h
        game/LevelB.cpp
        game/LevelT.h
        game/LevelT.cpp
        game/Start.h
        game/Start.cpp
        game/Lose.h
        game/Lose.cpp
        game/Win.h
        game/Win.cpp)

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