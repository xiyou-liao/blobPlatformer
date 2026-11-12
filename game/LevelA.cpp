#include "LevelA.h"

LevelA::LevelA()
    : Scene{ { 0.0f, 0.0f }, "#000000" }
{}

LevelA::LevelA(Vector2 origin, const char *bgHexCode)
    : Scene{ origin, bgHexCode }
{}

LevelA::~LevelA() { shutdown(); }

void LevelA::initialize()
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

    Vector2 spawn = {mGameState.map->getLeftBoundary(), mGameState.map->getTopBoundary()};
  //  mSpawnPoint = spawn;
    mGameState.blob = new Entity(
        spawn, // position
        { 100.0f * sizeRatio, 100.0f },             // scale
        "assets/blob.png",                     // texture file address
        ATLAS,                                      // single image or atlas?
        { 5, 6 },                                   // atlas dimensions
        blobAnimationAtlas,                      // actual atlas
        PLAYER                                      // entity type
    );

    mGameState.blob->setMass(6);
    mGameState.blob->setJumpingPower(600.0f);
    mGameState.blob->setColliderDimensions({
        mGameState.blob->getScale().x / 3.0f,
        mGameState.blob->getScale().y / 2.5f
    });
    mGameState.blob->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });

    Entity* flowers = new Entity{
        mGameState.map->getCoords(6, 21),
        {100.0f, 100.0f},
        "assets/flowers.png",
        KEY
    };
    Entity* door = new Entity{
        {mGameState.map->getCoords(9, 38)},
        {75.0f, 75.0f},
        "assets/door.png",
        DOOR
    };
    mGameState.entities.push_back(flowers);
    mGameState.entities.push_back(door);

    std::map<Direction, std::vector<int>> humanoidAnimationAtlas = {
        { RIGHT,  { 0, 1, 2, 3, 4, 5, 6, 7 } },
        { LEFT, { 8, 9, 10, 11, 12, 13, 14, 15 } },
    };

    // @see dyru.itch.io/pixel-ghost-template
    Entity* humanoid = new Entity(
        mGameState.map->getCoords(9, 10), // position
        { 75.0f, 75.0f },                       // scale
        "assets/humanoid.png",                  // texture file address
        ATLAS,                                    // single image or atlas?
        {2, 8},                         // atlas dimensions
        humanoidAnimationAtlas,                      // actual atlas
        NPC                                      // entity type
    );

    humanoid->setAIType(JUMPER);
    humanoid->setAIState(IDLE);
    humanoid->setSpeed(Entity::DEFAULT_SPEED * 0.50f);
    humanoid->setVelocity({0.0f, -500.0f});
    humanoid->setColliderDimensions({
        humanoid->getScale().x / 2.0f,
        humanoid->getScale().y
    });
    humanoid->setJumpingPower(600.0f);
    humanoid->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
    humanoid->setDirection(LEFT);
    mGameState.entities.push_back(humanoid);
}

void LevelA::update(float deltaTime)
{
    std::cout << "number of lives left: " << mGameState.lives << std::endl;
    UpdateMusicStream(mGameState.bgm);

    mGameState.blob->update(
        deltaTime,      // delta time / fixed timestep
        nullptr,        // player
        mGameState.map, // map
        mGameState.entities        // collidable entities
                      // col. entity count
    );
    if (mGameState.blob->isTakingDamage()) {
        mGameState.lives--;
        mGameState.blob->setTakingDamage(false);
    }
    if (mGameState.blob->getPosition().y > 1200.0f) {
        mGameState.lives--;
        reset();
    }
    for (size_t i = 0; i < mGameState.entities.size(); i++) {
        if (mGameState.entities[i] == nullptr) continue;
       // if (mGameState.entities[i]->getEntityType() == PLATFORM) {
          //  std::vector<Entity*> blank;
            mGameState.entities[i]->update(
                deltaTime,
                mGameState.blob,
                mGameState.map,
                mGameState.entities);
      //  }
          if (!mGameState.entities[i]->isActive()) {
              mGameState.entities[i] = nullptr;
          }
        if (mGameState.entities[i]->getEntityType() == KEY && mGameState.entities[i]->getAcquired()) {
            isKeyAcquired = true;
        }
    }
    if (mGameState.blob->getExitReached()) reachedDoor = true;
    if (isKeyAcquired && reachedDoor)
        mGameState.nextSceneID = LEVEL_B;
    if (mGameState.lives == 0) std::cout<<"You lose!"<<std::endl;
}

void LevelA::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.blob->render();
    mGameState.map->render();
    for (size_t i = 0; i<mGameState.entities.size(); i++) {
        if (mGameState.entities[i]->getEntityType() == KEY && mGameState.entities[i]->getAcquired()) continue;
        mGameState.entities[i]->render();
    }
}

void LevelA::shutdown()
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

void LevelA::reset() {
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