//Xiyou Liao
//LevelA.cpp

#include "LevelA.h"

LevelA::LevelA() : Scene{ { 0.0f, 0.0f }, "#000000" } {}

LevelA::LevelA(Vector2 origin, const char *bgHexCode) : Scene{ origin, bgHexCode } {}

LevelA::~LevelA() { shutdown(); }

void LevelA::initialize(){
    mGameState.nextSceneID = NO_SCENE;

    //credit to Aeva on OpenGameArt.org
    mGameState.damageTakenSound = LoadSound("assets/damageTakenSound.wav");
    SetSoundVolume(mGameState.damageTakenSound, 0.8f);

    //Map
    mGameState.map = new Map(
        LEVEL_WIDTH, LEVEL_HEIGHT,
        mLevelData,
        "assets/desertTile.png",
        TILE_DIMENSION,
        4, 1,
        mOrigin
    );

    //Protagonist
    std::map<Direction, std::vector<int>> blobAnimationAtlas = {
        { RIGHT,{6, 7, 8, 9, 10, 11} },
        { LEFT, {12, 13, 14, 15, 16, 17} },
    };

    float sizeRatio = 45.0f/60.0f;

    mGameState.blob = new Entity(
        mGameState.map->getCoords(12, 2),
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

    //game objects
    Entity* key = new Entity{
        mGameState.map->getCoords(6, 21),
        {100.0f, 100.0f},
        "assets/key.png",
        KEY
    };
    Entity* door = new Entity{
        {mGameState.map->getCoords(9, 38)},
        {75.0f, 75.0f},
        "assets/door.png",
        DOOR
    };
    mGameState.entities.push_back(key);
    mGameState.entities.push_back(door);

    //NPCs
    Entity* sheep = new Entity(
        mGameState.map->getCoords(9, 10),
        { 75.0f, 75.0f },
        "assets/sheep.png",
        NPC
    );
    sheep->setAIType(JUMPER);
    sheep->setAIState(IDLE);
    sheep->setDirection(LEFT);
    sheep->setSpeed(Entity::DEFAULT_SPEED * 0.50f);
    sheep->setVelocity({0.0f, -500.0f});
    sheep->setColliderDimensions({
        sheep->getScale().x / 2.0f,
        sheep->getScale().y
    });
    sheep->setJumpingPower(600.0f);
    sheep->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
    mGameState.entities.push_back(sheep);

    Entity* rat = new Entity(
        mGameState.map->getCoords(6, 19),
        { 75.0f, 75.0f },
        "assets/rat.png",
        NPC
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

    Entity* spike = new Entity(
        mGameState.map->getCoords(13, 22),
        { 75.0f, 75.0f },
        "assets/spike.png",
        NPC
    );
    spike->setDirection(DOWN);
    mGameState.entities.push_back(spike);
}

void LevelA::update(float deltaTime){
    reachedDoor = false;
    mGameState.blob->update(
        deltaTime,nullptr, mGameState.map, mGameState.entities
    );
    if (mGameState.blob->isTakingDamage()) {
        mGameState.lives--;
        mGameState.blob->setTakingDamage(false);
        PlaySound(mGameState.damageTakenSound);
    }
    if (mGameState.blob->getPosition().y > 1200.0f) {
        mGameState.lives--;
        respawn();
    }

    for (size_t i = 0; i < mGameState.entities.size(); i++) {
        if (mGameState.entities[i] == nullptr) continue;
            mGameState.entities[i]->update(
                deltaTime, mGameState.blob, mGameState.map, mGameState.entities
            );
        if (mGameState.entities[i]->getEntityType() == KEY &&
            mGameState.entities[i]->getAcquired()) {
                isKeyAcquired = true;
        }
    }

    if (mGameState.blob->getExitReached()) {
        if (isKeyAcquired) {
            mGameState.nextSceneID = LEVEL_B;
        } else {
            mGameState.blob->setExitReached(false);
        }
    }
    if (mGameState.lives == 0) {
        mGameState.nextSceneID = LOSE;
    }
}

void LevelA::render(){
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.blob->render();
    mGameState.map->render();
    for (size_t i = 0; i<mGameState.entities.size(); i++) {
        if (mGameState.entities[i] == nullptr) continue;
        if (mGameState.entities[i]->getEntityType() == KEY &&
            mGameState.entities[i]->getAcquired()) continue;
        mGameState.entities[i]->render();
    }
}

void LevelA::shutdown() {
    delete mGameState.blob;
    mGameState.blob = nullptr;
    delete mGameState.map;
    mGameState.map = nullptr;

    UnloadSound(mGameState.damageTakenSound);
    mGameState.damageTakenSound = {};
}

void LevelA::respawn() {
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

void LevelA::restart() {
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