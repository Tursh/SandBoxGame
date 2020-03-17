//
// Created by tursh on 4/30/19.
//

#include "World/World.h"
#include <glm/common.hpp>
#include <glm/gtx/string_cast.hpp>
#include <climits>
#include <IO/Window.h>
#include <Utils/TimeUtils.h>
#include <Text/TextRenderer.h>
#include <Utils/Log.h>

void World::tick()
{
    //TODO create a world generator and check when the player move if chunks can be deleted

    player_->move(0.025f, this);
    for (const auto &entity : entities_)
    {
        entity->update();
    }
    camera_.followPlayer(player_);
    player_->checkAction(this);
}

void World::render()
{
    shader.start();
    shader.loadMatrix(CGE::Shader::VIEW, camera_.toViewMatrix());
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CW);
    glEnable(GL_CULL_FACE);
    for (auto &yChunks : chunks_)
        for (auto &zChunks : yChunks.second)
            for (auto &chunkPair : zChunks.second)
            {
                auto chunk = chunkPair.second;
                if (chunk != nullptr && chunk->isLoaded())
                {
                    shader.loadMatrix(CGE::Shader::TRANSFORMATION,
                                      glm::translate(glm::mat4(1), (glm::vec3) chunk->getChunkPosition() *
                                                                   (float) CHUNK_SIZE));
                    chunk->render();
                }
            }

    shader.stop();

    cloudManager_.render(camera_);

    glDisable(GL_CULL_FACE);

    glDisable(GL_DEPTH_TEST);


    CGE::Text::textRenderer::renderText("FPS: " + std::to_string(CGE::Utils::getFPS()).substr(0, 4) + " TPS: " +
                                        std::to_string(CGE::Utils::TPSClock::getTPS()).substr(0, 4), 0.66f, 0.95f, 0.1f,
                                        glm::vec3(1, 1, 1),
                                        false);

    CGE::Text::textRenderer::renderText(glm::to_string(camera_.position_), -1, 0.95f, 0.1f, glm::vec3(1, 1, 1),
                                        false);

    CGE::Text::textRenderer::renderText("X", 0, 0, 0.2f, glm::vec3(1, 1, 1),
                                        false);

    CGE::Text::textRenderer::renderText(glm::to_string(player_->getSpeed()), -1, 0.90f, 0.1f,
                                        glm::vec3(1, 1, 1),
                                        false);
    CGE::Text::textRenderer::renderText(glm::to_string(getChunkPosition(camera_.position_)), -1, 0.85f, 0.1f,
                                        glm::vec3(1, 1, 1),
                                        false);
    deleteBufferedChunks();
}


Chunk *World::getChunk(glm::ivec3 position)
{
    position = glm::floor((glm::vec3) position / (float) CHUNK_SIZE);
    return getChunkByChunkPosition(position);
}

Chunk *World::getChunk(const glm::vec3 &position)
{
    return getChunk((glm::ivec3) glm::floor(position));
}

Chunk **World::getAroundChunk(glm::ivec3 chunkPosition)
{
    auto **chunkList = new Chunk *[6];

    chunkList[0] = getChunkByChunkPosition(chunkPosition + glm::ivec3(-1, 0, 0));
    chunkList[1] = getChunkByChunkPosition(chunkPosition + glm::ivec3(1, 0, 0));
    chunkList[2] = getChunkByChunkPosition(chunkPosition + glm::ivec3(0, -1, 0));
    chunkList[3] = getChunkByChunkPosition(chunkPosition + glm::ivec3(0, 1, 0));
    chunkList[4] = getChunkByChunkPosition(chunkPosition + glm::ivec3(0, 0, -1));
    chunkList[5] = getChunkByChunkPosition(chunkPosition + glm::ivec3(0, 0, 1));

    return chunkList;
}


