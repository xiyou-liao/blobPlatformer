#ifndef ENTITY_H
#define ENTITY_H

#include "Map.h"

enum Direction    { LEFT, UP, RIGHT, STILL, THROW, HURT, DOWN              }; // For walking
enum EntityStatus { ACTIVE, INACTIVE                   };
enum EntityType   { PLAYER, BLOCK, PLATFORM, NPC, NONE, KEY, DOOR };
enum AIType       { WANDERER, FOLLOWER, LERPER, JUMPER, FLYER };
enum AIState      { WALKING, IDLE, FOLLOWING           };

class Entity
{
private:
    Vector2 mPosition;
    Vector2 mMovement;
    Vector2 mVelocity;
    Vector2 mAcceleration;

    Vector2 mScale;
    Vector2 mColliderDimensions;

    Texture2D   mTexture;
    TextureType mTextureType;
    Vector2     mSpriteSheetDimensions;

    float mHighestY;
    std::pair<Vector2, Vector2> mFlyBoundaries;

    std::map<Direction, std::vector<int>> mAnimationAtlas;
    std::vector<int>                      mAnimationIndices;
    Direction                             mDirection;
    int                                   mFrameSpeed;

    int mass = 0;
    int   mCurrentFrameIndex = 0;
    float mAnimationTime     = 0.0f;
    float mDamageCooldown = 0.0f;
    bool  mIsJumping    = false;
    float mJumpingPower = 0.0f;

    int   mSpeed;
    float mAngle;
    float mLerpFactor = 2.0f;

    bool mIsCollidingTop    = false;
    bool mIsCollidingBottom = false;
    bool mIsCollidingRight  = false;
    bool mIsCollidingLeft   = false;

    EntityStatus mEntityStatus = ACTIVE;
    EntityType   mEntityType;

    AIType  mAIType;
    AIState mAIState;
    bool acquired = false;
    bool isFlying = false;
    bool exitReached = false;
    bool mTakingDamage = false;
    bool isColliding(Entity *other) const;

    void checkCollisionY(const std::vector<Entity*>& collidableEntities);
    void checkCollisionY(Map *map);

    void checkCollisionX(const std::vector<Entity*>& collidableEntities);
    void checkCollisionX(Map *map);

    void resetColliderFlags()
    {
        mIsCollidingTop    = false;
        mIsCollidingBottom = false;
        mIsCollidingRight  = false;
        mIsCollidingLeft   = false;
    }

    void animate(float deltaTime);
    void AIActivate(Entity *target, float deltaTime);
    void AIWander();
    void AIFollow(Entity *target);
    void AILerp(Entity* target, float deltaTime);
     void AIFly(float deltaTime);
public:
    static constexpr int   DEFAULT_SIZE          = 250;
    static constexpr int   DEFAULT_SPEED         = 200;
    static constexpr int   DEFAULT_FRAME_SPEED   = 14;
    static constexpr float Y_COLLISION_THRESHOLD = 0.5f;

    Entity();
    Entity(
        Vector2     position,
        Vector2     scale,
        const char *textureFilepath,
        EntityType  entityType
    );
    Entity(
        Vector2                               position,
        Vector2                               scale,
        const char                           *textureFilepath,
        TextureType                           textureType,
        Vector2                               spriteSheetDimensions,
        std::map<Direction, std::vector<int>> animationAtlas,
        EntityType                            entityType
    );
    ~Entity();

    void update(
        float   deltaTime,
        Entity *player,
        Map    *map,
        const std::vector<Entity*>& collidableEntities
    );
    void render();
    void normalizeMovement() { Normalize(&mMovement); }

    void jump()       { mIsJumping = true;         }
    void throwPlatform();
    void activate()   { mEntityStatus = ACTIVE;    }
    void deactivate() { mEntityStatus = INACTIVE;  }
    void displayCollider();

    bool takeDamage();
    bool isActive() { return mEntityStatus == ACTIVE; }
    void stopFlying() {
        mAcceleration = {0.0f, 0.0f};
        isFlying = false;
    }
    void moveUp()    { mMovement.y = -1; mDirection = UP;    }
    //void moveDown()  { mMovement.y =  1; mDirection = DOWN;  }
    void moveLeft()  { mMovement.x = -1; mDirection = LEFT;  }
    void moveRight() { mMovement.x =  1; mDirection = RIGHT; }
    void standStill() {mDirection = STILL;}

