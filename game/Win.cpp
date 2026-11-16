#include "Win.h"

Win::Win()
    : Scene{ { 0.0f, 0.0f }, "#000000" }
{}

Win::Win(Vector2 origin, const char *bgHexCode)
    : Scene{ origin, bgHexCode }
{}
Win::~Win(){}
void Win::initialize() {
    //mBackgroundTexture = LoadTexture("assets/winScreen.png");
}
void Win::update(float deltaTime){}
void Win::render(){
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
    DrawText("Blob made it home!",
        50.0f, 200.0f,
        100, BLACK);
    DrawText("Press ENTER to return to start.\n Press Q to quit.",
         500.0f, 700.0f,
         50, BLACK);

}
void Win::shutdown() {
  //  UnloadTexture(mBackgroundTexture);
}
void Win::respawn() {}
void Win::restart() {}