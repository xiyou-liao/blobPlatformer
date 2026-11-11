#include "game/Entity.h"

struct GameState
{
    Entity *blob;
    Map *map;

    Music bgm;
    Sound jumpSound;

    Camera2D camera;
};

// Global Constants
constexpr int SCREEN_WIDTH  = 1500,
              SCREEN_HEIGHT = 800,
              FPS           = 120;

constexpr char    BG_COLOUR[]      = "#DAD7CD";
constexpr Vector2 ORIGIN           = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
                  ATLAS_DIMENSIONS = { 4, 6 };

constexpr float TILE_DIMENSION          = 75.0f,
                // in m/ms², since delta time is in ms
                ACCELERATION_OF_GRAVITY = 981.0f,
                FIXED_TIMESTEP          = 1.0f / 60.0f,
                END_GAME_THRESHOLD      = 800.0f;

constexpr int LEVEL_WIDTH  = 24,
              LEVEL_HEIGHT = 12;

// Global Variables
AppStatus gAppStatus       = RUNNING;
float     gPreviousTicks   = 0.0f,
          gTimeAccumulator = 0.0f;


unsigned int gLevelData[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 2, 2, 2, 0, 4, 4, 4,
    0, 0, 0, 0, 0, 2, 2, 2, 0, 4, 4, 4, 0, 0, 0, 0, 0, 3, 3, 3, 0, 4, 4, 4,
    2, 2, 2, 2, 0, 3, 3, 3, 0, 4, 4, 4, 0, 2, 2, 2, 0, 4, 4, 4, 0, 4, 4, 4,
    3, 3, 3, 3, 0, 4, 4, 4, 0, 3, 3, 3, 0, 3, 3, 3, 0, 4, 4, 4, 0, 4, 4, 4,
    4, 4, 4, 4, 0, 4, 4, 4, 0, 4, 4, 4, 0, 4, 4, 4, 0, 4, 4, 4, 0, 4, 4, 4,
    4, 4, 4, 4, 0, 4, 4, 4, 0, 4, 4, 4, 0, 4, 4, 4, 0, 4, 4, 4, 0, 4, 4, 4
};

GameState gState;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
void panCamera(Camera2D *camera, const Vector2 *targetPosition);

/**
 * The function `panCamera` smoothly adjusts the camera's target position
 * towards a specified target position.
 *
 * @param camera The `camera` parameter is a pointer to a `Camera2D` struct,
 * which contains information about a 2D camera such as its position, target,
 * zoom level, and other properties.
 * @param targetPosition The `targetPosition` parameter is a pointer to a
 * `Vector2` structure that represents the desired position where the camera
 * should be focused or centered on.
 */
void panCamera(Camera2D *camera, const Vector2 *targetPosition)
{
    Vector2 positionDifference =
        Vector2Subtract(*targetPosition, camera->target);

    camera->target = Vector2Add(
        camera->target, Vector2Scale(positionDifference, 0.1f)
    ); // 0.1 = smoothing factor
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Blob Platformer");
    InitAudioDevice();
    SetMasterVolume(0.0f); // this exercise is silent: music and sound effects

    //gState.bgm = LoadMusicStream("assets/game/04 - Silent Forest.wav");
    SetMusicVolume(gState.bgm, 0.33f);
    PlayMusicStream(gState.bgm);

   // gState.jumpSound = LoadSound("assets/game/Dirt Jump.wav");

    /*
        ----------- MAP -----------
    */
    gState.map = new Map(
        LEVEL_WIDTH,                // map grid cols
        LEVEL_HEIGHT,               // map grid rows
        gLevelData,                 // grid data
        "assets/tileset.png",  // texture filepath
        TILE_DIMENSION,             // tile size
        4,                          // texture cols
        1,                          // texture rows
        ORIGIN                      // in-game origin
    );

    /*
        ----------- PROTAGONIST -----------
    */
    std::map<Direction, std::vector<int>> blobAnimationAtlas = {
        { STILL, { 0, 1, 2, 3, 4, 5 } },
        { RIGHT,  { 6, 7, 8, 9, 10, 11 } },
        { UP,    { 12, 13, 14, 15, 16, 17 } },
        { LEFT, { 18, 19, 20, 21, 22, 23 } },
    };

    float sizeRatio = 50.0f / 100.0f;

    // Assets from @see https://sscary.itch.io/the-adventurer-female
    gState.blob = new Entity(
        { 100.0f , ORIGIN.y  }, // position
        { 100.0f * sizeRatio, 100.0f },           // scale
        "assets/blob.png",                   // texture file address
        ATLAS,                                    // single image or atlas?
        ATLAS_DIMENSIONS,                         // atlas dimensions
        blobAnimationAtlas,                    // actual atlas
        PLAYER                                    // entity type
    );
    gState.blob->setHealth(6);
    //gState.blob->scaleSize();
    gState.blob->setJumpingPower(600.0f);
    gState.blob->setColliderDimensions({
        gState.blob->getScale().x / 3.5f,
        gState.blob->getScale().y / 3.0f
    });
    gState.blob->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });

    /*
        ----------- CAMERA -----------
    */
    gState.camera          = {};                           // zero initialise
    gState.camera.target   = gState.blob->getPosition(); // follows player
    gState.camera.offset   = ORIGIN;                        // centre of screen
    gState.camera.rotation = 0.0f;                          // no rotation
    gState.camera.zoom     = 1.0f;                          // default zoom

    SetTargetFPS(FPS);
}

void processInput()
{
    gState.blob->resetMovement();

    if      (IsKeyDown(KEY_A)) gState.blob->moveLeft();
    else if (IsKeyDown(KEY_D)) gState.blob->moveRight();

    if (IsKeyPressed(KEY_W) && gState.blob->isCollidingBottom())
    {
        gState.blob->jump();
        PlaySound(gState.jumpSound);
    }
    if (!IsKeyDown(KEY_A) && !IsKeyDown(KEY_D) && !IsKeyDown(KEY_W)) gState.blob->standStill();
    // to avoid faster diagonal speed
    if (GetLength(gState.blob->getMovement()) > 1.0f)
        gState.blob->normaliseMovement();

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
        UpdateMusicStream(gState.bgm);

        gState.blob->update(
            FIXED_TIMESTEP, // delta time / fixed timestep
            nullptr,        // player
            gState.map,     // map
            nullptr,        // collidable entities
            0               // col. entity count
        );
        deltaTime -= FIXED_TIMESTEP;

        Vector2 currentPlayerPosition = gState.blob->getPosition();

        panCamera(&gState.camera, &currentPlayerPosition);
    }

    gTimeAccumulator = deltaTime;

    if (gState.blob->getPosition().y > END_GAME_THRESHOLD)
        gAppStatus = TERMINATED;
}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    BeginMode2D(gState.camera);

    gState.blob->render();
    gState.map->render();

    EndMode2D();

    EndDrawing();
}

void shutdown()
{
    delete gState.blob;
    delete gState.map;

    UnloadMusicStream(gState.bgm);
    UnloadSound(gState.jumpSound);

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
        render();
    }

    shutdown();

    return 0;
}