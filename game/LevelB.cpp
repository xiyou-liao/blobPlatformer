#include "LevelB.h"

LevelB::LevelB()
    : Scene{ { 0.0f, 0.0f }, "#000000" }
{}

LevelB::LevelB(Vector2 origin, const char *bgHexCode)
    : Scene{ origin, bgHexCode }
{}

LevelB::~LevelB() { shutdown(); }

void LevelB::initialize()
{
    mGameState.nextSceneID = NO_SCENE;

    //mGameState.bgm = LoadMusicStream("assets/game/04 - Silent Forest.wav");
    SetMusicVolume(mGameState.bgm, 0.33f);
    PlayMusicStream(mGameState.bgm);

   // mGameState.jumpSound = LoadSound("assets/game/Dirt Jump.wav");

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
        { LEFT,    {12, 13, 14, 15, 16, 17} },
        { THROW,    {18, 19, 20, 21, 22, 23} },
        { HURT,    {24, 25, 26, 27, 28, 29} }

    };

    float sizeRatio = 45.0f/60.0f;

    mGameState.blob = new Entity(
        {mGameState.map->getLeftBoundary(), mGameState.map->getTopBoundary()}, // position
        { 100.0f * sizeRatio, 100.0f },             // scale
        "assets/blob.png",                     // texture file address
        ATLAS,                                      // single image or atlas?
        { 5, 6 },                                   // atlas dimensions
        blobAnimationAtlas,                      // actual atlas
        PLAYER                                      // entity type
    );

    mGameState.blob->setMass(6);
    mGameState.blob->setJumpingPower(550.0f);
    mGameState.blob->setColliderDimensions({
        mGameState.blob->getScale().x / 3.0f,
        mGameState.blob->getScale().y / 2.5f
    });
    mGameState.blob->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });

    Entity* flowers = new Entity{
        mGameState.map->getCoords(5, 21),
        {100.0f, 100.0f},
        "assets/flowers.png",
        KEY
    };
    Entity* door = new Entity{
        {mGameState.map->getCoords(8, 38)},
        {500.0f, 500.0f},
        "assets/door.png",
        DOOR
    };
    mGameState.entities.push_back(flowers);
    mGameState.entities.push_back(door);
}

void LevelB::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);

    mGameState.blob->update(
        deltaTime,      // delta time / fixed timestep
        nullptr,        // player
        mGameState.map, // map
        mGameState.entities        // collidable entities
                      // col. entity count
    );

    for (size_t i = 0; i < mGameState.entities.size(); i++) {
        if (mGameState.entities[i] == nullptr) continue;
       // if (mGameState.entities[i]->getEntityType() == PLATFORM) {
            std::vector<Entity*> blank;
            mGameState.entities[i]->update(
                deltaTime,
                mGameState.blob,
                mGameState.map,
                blank);
      //  }
          if (!mGameState.entities[i]->isActive()) {
              mGameState.entities[i] = nullptr;
          }
        if (mGameState.entities[i]->getEntityType() == KEY && mGameState.entities[i]->getAcquired()) {
            isKeyAcquired = true;
        }
    }
    if (isKeyAcquired)
        mGameState.nextSceneID = LEVEL_C;
}

void LevelB::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.blob->render();
    mGameState.map->render();
    for (size_t i = 0; i<mGameState.entities.size(); i++) {
        if (mGameState.entities[i]->getEntityType() == KEY && mGameState.entities[i]->getAcquired()) continue;
        mGameState.entities[i]->render();
    }
}

void LevelB::shutdown()
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
void LevelB::reset() {
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