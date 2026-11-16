#include "Start.h"

Start::Start()
    : Scene{ { 0.0f, 0.0f }, "#000000" }
{}

Start::Start(Vector2 origin, const char *bgHexCode)
    : Scene{ origin, bgHexCode }
{}

Start::~Start() {}

void Start::initialize() {
    mBackgroundTexture = LoadTexture("assets/startScreen.png");
}
void Start::update(float deltaTime) {}
void Start::render(){
    ClearBackground(ColorFromHex(mBGColourHexCode));
    Vector2 objectOrigin = {0.0f, 0.0f};

    Rectangle textureAreaBackground = {
        0.0f, 0.0f,
        static_cast<float>(mBackgroundTexture.width),
        static_cast<float>(mBackgroundTexture.height),
    };
    Rectangle destinationAreaBackground = {
        0.0f, 0.0f,
        1500.0f, 1000.0f
    };

    DrawTexturePro(mBackgroundTexture, textureAreaBackground,
        destinationAreaBackground, objectOrigin, 0.0f , WHITE);
    DrawText("Press ENTER to start.\n Press Q to quit.",
         800.0f, 700.0f,
         50, BLACK);
}
void Start::shutdown() {
    UnloadTexture(mBackgroundTexture);
}

void Start::respawn() {}

void Start::restart() {}