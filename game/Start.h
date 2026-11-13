#ifndef START_H
#define START_H

#include "Scene.h"

class Start : public Scene
{
public:
    Start();
    Start(Vector2 origin, const char *bgHexCode);
    ~Start();

    void initialize() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
    void respawn() override;
    void restart() override;
};

#endif //START_H