#include "Lose.h"

Lose::Lose()
    : Scene{ { 0.0f, 0.0f }, "#000000" }
{}

Lose::Lose(Vector2 origin, const char *bgHexCode)
    : Scene{ origin, bgHexCode }
{}
void Lose::initialize(){}
void Lose::update(float deltaTime){}
void Lose::render(){
    ClearBackground(ColorFromHex(mBGColourHexCode));

    DrawText("Press ENTER to return to start.",
            mOrigin.x / 2.0f, mOrigin.y / 2.0f,
            50.0, BLACK);
}
void Lose::shutdown() {}
void Lose::respawn() {}
void Lose::restart() {}