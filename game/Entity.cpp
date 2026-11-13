#include "Entity.h"

Entity::Entity() :
    mPosition{ 0.0f, 0.0f }, mMovement{ 0.0f, 0.0f },
    mVelocity{ 0.0f, 0.0f }, mAcceleration{ 0.0f, 0.0f },
    mScale{ DEFAULT_SIZE, DEFAULT_SIZE },
    mColliderDimensions{ DEFAULT_SIZE, DEFAULT_SIZE },
    mTexture{}, mTextureType{ SINGLE }, mSpriteSheetDimensions{},
    mAnimationAtlas{}, mAnimationIndices{}, mDirection{ RIGHT },
    mFrameSpeed{ 0 }, mSpeed{ DEFAULT_SPEED }, mAngle{ 0.0f },
    mEntityType{ NONE }, mAIType{ WANDERER }, mAIState{ IDLE } {}

Entity::Entity(
    Vector2     position,
    Vector2     scale,
    const char *textureFilepath,
    EntityType  entityType
) :
    mPosition{ position }, mMovement{ 0.0f, 0.0f },
    mVelocity{ 0.0f, 0.0f }, mAcceleration{ 0.0f, 0.0f },
    mScale{ scale }, mColliderDimensions{ scale },
    mTexture{ LoadTexture(textureFilepath) }, mTextureType{ SINGLE },
    mSpriteSheetDimensions{}, mAnimationAtlas{}, mAnimationIndices{},
    mDirection{ RIGHT }, mFrameSpeed{ 0 }, mSpeed{ DEFAULT_SPEED },
    mAngle{ 0.0f }, mEntityType{ entityType }, mAIType{ WANDERER },
    mAIState{ IDLE } {}

Entity::Entity(
    Vector2                               position,
    Vector2                               scale,
    const char                           *textureFilepath,
    TextureType                           textureType,
    Vector2                               spriteSheetDimensions,
    std::map<Direction, std::vector<int>> animationAtlas,
    EntityType                            entityType
) :
    mPosition{ position }, mMovement{ 0.0f, 0.0f },
    mVelocity{ 0.0f, 0.0f }, mAcceleration{ 0.0f, 0.0f },
    mScale{ scale }, mColliderDimensions{ scale },
    mTexture{ LoadTexture(textureFilepath) }, mTextureType{ textureType },
    mSpriteSheetDimensions{ spriteSheetDimensions },
    mAnimationAtlas{ animationAtlas },
    mAnimationIndices{ animationAtlas.at(RIGHT) }, mDirection{ RIGHT },
    mFrameSpeed{ DEFAULT_FRAME_SPEED }, mSpeed{ DEFAULT_SPEED },
    mAngle{ 0.0f }, mEntityType{ entityType }, mAIType{ WANDERER },
    mAIState{ IDLE } {}

Entity::~Entity() { UnloadTexture(mTexture); }

/**
 * Iterates through a list of collidable entities, checks for collisions with
 * the player entity, and resolves any vertical overlap by adjusting the
 * player's position and velocity accordingly.
 *
 * @param collidableEntities An array of pointers to `Entity` objects that
 * represent the entities that the current `Entity` instance can potentially
 * collide with. The `collisionCheckCount` parameter specifies the number of
 * entities in the `collidableEntities` array that need to be checked for
 * collision.
 * @param collisionCheckCount The number of entities that the current entity
 * (`Entity`) should check for collisions with. This parameter specifies how
 * many entities are in the `collidableEntities` array that need to be checked
 * for collisions with the current entity.
 */
void Entity::checkCollisionY(
    const std::vector<Entity*>& collidableEntities
)
{
    for (size_t i = 0; i<collidableEntities.size(); i++)
    {
        // STEP 1: For every entity that our player can collide with...
        Entity* collidableEntity = collidableEntities[i];
        if (!collidableEntity->isActive()) continue;
        if (isColliding(collidableEntity))
        {
            // STEP 2: Calculate the distance between its centre and our centre
            //         and use that to calculate the amount of overlap between
            //         both bodies.
            float yDistance = fabs(mPosition.y - collidableEntity->mPosition.y);
            float yOverlap  = fabs(
                yDistance - (mColliderDimensions.y / 2.0f) -
                (collidableEntity->mColliderDimensions.y / 2.0f)
            );

            // STEP 3: "Unclip" ourselves from the other entity, and zero our
            //         vertical velocity.
            if (mVelocity.y > 0)
            {
                mPosition.y        -= yOverlap;
                mVelocity.y         = 0;
                mIsCollidingBottom  = true;
                if (mEntityType == PLAYER) {
                    if (collidableEntity->getEntityType() == KEY) collidableEntity->setAcquired();
                    if (collidableEntity->getEntityType() == DOOR) exitReached = true;
                    if (collidableEntity->getEntityType() == NPC) {
                        if (takeDamage()) mTakingDamage = true;
                    }
                }
            }
            else if (mVelocity.y < 0)
            {
                mPosition.y     += yOverlap;
                mVelocity.y      = 0;
                mIsCollidingTop  = true;
                if (mEntityType == PLAYER) {
                    if (collidableEntity->getEntityType() == KEY) collidableEntity->setAcquired();
                    if (collidableEntity->getEntityType() == DOOR) exitReached = true;
                    if (collidableEntity->getEntityType() == NPC) {
                        if (takeDamage()) mTakingDamage = true;
                    }
                }
            }

        }
    }
}

