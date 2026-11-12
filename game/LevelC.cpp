#include "LevelC.h"

LevelC::LevelC()
    : Scene{ { 0.0f, 0.0f },  "#000000" }
{}

LevelC::LevelC(Vector2 origin, const char *bgHexCode)
    : Scene{ origin,  bgHexCode }
{}

LevelC::~LevelC() { shutdown(); }

void LevelC::initialize()
{
    mGameState.nextSceneID = NO_SCENE;

   // mGameState.bgm = LoadMusicStream("assets/game/04 - Silent Forest.wav");
    SetMusicVolume(mGameState.bgm, 0.33f);
    PlayMusicStream(mGameState.bgm);

  //  mGameState.jumpSound = LoadSound("assets/game/Dirt Jump.wav");

    /*
        ----------- MAP -----------
    */
    mGameState.map = new Map(
        LEVEL_WIDTH, LEVEL_HEIGHT, // map grid cols & rows
        mLevelData,                // grid data
        "assets/tileset.png", // texture filepath
        TILE_DIMENSION,            // tile size
        4, 1,                      // texture cols & rows
        mOrigin                    // in-game origin
    );

    /*
        ----------- PROTAGONIST -----------
    */
    std::map<Direction, std::vector<int>> blobAnimationAtlas = {
        { STILL,  {0, 1, 2, 3, 4, 5} },
        { RIGHT,  {6, 7, 8, 9, 10, 11} },
        { UP,    {12, 13, 14, 15, 16, 17} },
        { LEFT, {18, 19, 20, 21, 22, 23} },
    };

    float sizeRatio = 48.0f / 64.0f;

    // Assets from @see https://sscary.itch.io/the-adventurer-female
    mGameState.blob = new Entity(
        { mOrigin.x - 300.0f, mOrigin.y - 200.0f }, // position
        { 250.0f * sizeRatio, 250.0f },             // scale
        "assets/blob.png",                     // texture file address
        ATLAS,                                      // single image or atlas?
        { 6, 8 },                                   // atlas dimensions
        blobAnimationAtlas,                      // actual atlas
        PLAYER                                      // entity type
    );

    mGameState.blob->setJumpingPower(550.0f);
    mGameState.blob->setColliderDimensions({
        mGameState.blob->getScale().x / 3.5f,
        mGameState.blob->getScale().y / 3.0f
    });
    mGameState.blob->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
}

void LevelC::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);

    mGameState.blob->update(
        deltaTime,      // delta time / fixed timestep
        nullptr,        // player
        mGameState.map, // map
        mGameState.entities           // col. entity count
    );
}

void LevelC::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.blob->render();
    mGameState.map->render();
}

void LevelC::shutdown()
{
    delete mGameState.blob;
    mGameState.blob = nullptr;
    delete mGameState.map;
    mGameState.map = nullptr;

    UnloadMusicStream(mGameState.bgm);
    mGameState.bgm = {};
    UnloadSound(mGameState.jumpSound);
    mGameState.jumpSound = {};
}
void LevelC::reset() {
    int remainingLives = mGameState.lives;
    for (size_t i = 0; i<mGameState.entities.size(); i++) {
        delete mGameState.entities[i];
    }
    delete mGameState.blob;
    mGameState.entities.clear();
    delete mGameState.map;
    initialize();
    mGameState.lives = remainingLives;
}