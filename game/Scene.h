//
// Created by xiyou on 11/10/2026.
//

#ifndef SCENE_H
#define SCENE_H

#include "Entity.h"

struct GameState
{
    Entity *blob;
    Map    *map;
    std::vector<Entity*> entities;

    Music bgm;
    Sound jumpSound;

    SceneID nextSceneID;
};

class Scene
{
protected:
    GameState   mGameState;
    Vector2     mOrigin;
    const char *mBGColourHexCode = "#000000";

public:
    Scene();
    Scene(Vector2 origin, const char *bgHexCode);
    virtual ~Scene() {}

    virtual void initialise() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;

    GameState&   getState()          { return mGameState;        }
    Vector2     getOrigin()          const { return mOrigin;           }
    const char *getBGColourHexCode() const { return mBGColourHexCode; }

};

#endif //SCENE_H