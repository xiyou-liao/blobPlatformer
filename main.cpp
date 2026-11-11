#include "game/LevelC.h"

// Global Constants
constexpr int SCREEN_WIDTH  = 1500,
              SCREEN_HEIGHT = 800,
              FPS           = 120;

constexpr Vector2 ORIGIN           = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;


// Global Variables
AppStatus gAppStatus       = RUNNING;
float     gPreviousTicks   = 0.0f,
          gTimeAccumulator = 0.0f;

Vector2 gMousePosition = GetMousePosition();
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
    if (gCurrentScene) gCurrentScene->shutdown();
    gCurrentScene = scene;
    gCurrentScene->initialise();
    gCamera.target = gCurrentScene->getState().blob->getPosition();
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Blob Platformer");

    gLevels[LEVEL_A] = new LevelA(ORIGIN, "#DAD7CD");
    gLevels[LEVEL_B] = new LevelB(ORIGIN, "#DAD7CD");
    gLevels[LEVEL_C] = new LevelC(ORIGIN, "DAD7CD");

    switchToScene(gLevels[LEVEL_A]);

    gCamera.offset   = ORIGIN;
    gCamera.rotation = 0.0f;
    gCamera.zoom     = 1.0f;

    SetTargetFPS(FPS);
}

void processInput()
{
    gCurrentScene->getState().blob->resetMovement();

    if (!placePlatform) {
        if      (IsKeyDown(KEY_A)) gCurrentScene->getState().blob->moveLeft();
        else if (IsKeyDown(KEY_D)) gCurrentScene->getState().blob->moveRight();

        if (IsKeyPressed(KEY_W) && gCurrentScene->getState().blob->isCollidingBottom())
        {
            gCurrentScene->getState().blob->jump();
        }
        if (!IsKeyDown(KEY_A) && !IsKeyDown(KEY_D) && !IsKeyDown(KEY_W)) gCurrentScene->getState().blob->standStill();
        // to avoid faster diagonal speed
    }
    if (IsKeyPressed(KEY_T)) {
     //   std::cout<<"keyT is pressed!" << std::endl;
        if (!placePlatform) {
            placePlatform = true;

        }
        else placePlatform = false;
    }
    if (IsKeyPressed(KEY_F)) {
        if (!absorbPlatform) absorbPlatform = true;
        else absorbPlatform = false;
    }

    if (placePlatform) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            gCurrentScene->getState().blob->throwPlatform();
            gMousePosition = GetMousePosition();
            Vector2 worldMousePosition = {
                gMousePosition.x - gCamera.offset.x + gCamera.target.x,
                gMousePosition.y - gCamera.offset.y + gCamera.target.y
            };
            if (gCurrentScene->getState().blob->getLives() > 1) {
             //   std::cout <<"inside mouse click to place platform" << std:: endl;
                Entity* newPlatform = new Entity{
                    worldMousePosition,
                    {50.0f, 50.0f},
                    "assets/blobPlatform.png",
                    PLATFORM
                };
              //  std::cout << "platform position: " << newPlatform->getPosition().x << " " << newPlatform->getPosition().y << " mouse position: " <<
              //      gMousePosition.x << " " << gMousePosition.y << std::endl;
               gCurrentScene->getState().entities.push_back(newPlatform);

                //std::cout<<"platform is placed! " << gCurrentScene->getState().entities.size()<<std::endl;
                gCurrentScene->getState().blob->decreaseLives();
                placePlatform = false;
            }
        }
    }
    // if (absorbPlatform) {
    //
    // }


    if (GetLength(gCurrentScene->getState().blob->getMovement()) > 1.0f)
        gCurrentScene->getState().blob->normaliseMovement();

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
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
    for (size_t i = 0; i<gCurrentScene->getState().entities.size(); i++) {
        if (gCurrentScene->getState().entities[i]->getEntityType() == TARGET &&
            gCurrentScene->getState().entities[i]->getAcquired()) {
            gCurrentScene->getState().entities[i]->setPosition({SCREEN_WIDTH - 150.0f, 50.0f});
            gCurrentScene->getState().entities[i]->render();
        }
    }
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
    initialise();

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