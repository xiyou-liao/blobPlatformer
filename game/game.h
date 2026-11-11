#ifndef GAME_H
#define GAME_H


#define LOG(argument) std::cout << argument << '\n'


#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include <iostream>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <map>
#include <vector>
#include <string>

#define LOG(argument) std::cout << argument << '\n'

enum AppStatus   { TERMINATED, RUNNING                  };
enum TextureType { SINGLE, ATLAS                        };
enum SceneID     { NO_SCENE = -1, LEVEL_A, LEVEL_B, LEVEL_C      };

Color     ColorFromHex(const char *hex);
void      Normalize(Vector2 *vector);
float     GetLength(const Vector2 vector);
Rectangle getUVRectangle(const Texture2D *texture, int index, int rows, int cols);
void      panCamera(Camera2D *camera, const Vector2 *targetPosition);

#endif //GAME_H