Chunk *World::getChunkByChunkPosition(glm::ivec3 chunkPosition)
{
    auto xChunks = chunks_.find(chunkPosition.x);
    if (xChunks != chunks_.end())
    {
        auto yChunks = xChunks->second.find(chunkPosition.y);
        if (yChunks != xChunks->second.end())
        {
            auto zChunks = yChunks->second.find(chunkPosition.z);
            if (zChunks != yChunks->second.end())
            {
                return zChunks->second;
            }
        }
    }
    return nullptr;
}


void World::setBlock(glm::ivec3 position, Block block)
{
    //Get the chunk where the Blockk is
    Chunk *chunk = getChunk(position);
    //Translate position to inside the chunk
    position = getPositionInChunk(position);
    //Set the block
    if (chunk != nullptr)
        chunk->setBlock(position, block);
}

const Block &World::getBlock(glm::ivec3 position)
{
    //Get the chunk where the Blockk is
    Chunk *chunk = getChunk(position);
    //Translate position to inside the chunk
    position = getPositionInChunk(position);
    //return the block
    if (chunk != nullptr)
        return chunk->getBlock(position);
    else
        return Blocks::AIR_BLOCK;
}

static glm::vec3 checkCollision(CGE::Entities::Entity *entity, World *world)
{
    Hitbox entityHitbox = entity->getHitbox();
    glm::vec3 movement = entity->getSpeed();

    std::vector<Hitbox> blockHitboxes = world->getBlockHitboxes(entityHitbox.expand(1));

    for (Hitbox hitbox : blockHitboxes)
    {
        for (int axis = 0; axis < 3; ++axis)
            movement[axis] = hitbox.checkIfCollideInAxis(entityHitbox, axis, movement[axis]);
    }

    entity->setSpeed(movement);
    return entity->getOldPosition() + movement;
}


