//Xiyou Liao
//Game.cpp

#include "Game.h"

Color ColorFromHex(const char *hex) {
    if (hex[0] == '#') hex++;
    unsigned int r = 0, g = 0, b = 0, a = 255;

    // 6‑digit form: RRGGBB
    if (sscanf(hex, "%02x%02x%02x", &r, &g, &b) == 3) {
        return Color{ static_cast<unsigned char>(r),
                      static_cast<unsigned char>(g),
                      static_cast<unsigned char>(b),
                      static_cast<unsigned char>(a) };
    }

    // 8‑digit form: RRGGBBAA
    if (sscanf(hex, "%02x%02x%02x%02x", &r, &g, &b, &a) == 4){
        return Color{ static_cast<unsigned char>(r),
                      static_cast<unsigned char>(g),
                      static_cast<unsigned char>(b),
                      static_cast<unsigned char>(a) };
    }
    return RAYWHITE;
}

float GetLength(const Vector2 vector) {
    return sqrtf(pow(vector.x, 2) + pow(vector.y, 2));
}

void Normalize(Vector2 *vector) {
    float magnitude = GetLength(*vector);
    vector->x /= magnitude;
    vector->y /= magnitude;
}

Rectangle getUVRectangle(const Texture2D *texture, int index, int rows, int cols) {
    float uCoord = static_cast<float>(index % cols) / static_cast<float>(cols);
    uCoord *= texture->width;

    float vCoord = static_cast<float>(index / cols) / static_cast<float>(rows);
    vCoord *= texture->height;

    float sliceWidth  = texture->width  / static_cast<float>(cols);
    float sliceHeight = texture->height / static_cast<float>(rows);

    return {uCoord,vCoord, sliceWidth, sliceHeight };
}

void panCamera(Camera2D *camera, const Vector2 *targetPosition) {
    Vector2 positionDifference = Vector2Subtract(
        *targetPosition, camera->target
    );

    camera->target = Vector2Add(
        camera->target, Vector2Scale(positionDifference, 0.1f)
    ); // 0.1 = smoothing factor
}
