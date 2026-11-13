#include "LevelT.h"

LevelT::LevelT()
    : Scene{ { 0.0f, 0.0f }, "#000000" }
{}

LevelT::LevelT(Vector2 origin, const char *bgHexCode)
    : Scene{ origin, bgHexCode }
{}

LevelT::~LevelT() { shutdown(); }

void LevelT::initialize()
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

    Entity* key = new Entity{
        mGameState.map->getCoords(7, 22),
        {100.0f, 100.0f},
        "assets/key.png",
        KEY
    };
    Entity* door = new Entity{
        {mGameState.map->getCoords(6, 35)},
        {75.0f, 75.0f},
        "assets/door.png",
        DOOR
    };
    mGameState.entities.push_back(key);
    mGameState.entities.push_back(door);

    Entity* sheep = new Entity(
        mGameState.map->getCoords(9, 10), // position
        { 75.0f, 75.0f },                       // scale
        "assets/sheep.png",                  // texture file address
        NPC                                      // entity type
    );
    sheep->setDirection(LEFT);
    sheep->setAIType(JUMPER);
    sheep->setAIState(IDLE);
    sheep->setSpeed(Entity::DEFAULT_SPEED * 0.50f);
    sheep->setVelocity({0.0f, -500.0f});
    sheep->setColliderDimensions({
        sheep->getScale().x / 2.0f,
        sheep->getScale().y
    });
    sheep->setJumpingPower(800.0f);
    sheep->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
    mGameState.entities.push_back(sheep);

    Entity* rat = new Entity(
        mGameState.map->getCoords(10, 22), // position
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


}

void LevelT::update(float deltaTime)
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
        mGameState.nextSceneID = LEVEL_A;
    if (mGameState.lives == 0) {
        mGameState.nextSceneID = LOSE;
    }
}

void LevelT::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.blob->render();
    mGameState.map->render();
    for (size_t i = 0; i<mGameState.entities.size(); i++) {
        if (mGameState.entities[i]->getEntityType() == KEY && mGameState.entities[i]->getAcquired()) continue;
        mGameState.entities[i]->render();
    }
    //Tutorial Text
    if (mGameState.map->getCoords(9, 10).x - mGameState.blob->getPosition().x < 300.0f &&
        mGameState.map->getCoords(9, 10).x - mGameState.blob->getPosition().x > -100.0f) {
        DrawText("That's a sheep. Don't touch it.",
        mGameState.map->getCoords(5, 7).x, mGameState.map->getCoords(5, 7).y,
        30.0, BLACK);
    }
    if (mGameState.map->getCoords(6, 22).x - mGameState.blob->getPosition().x < 500.0f &&
        mGameState.map->getCoords(6, 22).x - mGameState.blob->getPosition().x > -100.0f) {
        DrawText("That's a bubonic plague rat.",
            mGameState.map->getCoords(8, 16).x, mGameState.map->getCoords(8, 16).y,
            30.0, BLACK);
    }
    if (mGameState.map->getCoords(8, 25).x - mGameState.blob->getPosition().x < 300.0f) {
        DrawText("Jump!",
            mGameState.map->getCoords(8, 26).x, mGameState.map->getCoords(8, 26).y,
            35.0, BLACK);
    }
    if (mGameState.map->getCoords(13, 24).y - mGameState.blob->getPosition().y < 150.0f) {
        DrawText("Try left clicking next time.",
            mGameState.map->getCoords(12, 24).x, mGameState.map->getCoords(12, 24).y,
            30.0, BLACK);
        }

}

void LevelT::shutdown()
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

void LevelT::respawn() {
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

void LevelT::restart() {
    int remainingLives = mGameState.lives;
    for (size_t i = 0; i<mGameState.entities.size(); i++) {
        delete mGameState.entities[i];
    }
    delete mGameState.blob;
    mGameState.entities.clear();
    delete mGameState.map;
    mGameState.lives = remainingLives;
}