World::World()
        : player_(new Entities::Player(nullptr, camera_, {0, 60, 0})),
          collisionFunction_(std::bind(&checkCollision, std::placeholders::_1, this)),
          worldGenerator_(this, chunkManager_, cloudManager_),
          cloudManager_(player_, worldGenerator_),
          chunkManager_(player_, this, chunks_, worldGenerator_, cloudManager_)
{
    addEntity(std::shared_ptr<CGE::Entities::Entity>(player_));

/*
    Block *blocks = new Block[(int) pow(CHUNK_SIZE, 3)];
    for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; ++i)
        blocks[i] = Blocks::AIR_BLOCK;


    blocks[10 + CHUNK_SIZE + SQUARED_CHUNK_SIZE * 1] = {1, 0B00000000};

    blocks[2 + CHUNK_SIZE + SQUARED_CHUNK_SIZE * 1] = {1, 0B00001110};
    blocks[4 + CHUNK_SIZE + SQUARED_CHUNK_SIZE * 1] = {1, 0B00001101};
    blocks[6 + CHUNK_SIZE + SQUARED_CHUNK_SIZE * 1] = {1, 0B00001011};
    blocks[8 + CHUNK_SIZE + SQUARED_CHUNK_SIZE * 1] = {1, 0B00000111};

    blocks[2 + CHUNK_SIZE + SQUARED_CHUNK_SIZE * 3] = {1, 0B01000101};
    blocks[4 + CHUNK_SIZE + SQUARED_CHUNK_SIZE * 3] = {1, 0B01001010};
    blocks[6 + CHUNK_SIZE + SQUARED_CHUNK_SIZE * 3] = {1, 0B00010011};
    blocks[8 + CHUNK_SIZE + SQUARED_CHUNK_SIZE * 3] = {1, 0B00011100};

    blocks[2 + CHUNK_SIZE + SQUARED_CHUNK_SIZE * 5] = {1, 0B01010001};
    blocks[4 + CHUNK_SIZE + SQUARED_CHUNK_SIZE * 5] = {1, 0B01010010};
    blocks[6 + CHUNK_SIZE + SQUARED_CHUNK_SIZE * 5] = {1, 0B01010100};
    blocks[8 + CHUNK_SIZE + SQUARED_CHUNK_SIZE * 5] = {1, 0B01011000};

    blocks[2 + CHUNK_SIZE + SQUARED_CHUNK_SIZE * 7] = {1, 0B00001100};
    blocks[4 + CHUNK_SIZE + SQUARED_CHUNK_SIZE * 7] = {1, 0B00001010};
    blocks[6 + CHUNK_SIZE + SQUARED_CHUNK_SIZE * 7] = {1, 0B00000011};
    blocks[8 + CHUNK_SIZE + SQUARED_CHUNK_SIZE * 7] = {1, 0B00000101};

    blocks[2 + CHUNK_SIZE + SQUARED_CHUNK_SIZE * 9] = {1, 0B00010001};
    blocks[4 + CHUNK_SIZE + SQUARED_CHUNK_SIZE * 9] = {1, 0B00010010};
    blocks[6 + CHUNK_SIZE + SQUARED_CHUNK_SIZE * 9] = {1, 0B00010100};
    blocks[8 + CHUNK_SIZE + SQUARED_CHUNK_SIZE * 9] = {1, 0B00011000};

    blocks[2 + CHUNK_SIZE + SQUARED_CHUNK_SIZE * 11] = {1, 0B01000001};
    blocks[4 + CHUNK_SIZE + SQUARED_CHUNK_SIZE * 11] = {1, 0B01000010};
    blocks[6 + CHUNK_SIZE + SQUARED_CHUNK_SIZE * 11] = {1, 0B01000100};
    blocks[8 + CHUNK_SIZE + SQUARED_CHUNK_SIZE * 11] = {1, 0B01001000};

    blocks[2 + CHUNK_SIZE + SQUARED_CHUNK_SIZE * 13] = {1, 0B00000001};
    blocks[4 + CHUNK_SIZE + SQUARED_CHUNK_SIZE * 13] = {1, 0B00000010};
    blocks[6 + CHUNK_SIZE + SQUARED_CHUNK_SIZE * 13] = {1, 0B00000100};
    blocks[8 + CHUNK_SIZE + SQUARED_CHUNK_SIZE * 13] = {1, 0B00001000};

    //Slabs (midY = 1)
    blocks[10 + 4 * CHUNK_SIZE + SQUARED_CHUNK_SIZE] = {1, 0B00100000};

    blocks[2 + 4 * CHUNK_SIZE + SQUARED_CHUNK_SIZE] = {1, 0B00100001};
    blocks[4 + 4 * CHUNK_SIZE + SQUARED_CHUNK_SIZE] = {1, 0B00100010};
    blocks[6 + 4 * CHUNK_SIZE + SQUARED_CHUNK_SIZE] = {1, 0B00100100};
    blocks[8 + 4 * CHUNK_SIZE + SQUARED_CHUNK_SIZE] = {1, 0B00101000};

    blocks[2 + 4 * CHUNK_SIZE + 3 * SQUARED_CHUNK_SIZE] = {1, 0B00100011};
    blocks[4 + 4 * CHUNK_SIZE + 3 * SQUARED_CHUNK_SIZE] = {1, 0B00101010};
    blocks[6 + 4 * CHUNK_SIZE + 3 * SQUARED_CHUNK_SIZE] = {1, 0B00100101};
    blocks[8 + 4 * CHUNK_SIZE + 3 * SQUARED_CHUNK_SIZE] = {1, 0B00101100};

    blocks[2 + 4 * CHUNK_SIZE + 5 * SQUARED_CHUNK_SIZE] = {1, 0B00101110};
    blocks[4 + 4 * CHUNK_SIZE + 5 * SQUARED_CHUNK_SIZE] = {1, 0B00101101};
    blocks[6 + 4 * CHUNK_SIZE + 5 * SQUARED_CHUNK_SIZE] = {1, 0B00101011};
    blocks[8 + 4 * CHUNK_SIZE + 5 * SQUARED_CHUNK_SIZE] = {1, 0B00100111};

    //Up Slabs
    blocks[2 + 6 * CHUNK_SIZE + 1 * SQUARED_CHUNK_SIZE] = {1, 0B01110001};
    blocks[4 + 6 * CHUNK_SIZE + 1 * SQUARED_CHUNK_SIZE] = {1, 0B01110010};
    blocks[6 + 6 * CHUNK_SIZE + 1 * SQUARED_CHUNK_SIZE] = {1, 0B01110100};
    blocks[8 + 6 * CHUNK_SIZE + 1 * SQUARED_CHUNK_SIZE] = {1, 0B01111000};

    blocks[2 + 6 * CHUNK_SIZE + 3 * SQUARED_CHUNK_SIZE] = {1, 0B01110011};
    blocks[4 + 6 * CHUNK_SIZE + 3 * SQUARED_CHUNK_SIZE] = {1, 0B01111010};
    blocks[6 + 6 * CHUNK_SIZE + 3 * SQUARED_CHUNK_SIZE] = {1, 0B01110101};
    blocks[8 + 6 * CHUNK_SIZE + 3 * SQUARED_CHUNK_SIZE] = {1, 0B01111100};

    blocks[2 + 6 * CHUNK_SIZE + 5 * SQUARED_CHUNK_SIZE] = {1, 0B01111110};
    blocks[4 + 6 * CHUNK_SIZE + 5 * SQUARED_CHUNK_SIZE] = {1, 0B01111101};
    blocks[6 + 6 * CHUNK_SIZE + 5 * SQUARED_CHUNK_SIZE] = {1, 0B01111011};
    blocks[8 + 6 * CHUNK_SIZE + 5 * SQUARED_CHUNK_SIZE] = {1, 0B01110111};


    //int i = 0;
//
    //for (int x = 1; x < 14; ++x)
    //    for (int y = 1; y < 14; ++y)
    //        for (int z = 1; z < 14; ++z)
    //        {
    //            if (x % 2 == 1 && y % 2 == 1 && z % 2 == 1)
    //            {
    //                blocks[x + (y + z * 16) * 16] = {1, (unsigned char) i};
    //                ++i;
    //            }
    //        }


    glm::ivec3 chunkPosition = {0, 0, 0};
    Chunk *newChunk = new Chunk(blocks, this, chunkPosition, false);
    chunks_[chunkPosition.x][chunkPosition.y][chunkPosition.z] = newChunk;

*/

    auto display = CGE::IO::getWindow();
    shader.start();

    shader.loadMatrix(CGE::Shader::PROJECTION,
                      glm::perspectiveFov(45.0f, (float) display->getWidth(), (float) display->getHeight(), 0.1f,
                                          1000.0f)
    );
    shader.stop();

    chunkManager_.start();
}

