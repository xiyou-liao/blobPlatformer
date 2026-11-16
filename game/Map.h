#ifndef MAP_H
#define MAP_H

#include "game.h"

class Map{
public:
    Map(
        int mapColumns,
        int mapRows,
        unsigned int* levelData,
        const char* textureFilePath,
        float tileSize,
        int textureColumns,
        int textureRows,
        Vector2 origin
    );
    ~Map();

    void build();
    void render();
    bool isSolidTileAt(Vector2 position, float *xOverlap, float *yOverlap);

    Vector2 getCoords(int row, int col) {
        return {
            mLeftBoundary + col * mTileSize + mTileSize/2.0f,
            mTopBoundary + row * mTileSize - mTileSize/2.0f
        };
    }
    int getMapColumns() const { return mMapColumns; }
    int getMapRows() const { return mMapRows; }
    float getTileSize() const { return mTileSize; }
    unsigned int* getLevelData() const { return mLevelData; }
    Texture2D getTextureAtlas() const { return mTextureAtlas; }
    int getTextureColumns() const { return mTextureColumns; }
    int getTextureRows() const { return mTextureRows; }
    float getLeftBoundary() const { return mLeftBoundary; }
    float getRightBoundary() const { return mRightBoundary; }
    float getTopBoundary() const { return mTopBoundary; }
    float getBottomBoundary() const { return mBottomBoundary; }


private:
    int mMapColumns;
    int mMapRows;

    unsigned int* mLevelData;
    Texture2D mTextureAtlas;

    float mTileSize;

    int mTextureColumns;
    int mTextureRows;

    std::vector<Rectangle> mTextureAreas;
    Vector2 mOrigin;

    float mLeftBoundary;
    float mRightBoundary;
    float mTopBoundary;
    float mBottomBoundary;
};

#endif //MAP_H