void Entity::checkCollisionX(
    const std::vector<Entity*>& collidableEntities
)
{
    for (size_t i = 0; i<collidableEntities.size(); i++)
    {
        // STEP 1: For every entity that our player can collide with...
        Entity* collidableEntity = collidableEntities[i];
     //   if (!collidableEntity->isActive()) continue;
        if (isColliding(collidableEntity))
        {
            // When standing on a platform, we're always slightly overlapping
            // it vertically due to gravity, which causes false horizontal
            // collision detections. So the solution I found is only resolve X
            // collisions if there's significant Y overlap, preventing the
            // platform we're standing on from acting like a wall.
            float yDistance = fabs(mPosition.y - collidableEntity->mPosition.y);
            float yOverlap  = fabs(
                yDistance - (mColliderDimensions.y / 2.0f) -
                (collidableEntity->mColliderDimensions.y / 2.0f)
            );

            // Skip if barely touching vertically (standing on platform)
            if (yOverlap < Y_COLLISION_THRESHOLD) continue;

            float xDistance = fabs(mPosition.x - collidableEntity->mPosition.x);
            float xOverlap  = fabs(
                xDistance - (mColliderDimensions.x / 2.0f) -
                (collidableEntity->mColliderDimensions.x / 2.0f)
            );

            if (mVelocity.x > 0)
            {
                mPosition.x -= xOverlap;
                mVelocity.x  = 0;

                // Collision!
                mIsCollidingRight = true;
                if (mEntityType == PLAYER) {
                    if (collidableEntity->getEntityType() == KEY) collidableEntity->setAcquired();
                    if (collidableEntity->getEntityType() == DOOR) exitReached = true;
                    if (collidableEntity->getEntityType() == NPC) {
                        if (takeDamage()) mTakingDamage = true;
                    }
                }
            }
            else if (mVelocity.x < 0)
            {
                mPosition.x += xOverlap;
                mVelocity.x  = 0;

                // Collision!
                mIsCollidingLeft = true;
                if (mEntityType == PLAYER) {
                    if (collidableEntity->getEntityType() == KEY) collidableEntity->setAcquired();
                    if (collidableEntity->getEntityType() == DOOR) exitReached = true;
                    if (collidableEntity->getEntityType() == NPC) {
                        if (takeDamage()) mTakingDamage = true;
                    }
                }
            }
        }
    }
}

void Entity::checkCollisionY(Map *map)
{
    if (map == nullptr) return;

    Vector2 topCentreProbe = {
        mPosition.x, mPosition.y - (mColliderDimensions.y / 2.0f)
    };
    Vector2 topLeftProbe = {
        mPosition.x - (mColliderDimensions.x / 2.0f),
        mPosition.y - (mColliderDimensions.y / 2.0f)
    };
    Vector2 topRightProbe = {
        mPosition.x + (mColliderDimensions.x / 2.0f),
        mPosition.y - (mColliderDimensions.y / 2.0f)
    };

    Vector2 bottomCentreProbe = {
        mPosition.x, mPosition.y + (mColliderDimensions.y / 2.0f)
    };
    Vector2 bottomLeftProbe = {
        mPosition.x - (mColliderDimensions.x / 2.0f),
        mPosition.y + (mColliderDimensions.y / 2.0f)
    };
    Vector2 bottomRightProbe = {
        mPosition.x + (mColliderDimensions.x / 2.0f),
        mPosition.y + (mColliderDimensions.y / 2.0f)
    };

    float xOverlap = 0.0f;
    float yOverlap = 0.0f;

    // COLLISION ABOVE (jumping upward)
    if ((map->isSolidTileAt(topCentreProbe, &xOverlap, &yOverlap) ||
         map->isSolidTileAt(topLeftProbe, &xOverlap, &yOverlap) ||
         map->isSolidTileAt(topRightProbe, &xOverlap, &yOverlap)) &&
        mVelocity.y < 0.0f && xOverlap < map->getTileSize() * 0.4f)
    {
        mPosition.y     += yOverlap * 1.01f; // push down
        mVelocity.y      = 0.0f;
        mIsCollidingTop  = true;
        if (mEntityType == PLATFORM) {
            setAcceleration({0.0f, 0.0f});
            setVelocity({0.0f, 0.0f});
        }
    }

    // COLLISION BELOW (falling downward)
    if ((map->isSolidTileAt(bottomCentreProbe, &xOverlap, &yOverlap) ||
         map->isSolidTileAt(bottomLeftProbe, &xOverlap, &yOverlap) ||
         map->isSolidTileAt(bottomRightProbe, &xOverlap, &yOverlap)) &&
        mVelocity.y > 0.0f)
    {
        mPosition.y        -= yOverlap; // push up
        mVelocity.y         = 0.0f;
        mIsCollidingBottom  = true;
        if (mEntityType == PLATFORM) {
            setAcceleration({0.0f, 0.0f});
            setVelocity({0.0f, 0.0f});
        }
        if (mEntityType == NPC && mAIType == JUMPER) {
            setVelocity({0.0f, -mJumpingPower});
        }
    }
}

