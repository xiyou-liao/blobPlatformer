//Xiyou Liao
//Win.cpp

#include "Win.h"

Win::Win() : Scene{ { 0.0f, 0.0f }, "#000000" } {}

Win::Win(Vector2 origin, const char *bgHexCode) : Scene{ origin, bgHexCode } {}

Win::~Win() {}

void Win::initialize() {}

void Win::update(float deltaTime){}

void Win::render(){
    ClearBackground(ColorFromHex(mBGColourHexCode));

    DrawText("Blob made it home!",
        100.0f, 200.0f,
        100, BLACK);
    DrawText("Press ENTER to return to start.\n Press Q to quit.",
         500.0f, 700.0f,
         50, BLACK);
}

void Win::shutdown() {}

void Win::respawn() {}

void Win::restart() {}