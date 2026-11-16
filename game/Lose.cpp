#include "Lose.h"

Lose::Lose()
    : Scene{ { 0.0f, 0.0f }, "#000000" }
{}

Lose::Lose(Vector2 origin, const char *bgHexCode)
    : Scene{ origin, bgHexCode }
{}
Lose::~Lose(){}
void Lose::initialize() {
   // mBackgroundTexture = LoadTexture("assets/loseScreen.png");
}
void Lose::update(float deltaTime){}
void Lose::render(){
    ClearBackground(ColorFromHex(mBGColourHexCode));

    // Vector2 objectOrigin = {0.0f, 0.0f};
    //
    // Rectangle textureAreaBackground = {
    //     0.0f, 0.0f,
    //     static_cast<float>(mBackgroundTexture.width),
    //     static_cast<float>(mBackgroundTexture.height),
    // };
    // Rectangle destinationAreaBackground = {
    //     0.0f, 0.0f,
    //     1500.0f, 1000.0f
    // };
    //
    // DrawTexturePro(mBackgroundTexture, textureAreaBackground,
    //     destinationAreaBackground, objectOrigin, 0.0f , WHITE);


    DrawText("Blob never made it home...",
         50.0f, 200.0f,
         100, RED);
    DrawText("Press ENTER to return to start.\n Press Q to quit.",
         500.0f, 700.0f,
         50, WHITE);

}
void Lose::shutdown() {
 //   UnloadTexture(mBackgroundTexture);
}
void Lose::respawn() {}
void Lose::restart() {}