void Entity::checkCollisionX(Map *map)
{
    if (map == nullptr) return;

    Vector2 leftCentreProbe = {
        mPosition.x - (mColliderDimensions.x / 2.0f), mPosition.y
    };
    Vector2 rightCentreProbe = {
        mPosition.x + (mColliderDimensions.x / 2.0f), mPosition.y
    };
    Vector2 topLeftProbe = {
        mPosition.x - (mColliderDimensions.x / 2.0f),
        mPosition.y - (mColliderDimensions.y / 2.0f)
    };
    Vector2 topRightProbe = {
        mPosition.x + (mColliderDimensions.x / 2.0f),
        mPosition.y - (mColliderDimensions.y / 2.0f)
    };

    Vector2 bottomLeftProbe = {
        mPosition.x - (mColliderDimensions.x / 2.0f),
        mPosition.y + (mColliderDimensions.y / 2.0f)
    };
    Vector2 bottomRightProbe = {
        mPosition.x + (mColliderDimensions.x / 2.0f),
        mPosition.y + (mColliderDimensions.y / 2.0f)
    };

    float xOverlap = 0.0f;
    float yOverlap = 0.0f;

    // COLLISION ON RIGHT (moving right)
    if ((map->isSolidTileAt(rightCentreProbe, &xOverlap, &yOverlap) ||
        map->isSolidTileAt(topRightProbe, &xOverlap, &yOverlap) ||
        map->isSolidTileAt(bottomRightProbe, &xOverlap, &yOverlap))    &&
             mVelocity.x > 0.0f && yOverlap >= 0.5f)
    {
        mPosition.x       -= xOverlap * 1.01f; // push left
        mVelocity.x        = 0.0f;
        mIsCollidingRight  = true;
        if (mEntityType == PLATFORM) {
            setAcceleration({0.0f, 0.0f});
            setVelocity({0.0f, 0.0f});
        }
    }

    // COLLISION ON LEFT (moving left)
    if ((map->isSolidTileAt(leftCentreProbe, &xOverlap, &yOverlap) ||
        map->isSolidTileAt(topLeftProbe, &xOverlap, &yOverlap) ||
        map->isSolidTileAt(bottomLeftProbe, &xOverlap, &yOverlap)) &&
        mVelocity.x < 0.0f && yOverlap >= 0.5f)
    {
        mPosition.x      += xOverlap * 1.01f; // push right
        mVelocity.x       = 0.0f;
        mIsCollidingLeft  = true;
        if (mEntityType == PLATFORM) {
            setAcceleration({0.0f, 0.0f});
            setVelocity({0.0f, 0.0f});
        }
    }
}

/**
 * Checks if two entities are colliding based on their positions and collider
 * dimensions.
 *
 * @param other represents another Entity with which you want to check for
 * collision. It is a pointer to the Entity class.
 *
 * @return returns `true` if the two entities are colliding based on their
 * positions and collider dimensions, and `false` otherwise.
 */
bool Entity::isColliding(Entity *other) const
{
    if (!other->isActive() || other == this) return false;

    float xDistance = fabs(mPosition.x - other->getPosition().x) -
        ((mColliderDimensions.x + other->getColliderDimensions().x) / 2.0f);
    float yDistance = fabs(mPosition.y - other->getPosition().y) -
        ((mColliderDimensions.y + other->getColliderDimensions().y) / 2.0f);

    return xDistance < 0.0f && yDistance < 0.0f;
}

