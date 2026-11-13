#include "Win.h"

Win::Win()
    : Scene{ { 0.0f, 0.0f }, "#000000" }
{}

Win::Win(Vector2 origin, const char *bgHexCode)
    : Scene{ origin, bgHexCode }
{}
void Win::initialize(){}
void Win::update(float deltaTime){}
void Win::render(){
    ClearBackground(ColorFromHex(mBGColourHexCode));

    DrawText("Press ENTER to return to start.",
            mOrigin.x / 2.0f, mOrigin.y / 2.0f,
            50.0, BLACK);
}
void Win::shutdown() {}
void Win::respawn() {}
void Win::restart() {}