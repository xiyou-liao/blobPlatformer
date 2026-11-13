#ifndef WIN_H
#define WIN_H

#include "Lose.h"

class Win : public Scene
{
public:
    Win();
    Win(Vector2 origin, const char *bgHexCode);
    ~Win();

    void initialize() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
    void respawn() override;
    void restart() override;
};

#endif //WIN_H