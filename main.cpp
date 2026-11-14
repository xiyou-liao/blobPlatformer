#include "game/LevelT.h"
#include "game/Win.h"

// Global Constants
constexpr int SCREEN_WIDTH  = 1500,
              SCREEN_HEIGHT = 1000,
              FPS           = 120;

constexpr Vector2 ORIGIN           = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

Texture2D healthyLifeTexture;
Texture2D lostLifeTexture;

// Global Variables
AppStatus gAppStatus       = RUNNING;
float     gPreviousTicks   = 0.0f,
          gTimeAccumulator = 0.0f;

bool placePlatform = false;

Camera2D gCamera = {};

Scene *gCurrentScene = nullptr;
std::map<SceneID, Scene*> gLevels = {};

// Function Declarations
void switchToScene(Scene* scene);
void initialize();
void processInput();
void update();
void render();
void shutdown();
void panCamera(Camera2D *camera, const Vector2 *targetPosition);

void switchToScene(Scene *scene)
{
    int currentLives = 5;
    if (gCurrentScene) {
        currentLives = gCurrentScene->getState().lives;
        gCurrentScene->shutdown();
    }
    gCurrentScene = scene;
    gCurrentScene->getState().lives = currentLives;
    gCurrentScene->getState().nextSceneID = NO_SCENE;
    gCurrentScene->restart();
    gCurrentScene->initialize();
    if (gCurrentScene->getState().blob != nullptr) {
        gCamera.target = gCurrentScene->getState().blob->getPosition();
    }

}

void initialize() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Blob Platformer");

    healthyLifeTexture = LoadTexture("assets/healthyLife.png");
    lostLifeTexture = LoadTexture("assets/lostLife.png");


    gLevels[LEVEL_A] = new LevelA(ORIGIN, "#C3D1E5");
    gLevels[LEVEL_B] = new LevelB(ORIGIN, "#2A2C28");
    gLevels[LEVEL_T] = new LevelT(ORIGIN, "#E1F6FF");
    gLevels[START] = new Start(ORIGIN, "#FFFFFF");
    gLevels[LOSE] = new Lose(ORIGIN, "#FFFFFF");
    gLevels[WIN] = new Win(ORIGIN, "#FFFFFF");

    switchToScene(gLevels[START]);

    gCamera.offset   = ORIGIN;
    gCamera.rotation = 0.0f;
    gCamera.zoom     = 1.0f;

    SetTargetFPS(FPS);
}

void processInput()
{
    if (gCurrentScene == gLevels[START]) {
        if (IsKeyPressed(KEY_ENTER)) {
            switchToScene(gLevels[LEVEL_T]);
        }
    }else if (gCurrentScene == gLevels[LOSE] || gCurrentScene == gLevels[WIN]){
        if (IsKeyPressed(KEY_ENTER)) {
            for (auto levels: gLevels) {
                if (levels.second != gLevels[START] && levels.second != gLevels[LOSE] && levels.second != gLevels[WIN]) {
                    levels.second->restart();
                }
            }
            switchToScene(gLevels[START]);
            gCurrentScene->getState().lives = 5;
        }
    }else {
        gCurrentScene->getState().blob->resetMovement();

        if      (IsKeyDown(KEY_A)) gCurrentScene->getState().blob->moveLeft();
        else if (IsKeyDown(KEY_D)) gCurrentScene->getState().blob->moveRight();

        if (IsKeyPressed(KEY_W) && gCurrentScene->getState().blob->isCollidingBottom())
        {
            gCurrentScene->getState().blob->jump();
        }
        if (!IsKeyDown(KEY_A) && !IsKeyDown(KEY_D) && !IsKeyDown(KEY_W)) gCurrentScene->getState().blob->standStill();
        // to avoid faster diagonal speed
        if (GetLength(gCurrentScene->getState().blob->getMovement()) > 1.0f) {
            gCurrentScene->getState().blob->normalizeMovement();
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            placePlatform = true;
        }
        if (placePlatform && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePosition = GetMousePosition();
            Vector2 blobPosition = gCurrentScene->getState().blob->getPosition();
            Vector2 worldMousePosition = {
                mousePosition.x - gCamera.offset.x + gCamera.target.x,
                mousePosition.y - gCamera.offset.y + gCamera.target.y
            };
            Vector2 direction = {
                worldMousePosition.x - blobPosition.x,
                worldMousePosition.y - blobPosition.y
            };
            //  if (GetLength(direction) > 0.0f)
            Normalize(&direction);
            if (gCurrentScene->getState().blob->getMass() > 1) {
                Entity* newPlatform = new Entity{
                    {blobPosition.x, blobPosition.y - gCurrentScene->getState().blob->getColliderDimensions().y },
                    {50.0f, 50.0f},
                    "assets/platform.png",
                    PLATFORM
                };
                float platformSpeed = 500.0f;
                newPlatform->setSpeed(platformSpeed);
                newPlatform->setAcceleration({0.0f, 981.0f});
                newPlatform->setFlying();
                newPlatform->setVelocity({direction.x * platformSpeed, direction.y * platformSpeed});
                gCurrentScene->getState().entities.push_back(newPlatform);

                gCurrentScene->getState().blob->decreaseMass();
                placePlatform = false;
            }
        }

        if (GetLength(gCurrentScene->getState().blob->getMovement()) > 1.0f)
            gCurrentScene->getState().blob->normalizeMovement();
    }
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
    if (IsKeyPressed(KEY_ONE)) switchToScene(gLevels[LEVEL_T]);
    if (IsKeyPressed(KEY_TWO)) switchToScene(gLevels[LEVEL_A]);
    if (IsKeyPressed(KEY_THREE)) switchToScene(gLevels[LEVEL_B]);

}