/**
 * Updates the current frame index of an entity's animation based on the
 * elapsed time and frame speed.
 *
 * @param deltaTime represents the time elapsed since the last frame update.
 */
void Entity::animate(float deltaTime)
{
    if (mTakingDamage) mAnimationIndices = mAnimationAtlas.at(HURT);
    else mAnimationIndices = mAnimationAtlas.at(mDirection);


    mAnimationTime += deltaTime;
    float secondsPerFrame = 1.0f / mFrameSpeed;

    if (mAnimationTime >= secondsPerFrame)
    {
        mAnimationTime = 0.0f;
        mCurrentFrameIndex++;
        if (mTakingDamage) {
            // Play damage animation only once
            if (mCurrentFrameIndex >= mAnimationIndices.size()) {
                mTakingDamage = false;
                mCurrentFrameIndex = 0;
            }
        }
        else{mCurrentFrameIndex %= mAnimationIndices.size();}
    }
}

// void Entity::throwPlatform() {
//     mDirection = THROW;
// }
bool Entity::takeDamage()
{
     if (mDamageCooldown > 0.0f) return false;

    mCurrentFrameIndex = 0;
    mAnimationTime = 0.0f;
    mDamageCooldown = 1.0f;
    return true;
}
void Entity::displayCollider()
{
    // draw the collision box
    Rectangle colliderBox = {
        mPosition.x - mColliderDimensions.x / 2.0f,
        mPosition.y - mColliderDimensions.y / 2.0f,
        mColliderDimensions.x,
        mColliderDimensions.y
    };

    DrawRectangleLines(
        colliderBox.x,      // Top-left X
        colliderBox.y,      // Top-left Y
        colliderBox.width,  // Width
        colliderBox.height, // Height
        GREEN               // Color
    );
}

void Entity::AIWander(){moveLeft();}

void Entity::AIFollow(Entity *target)
{
    switch (mAIState)
    {
    case IDLE:
        // If the player approaches...
        if (Vector2Distance(mPosition, target->getPosition()) < 250.0f)
            // Switch to walking
            mAIState = WALKING;
        break;
    case WALKING:
        // Depending on where the player is in respect to their x-position
        // change direction of the NPC
        if (mPosition.x > target->getPosition().x)
            moveLeft();
        else
            moveRight();
        break;
    default:
        break;
    }
}
void Entity::AIFly(float deltaTime){
    // if (mDirection == RIGHT && mPosition.x < mFlyBoundaries.second.x) mPosition.x += mSpeed * deltaTime;
    // else if (mDirection == LEFT && mPosition.x > mFlyBoundaries.first.x) mPosition.x -= mSpeed * deltaTime;
    // if (mPosition.x > mFlyBoundaries.second.x) {
    //     mPosition.x = mFlyBoundaries.second.x - 10.0f;
    //     mDirection = LEFT;
    // }
    // else if (mPosition.x < mFlyBoundaries.first.x) {
    //     mPosition.x = mFlyBoundaries.first.x + 10.0f;
    //     mDirection = RIGHT;
    // }
}
void Entity::AILerp(Entity *target, float deltaTime)
{
    if (target->getPosition().x < mPosition.x) mDirection = LEFT;
    else mDirection = RIGHT;

    float distance = Vector2Distance(mPosition, target->getPosition());
    switch (mAIState) {
        case IDLE:
            if (distance < 350.0f)
                mAIState = FOLLOWING;
            break;
        case FOLLOWING:
            if (distance > 150.0f) {
                mPosition = mPosition + (target->getPosition() - mPosition) * mLerpFactor * deltaTime;
            }

    }
}

// void Entity::AIJump() {
//     if (mIsJumping)
//     {
//         // STEP 1: Immediately return the flag to its original false state
//         mIsJumping = false;
//
//         // STEP 2: The player now acquires an upward velocity
//         mVelocity.y = -mJumpingPower;
//     }
//     else if (mIsJumping == false && mVelocity.y == 0.0f) mIsJumping = true;
// }


void Entity::AIActivate(Entity *target, float deltaTime)
{
    switch (mAIType)
    {
    case WANDERER:
        AIWander();
        break;
    case FOLLOWER:
        AIFollow(target);
        break;
    case LERPER:
        AILerp(target, deltaTime);
        break;
    // case FLYER:
    //     AIFly(deltaTime);
    //     break;
    default:
        break;
    }
}


