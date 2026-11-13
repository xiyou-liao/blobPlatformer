#ifndef LOSE_H
#define LOSE_H

#include "Start.h"

class Lose : public Scene
{
public:
    Lose();
    Lose(Vector2 origin, const char *bgHexCode);
    ~Lose();

    void initialize() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
    void respawn() override;
    void restart() override;
};

#endif //LOSE_H