    void resetMovement() { mMovement = { 0.0f, 0.0f }; }

    Vector2     getPosition()              const { return mPosition;              }
    Vector2     getMovement()              const { return mMovement;              }
    Vector2     getVelocity()              const { return mVelocity;              }
    Vector2     getAcceleration()          const { return mAcceleration;          }
    Vector2     getScale()                 const { return mScale;                 }
    Vector2     getColliderDimensions()    const { return mColliderDimensions;    }
    Vector2     getSpriteSheetDimensions() const { return mSpriteSheetDimensions; }
    Texture2D   getTexture()               const { return mTexture;               }
    TextureType getTextureType()           const { return mTextureType;           }
    Direction   getDirection()             const { return mDirection;             }
    int         getFrameSpeed()            const { return mFrameSpeed;            }
    float       getJumpingPower()          const { return mJumpingPower;          }
    bool        isJumping()                const { return mIsJumping;             }
    int         getSpeed()                 const { return mSpeed;                 }
    float       getAngle()                 const { return mAngle;                 }
    EntityType  getEntityType()            const { return mEntityType;            }
    AIType      getAIType()                const { return mAIType;                }
    AIState     getAIState()               const { return mAIState;               }

    int getMass() const {return mass;}
    void decreaseMass() {mass--;}
    void setFlying() {
        isFlying = true;
        mHighestY = mPosition.y;
    }
    bool getAcquired() const {
        return acquired;
    }
    bool getExitReached() const {return exitReached;}
    bool isTakingDamage() const {return mTakingDamage;}

    bool isCollidingTop()    const { return mIsCollidingTop;    }
    bool isCollidingBottom() const { return mIsCollidingBottom; }

    std::map<Direction, std::vector<int>> getAnimationAtlas() const { return mAnimationAtlas; }

    void setMass(int newMass) {mass = newMass;}
    void scaleSize() {
        mScale = {mScale.x * static_cast<float>(mass), mScale.y * static_cast<float>(mass)};
    }
    void setTakingDamage(bool change) {mTakingDamage = change;}

    void setVelocity (Vector2 newVelocity) {
        mVelocity = newVelocity;
        mHighestY = mPosition.y;
    }
    void setAcquired() {
        acquired = true;
        //mEntityStatus = INACTIVE;
    }
    void setFlyBoundaries(std::pair<Vector2, Vector2> boundaries){mFlyBoundaries = boundaries;}
    void setPosition(Vector2 newPosition)
        { mPosition = newPosition;                 }
    void setMovement(Vector2 newMovement)
        { mMovement = newMovement;                 }
    void setAcceleration(Vector2 newAcceleration)
        { mAcceleration = newAcceleration;         }
    void setScale(Vector2 newScale)
        { mScale = newScale;                       }
    void setTexture(const char *textureFilepath)
        { mTexture = LoadTexture(textureFilepath); }
    void setColliderDimensions(Vector2 newDimensions)
        { mColliderDimensions = newDimensions;     }
    void setSpriteSheetDimensions(Vector2 newDimensions)
        { mSpriteSheetDimensions = newDimensions;  }
    void setSpeed(int newSpeed)
        { mSpeed = newSpeed;                       }
    void setFrameSpeed(int newSpeed)
        { mFrameSpeed = newSpeed;                  }
    void setJumpingPower(float newJumpingPower)
        { mJumpingPower = newJumpingPower;         }
    void setAngle(float newAngle)
        { mAngle = newAngle;                       }
    void setEntityType(EntityType entityType)
        { mEntityType = entityType;                }
    void setAIState(AIState newState)
        { mAIState = newState;                     }
    void setAIType(AIType newType)
        { mAIType = newType;                       }
    void setDirection(Direction newDirection)
    {
        mDirection = newDirection;

        if (mTextureType == ATLAS)
            mAnimationIndices = mAnimationAtlas.at(mDirection);
    }
};

#endif //ENTITY_H