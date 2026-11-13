#include "game/LevelT.h"

// Global Constants
constexpr int SCREEN_WIDTH  = 1500,
              SCREEN_HEIGHT = 1000,
              FPS           = 120;

constexpr Vector2 ORIGIN           = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;


// Global Variables
AppStatus gAppStatus       = RUNNING;
float     gPreviousTicks   = 0.0f,
          gTimeAccumulator = 0.0f;
//int lives = 5;

//Vector2 gMousePosition = GetMousePosition();
bool placePlatform = false;
bool absorbPlatform = false;


Camera2D gCamera = {};

Scene *gCurrentScene = nullptr;
std::map<SceneID, Scene*> gLevels = {};

// Function Declarations
void switchToScene(Scene* scene);
void initialise();
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
    gCurrentScene->initialize();
    gCamera.target = gCurrentScene->getState().blob->getPosition();
}

void initialize() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Blob Platformer");

    gLevels[LEVEL_A] = new LevelA(ORIGIN, "#3A5A40");
    gLevels[LEVEL_B] = new LevelB(ORIGIN, "#DAD7CD");
    gLevels[LEVEL_T] = new LevelT(ORIGIN, "DAD7CD");

    switchToScene(gLevels[LEVEL_T]);

    gCamera.offset   = ORIGIN;
    gCamera.rotation = 0.0f;
    gCamera.zoom     = 1.0f;

    SetTargetFPS(FPS);
}

void processInput()
{
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

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
    if (IsKeyPressed(KEY_ONE)) switchToScene(gLevels[LEVEL_T]);
    if (IsKeyPressed(KEY_TWO)) switchToScene(gLevels[LEVEL_A]);
    if (IsKeyPressed(KEY_THREE)) switchToScene(gLevels[LEVEL_B]);

}

void update()
{
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
    BeginMode2D(gCamera);

    gCurrentScene->render();

    EndMode2D();
    EndDrawing();
}

void shutdown()
{
    for (std::pair<const SceneID, Scene *> &entry : gLevels)
        delete entry.second;

    gLevels.clear();

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
       //     int currentLives = gCurrentScene->getState().lives;
            SceneID id = gCurrentScene->getState().nextSceneID;
            switchToScene(gLevels[id]);
       //     gCurrentScene->getState().lives = currentLives;
        }

        render();
    }

    shutdown();

    return 0;
}