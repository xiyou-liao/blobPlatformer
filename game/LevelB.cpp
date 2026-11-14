#include "LevelB.h"

LevelB::LevelB()
    : Scene{ { 0.0f, 0.0f }, "#000000" }
{}

LevelB::LevelB(Vector2 origin, const char *bgHexCode)
    : Scene{ origin, bgHexCode }
{}

LevelB::~LevelB() { shutdown(); }

void LevelB::initialize() {
    mGameState.nextSceneID = NO_SCENE;

    SetMusicVolume(mGameState.bgm, 0.33f);
    PlayMusicStream(mGameState.bgm);


    /*
        ----------- MAP -----------
    */
    mGameState.map = new Map(
        LEVEL_WIDTH, LEVEL_HEIGHT, // map grid cols & rows
        mLevelData,                // grid data
        "assets/caveTile.png", // texture filepath
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
    mGameState.map->getCoords(8, 4),
        { 100.0f * sizeRatio, 100.0f },
        "assets/blob.png",
        ATLAS,
        { 5, 6 },
        blobAnimationAtlas,
        PLAYER
    );

    mGameState.blob->setMass(6);
    mGameState.blob->setJumpingPower(600.0f);
    mGameState.blob->setColliderDimensions({
        mGameState.blob->getScale().x / 3.0f,
        mGameState.blob->getScale().y / 2.5f
    });
    mGameState.blob->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });

    Entity* key = new Entity{
        mGameState.map->getCoords(6, 29),
        {100.0f, 100.0f},
        "assets/key.png",
        KEY
    };
    Entity* door = new Entity{
            mGameState.map->getCoords(10, 38),
            {75.0f, 75.0f},
            "assets/door.png",
            DOOR
        };
    mGameState.entities.push_back(key);
    mGameState.entities.push_back(door);

    Entity* helicopter = new Entity(
        mGameState.map->getCoords(5, 40),
        {100.0f, 100.0f},
        "assets/helicopter.png",
        NPC
    );
    helicopter->setSpeed(350.0f);
    helicopter->setAIType(FLYER);
    helicopter->setDirection(LEFT);
    helicopter->setFlyBoundaries({mGameState.map->getCoords(5, 16), mGameState.map->getCoords(5, 40)});
    mGameState.entities.push_back(helicopter);

    for (int i = 0; i<4; i++) {
        Entity* sheep = new Entity(
        mGameState.map->getCoords(9, 10+i), // position
        { 75.0f, 75.0f },                       // scale
        "assets/sheep.png",                  // texture file address
        NPC                                      // entity type
    );

        sheep->setAIType(JUMPER);
        sheep->setAIState(IDLE);
        sheep->setSpeed(Entity::DEFAULT_SPEED * 0.50f);
        sheep->setVelocity({0.0f, -500.0f * i / 2.0f});
        sheep->setColliderDimensions({
            sheep->getScale().x / 2.0f,
            sheep->getScale().y
        });
        sheep->setJumpingPower(800.0f);
        sheep->setDirection(LEFT);
        sheep->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
        mGameState.entities.push_back(sheep);

    }

    Entity* rat = new Entity(
        mGameState.map->getCoords(11, 34), // position
        { 75.0f, 75.0f },                       // scale
        "assets/rat.png",                  // texture file address
        NPC                                      // entity type
    );

    rat->setAIType(LERPER);
    rat->setAIState(IDLE);
    rat->setSpeed(Entity::DEFAULT_SPEED * 0.50f);
    rat->setColliderDimensions({
        rat->getScale().x / 2.0f,
        rat->getScale().y
    });

    rat->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
    mGameState.entities.push_back(rat);

    for (int i = 0; i<5; i++) {
        Entity* spike = new Entity(
       mGameState.map->getCoords(10, 19+i),
       { 75.0f, 75.0f },
       "assets/spike.png",
       NPC
        );
        mGameState.entities.push_back(spike);
    }
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
    if (mGameState.blob->isTakingDamage()) {
        mGameState.lives--;
        mGameState.blob->setTakingDamage(false);
    }
    if (mGameState.blob->getPosition().y > 1200.0f) {
        mGameState.lives--;
        respawn();
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
        mGameState.nextSceneID = WIN;
    if (mGameState.lives == 0) {
        mGameState.nextSceneID = LOSE;
    }
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

void LevelB::respawn() {
    int remainingLives = mGameState.lives;
    mGameState.nextSceneID = NO_SCENE;
    isKeyAcquired = false;
    reachedDoor = false;
    for (size_t i = 0; i<mGameState.entities.size(); i++) {
        delete mGameState.entities[i];
    }
    delete mGameState.blob;
    mGameState.entities.clear();
    delete mGameState.map;
    initialize();
    mGameState.lives = remainingLives;
}

void LevelB::restart() {
    int remainingLives = mGameState.lives;
    mGameState.nextSceneID = NO_SCENE;
    isKeyAcquired = false;
    reachedDoor = false;
    for (size_t i = 0; i<mGameState.entities.size(); i++) {
        delete mGameState.entities[i];
    }
    delete mGameState.blob;
    mGameState.entities.clear();
    delete mGameState.map;
    mGameState.lives = remainingLives;
}