void World::addEntity(std::shared_ptr<CGE::Entities::Entity> newEntity)
{
    entities_.push_back(newEntity);
    newEntity->setCollisionFunc(collisionFunction_);
}

std::vector<Hitbox> World::getBlockHitboxes(Hitbox area)
{
    std::vector<Hitbox> hitboxes;

    glm::ivec3 negLimit = glm::floor(area.getNegBorder());
    glm::ivec3 posLimit = glm::floor(area.getPosBorder());

    //Iterate through every blocks in the area
    for (int x = negLimit.x; x < posLimit.x; ++x)
        for (int y = negLimit.y; y < posLimit.y; ++y)
            for (int z = negLimit.z; z < posLimit.z; ++z)
            {
                //TODO: get the chunk then check inside the chunk (for better performance)
                //If it's not air, there is a block, so an hitbox
                if (getBlock({x, y, z}).ID != Blocks::AIR)
                    hitboxes.emplace_back((float) x, (float) x + 1, (float) y, (float) y + 1, (float) z, (float) z + 1);
            }

    return hitboxes;
}

glm::ivec3 World::getPickedBlock(float raySize)
{
    glm::vec3 rayPosition = camera_.position_;
    glm::vec3 rayOrientation = camera_.getRotationInNormalizedVector();
    glm::ivec3 blockPosition = glm::floor(rayPosition);



    //positive or negative direction in axis
    glm::bvec3 direction =
            {rayOrientation.x > 0.0f,
             rayOrientation.y > 0.0f,
             rayOrientation.z > 0.0f};

    while (true)
    {
#define X 0
#define Y 1
#define Z 2
        //Calculate the distance between the closest possible block from the ray position
        glm::vec3 delta;
        for (int axis = X; axis <= Z; ++axis)
        {
            delta[axis] = rayPosition[axis] - blockPosition[axis];
            if (direction[axis])
                delta[axis] = 1 - delta[axis];
        }

        //Calculate how many time the add ray orientation to get the next possible block
        glm::vec3 steps = delta / rayOrientation;
        glm::vec3 absSteps = glm::abs(steps);

        //Which is smaller
        //X
        if (absSteps.x < absSteps.y && absSteps.x < absSteps.z)
        {
            rayPosition += rayOrientation * (absSteps.x + (direction.x ? 0 : 0.001f));
            raySize -= (direction.x) ? 1 : -1 * absSteps.x;
        }
            //Y
        else if (absSteps.y < absSteps.z)
        {
            rayPosition += rayOrientation * (absSteps.y + (direction.y ? 0 : 0.0001f));
            raySize -= (direction.y) ? 1 : -1 * absSteps.y;
        }
            //Z
        else
        {
            rayPosition += rayOrientation * (absSteps.z + (direction.z ? 0 : 0.0001f));
            raySize -= absSteps.z;
        }

        //If there is no block
        if (raySize < 0.0f)
            return glm::ivec3(INT_MAX);


        blockPosition = glm::floor(rayPosition);

        if (getBlock(blockPosition) != Blocks::AIR_BLOCK)
            return blockPosition;

    }
}

