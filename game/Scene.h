#ifndef SCENE_H
#define SCENE_H

#include "Entity.h"

struct GameState
{
    Entity *blob = nullptr;
    Map    *map = nullptr;
    std::vector<Entity*> entities;

    int lives = 5;

    Music bgm;
    Sound jumpSound;

    SceneID nextSceneID = NO_SCENE;
};

class Scene
{
protected:
    GameState   mGameState;
    Vector2     mOrigin;
    const char *mBGColourHexCode = "#000000";
    bool isKeyAcquired = false;
    bool reachedDoor = false;
 //   Vector2 mSpawnPoint;
public:
    Scene();
    Scene(Vector2 origin, const char *bgHexCode);
    virtual ~Scene() {}

    virtual void initialize() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;
    virtual void respawn() = 0;
    virtual void restart() = 0;

    GameState&   getState()          { return mGameState;        }
    Vector2     getOrigin()          const { return mOrigin;           }

    bool getIsKeyAcquired() const {return isKeyAcquired;}
    const char *getBGColourHexCode() const { return mBGColourHexCode; }

};

#endif //SCENE_H