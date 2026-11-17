//Xiyou Liao
//Win.h

#ifndef WIN_H
#define WIN_H

#include "Scene.h"

class Win : public Scene {
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
private:
    Texture2D mBackgroundTexture;
};

#endif //WIN_H