#include "Lose.h"

Lose::Lose() : Scene{ { 0.0f, 0.0f }, "#000000" } {}

Lose::Lose(Vector2 origin, const char *bgHexCode) : Scene{ origin, bgHexCode } {}

Lose::~Lose() {}

void Lose::initialize() {}

void Lose::update(float deltaTime){}

void Lose::render(){
    ClearBackground(ColorFromHex(mBGColourHexCode));

    DrawText("Blob never made it home...",
         50.0f, 200.0f,
         100, RED);
    DrawText("Press ENTER to return to start.\n Press Q to quit.",
         500.0f, 700.0f,
         50, WHITE);
}

void Lose::shutdown() {}

void Lose::respawn() {}

void Lose::restart() {}