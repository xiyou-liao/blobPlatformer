#include "Start.h"

Start::Start()
    : Scene{ { 0.0f, 0.0f }, "#000000" }
{}

Start::Start(Vector2 origin, const char *bgHexCode)
    : Scene{ origin, bgHexCode }
{}

Start::~Start() {}

void Start::initialize(){}
void Start::update(float deltaTime) {}
void Start::render(){
    ClearBackground(ColorFromHex(mBGColourHexCode));

    DrawText("Press ENTER to start.",
            mOrigin.x / 2.0f, mOrigin.y / 2.0f,
            50.0, BLACK);
}
void Start::shutdown() {}

void Start::respawn() {}

void Start::restart() {}