void Entity::update(
    float   deltaTime,
    Entity *player,
    Map    *map,
    const std::vector<Entity*>& collidableEntities
)
{
    if (mEntityStatus == INACTIVE) return;

    if (mEntityType == NPC && mAIType != JUMPER && mAIType != FLYER) AIActivate(player, deltaTime);

    if (mEntityType == PLAYER) {
        mScale = {100.0f * 45.0f / 60.0f * mass / 2.0f, 100.0f * mass / 2.0f};
        mColliderDimensions = {
        mScale.x / 3.0f,
        mScale.y / 2.5f
    };
    }

    resetColliderFlags();

    if (mEntityType == PLATFORM && isFlying) {
        mVelocity.x += mAcceleration.x * deltaTime;
        mVelocity.y += mAcceleration.y * deltaTime;
        mPosition.y += mVelocity.y * deltaTime;
        checkCollisionY(collidableEntities);
        checkCollisionY(map);
        mPosition.x += mVelocity.x * deltaTime;
        checkCollisionX(collidableEntities);
        checkCollisionX(map);
        int maxFallDistance = 100.0f;
       // float highestY = mPosition.y;
        if (mPosition.y < mHighestY) {
            mHighestY = mPosition.y;
        }
        if (mPosition.y - mHighestY >= maxFallDistance) {
            mPosition.y = mHighestY + maxFallDistance;
            stopFlying();
        }
    }
    if (mEntityType != PLATFORM && mEntityType != KEY && mAIType != FLYER) {
        mVelocity.x = mMovement.x * mSpeed;
        mVelocity.x += mAcceleration.x * deltaTime;
        mVelocity.y += mAcceleration.y * deltaTime;
    }

    if (mAIType == FLYER) {
        if (mDirection == RIGHT && mPosition.x < mFlyBoundaries.second.x) mPosition.x += mSpeed * deltaTime;
        else if (mDirection == LEFT && mPosition.x > mFlyBoundaries.first.x) mPosition.x -= mSpeed * deltaTime;
        if (mPosition.x > mFlyBoundaries.second.x) {
            mPosition.x = mFlyBoundaries.second.x - 10.0f;
            mDirection = LEFT;
        }
        else if (mPosition.x < mFlyBoundaries.first.x) {
            mPosition.x = mFlyBoundaries.first.x + 10.0f;
            mDirection = RIGHT;
        }
    }
    //if (mEntityType == PLAYER) scaleSize();
    // ––––– JUMPING ––––– //
    if (mIsJumping)
    {
        // STEP 1: Immediately return the flag to its original false state
        mIsJumping = false;

        // STEP 2: The player now acquires an upward velocity
        mVelocity.y -= mJumpingPower;
    }

    if (mEntityType != PLATFORM && mEntityType!= KEY && mEntityType != FLYER) {
        mPosition.y += mVelocity.y * deltaTime;
        checkCollisionY(collidableEntities);
        checkCollisionY(map);

        mPosition.x += mVelocity.x * deltaTime;
        checkCollisionX(collidableEntities);
        checkCollisionX(map);
    }
        // Only animate if we're on the ground (not necessary)
    if (mTextureType == ATLAS && GetLength(mMovement) != 0 &&
        mIsCollidingBottom)
        animate(deltaTime);

    if (mDamageCooldown > 0.0f) {
        mDamageCooldown -= deltaTime;
    }

}

void Entity::render()
{
    if (mEntityStatus == INACTIVE) return;

    Rectangle textureArea;

    switch (mTextureType)
    {
    case SINGLE:
        // Whole texture (UV coordinates)
        textureArea = {
            // top-left corner
            0.0f, 0.0f,

            // bottom-right corner (of texture)
            static_cast<float>(mTexture.width),
            static_cast<float>(mTexture.height)
        };
        break;
    case ATLAS:
        textureArea = getUVRectangle(
            &mTexture,
            mAnimationIndices[mCurrentFrameIndex],
            mSpriteSheetDimensions.x,
            mSpriteSheetDimensions.y
        );
        break;
    default:
        break;
    }

    // Destination rectangle – centred on mPosition
    Rectangle destinationArea = {
        mPosition.x,
        mPosition.y,
        static_cast<float>(mScale.x),
        static_cast<float>(mScale.y)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 originOffset = {
        static_cast<float>(mScale.x) / 2.0f,
        static_cast<float>(mScale.y) / 2.0f
    };

    // Render the texture on screen
    if (mEntityType == NPC) {
        if (mDirection == RIGHT) textureArea.width *= -1.0f;
        if (mDirection == DOWN && textureArea.height > 0.0f) textureArea.height *= -1.0f;
    }
    DrawTexturePro(
        mTexture,
        textureArea, destinationArea, originOffset,
        mAngle, WHITE
    );

     displayCollider();
}