glm::ivec3 World::getPositionInChunk(glm::ivec3 blockPosition)
{
    blockPosition %= (int) CHUNK_SIZE;

    for (int axis = 0; axis < 3; ++axis)
        if (blockPosition[axis] < 0)
            blockPosition[axis] += 16;

    return blockPosition;
}

glm::ivec3 World::getChunkPosition(glm::ivec3 blockPosition)
{
    glm::ivec3 chunkPosition = blockPosition / 16;

    for (int axis = 0; axis < 3; ++axis)
        chunkPosition[axis] -= blockPosition[axis] < 0 ? 1 : 0;

    return chunkPosition;
}

void World::addChunk(Chunk *newChunk)
{
    const glm::ivec3 &chunkPosition = newChunk->getChunkPosition();
    chunks_[chunkPosition.x]
    [chunkPosition.y]
    [chunkPosition.z] = newChunk;
}

World::~World()
{
    chunkManager_.stop();
}

void World::deleteChunk(Chunk *chunk)
{
    chunksToDelete_.push_back(chunk);
}

void World::deleteBufferedChunks()
{
    if (chunksToDelete_.empty())
        return;

    Chunk *chunk = chunksToDelete_.back();
    chunksToDelete_.pop_back();

    const glm::ivec3 &chunkPosition = chunk->getChunkPosition();
    auto xChunks = chunks_.find(chunkPosition.x);
    if (xChunks != chunks_.end())
    {
        auto yChunks = xChunks->second.find(chunkPosition.y);
        if (yChunks != xChunks->second.end())
        {
            auto zChunks = yChunks->second.find(chunkPosition.z);
            if (zChunks != yChunks->second.end())
            {
                delete chunk;
                yChunks->second.erase(chunkPosition.z);
            }
            else goto end;
            if (yChunks->second.empty())
                xChunks->second.erase(chunkPosition.y);
        }
        else goto end;
        if (xChunks->second.empty())
            chunks_.erase(chunkPosition.x);
    }

    end:
    if (!chunksToDelete_.empty())
        deleteBufferedChunks();
}