void update()
{
    if (gCurrentScene == gLevels[START] || gCurrentScene == gLevels[LOSE] || gCurrentScene == gLevels[WIN]) {
        gTimeAccumulator = 0.0f;
        return;
    }
    // Delta time
    float ticks     = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    // Fixed timestep
    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        gCurrentScene->update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;

        Vector2 currentPlayerPosition = {
            gCurrentScene->getState().blob->getPosition().x, ORIGIN.y
        };

        panCamera(&gCamera, &currentPlayerPosition);
    }

    gTimeAccumulator = deltaTime;

}

void render()
{
    BeginDrawing();
    if (gCurrentScene == gLevels[START] || gCurrentScene == gLevels[LOSE] || gCurrentScene == gLevels[WIN]) {
        gCurrentScene->render();
    }else {
        BeginMode2D(gCamera);
        gCurrentScene->render();
        EndMode2D();
    }
    if (gCurrentScene != gLevels[START] && gCurrentScene != gLevels[LOSE] && gCurrentScene != gLevels[WIN]) {
        for (int i = 0; i<5; i++) {
            Vector2 healthBarCoords = {50.0f + 75.0f * i, 50.0f};
            Rectangle textureAreaHealthyLife = {
                0.0f, 0.0f,
                static_cast<float>(healthyLifeTexture.width), static_cast<float>(healthyLifeTexture.height)
            };

            Rectangle destinationAreaHealthyLife = {
                healthBarCoords.x , healthBarCoords.y,
                50.0f, 50.0f
            };
            Rectangle textureAreaLostLife = {
                0.0f, 0.0f,
                static_cast<float>(lostLifeTexture.width), static_cast<float>(lostLifeTexture.height)
            };

            Rectangle destinationAreaLostLife = {
                healthBarCoords.x , healthBarCoords.y,
                50.0f, 50.0f
            };
            Vector2 objectOrigin = {75.0f / 2.0f, 75.0f / 2.0f};
            if (i<gCurrentScene->getState().lives) {
                DrawTexturePro(healthyLifeTexture, textureAreaHealthyLife,
               destinationAreaHealthyLife, objectOrigin, 0.0f, WHITE);
            }else {
                DrawTexturePro(lostLifeTexture, textureAreaLostLife,
               destinationAreaLostLife, objectOrigin, 0.0f, WHITE);
            }

    }


    }
    EndDrawing();
}

void shutdown()
{
    for (std::pair<const SceneID, Scene *> &entry : gLevels)
        delete entry.second;

    gLevels.clear();
    UnloadTexture(healthyLifeTexture);
    UnloadTexture(lostLifeTexture);

    CloseAudioDevice();
    CloseWindow();
}

int main(void)
{
    initialize();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();

        if (gCurrentScene->getState().nextSceneID != NO_SCENE)
        {
            SceneID id = gCurrentScene->getState().nextSceneID;
            switchToScene(gLevels[id]);
        }

        render();
    }

    shutdown();

    return 0;
}