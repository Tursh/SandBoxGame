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

const unsigned int CHUNK_OFF_SET = UINT_MAX / 2;


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
            for (auto &chunk : zChunks.second)
            {
                if (!chunk.second->isEmpty() && chunk.second->isLoaded())
                {
                    shader.loadMatrix(CGE::Shader::TRANSFORMATION,
                                      glm::translate(glm::mat4(1), (glm::vec3) chunk.second->getChunkPosition() *
                                                                   (float) CHUNK_SIZE));
                    chunk.second->render();
                }
            }
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    shader.stop();

    CGE::Text::textRenderer::renderText("FPS: " + std::to_string(CGE::Utils::getFPS()).substr(0, 4) + " TPS: " +
                                        std::to_string(CGE::Utils::TPSClock::getTPS()).substr(0, 4), 0.66f, 0.95f, 0.1f,
                                        glm::vec3(1, 1, 1),
                                        false);

    CGE::Text::textRenderer::renderText(glm::to_string(camera_.position_), -1, 0.95f, 0.1f, glm::vec3(1, 1, 1),
                                        false);

    CGE::Text::textRenderer::renderText("X", 0, 0, 0.2f, glm::vec3(1, 1, 1),
                                        false);

    CGE::Text::textRenderer::renderText(glm::to_string(camera_.getRotationInNormalizedVector()), -1, 0.90f, 0.1f,
                                        glm::vec3(1, 1, 1),
                                        false);
    CGE::Text::textRenderer::renderText(glm::to_string(getChunkPosition(camera_.position_)), -1, 0.85f, 0.1f,
                                        glm::vec3(1, 1, 1),
                                        false);
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
    chunkPosition += CHUNK_OFF_SET;
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


void World::setBloc(glm::ivec3 position, Bloc bloc)
{
    //Get the chunk where the Block is
    Chunk *chunk = getChunk(position);
    //Translate position to inside the chunk
    position = getPositionInChunk(position);
    //Set the bloc
    if (chunk != nullptr)
        chunk->setBloc(position, bloc);
}

const Bloc &World::getBloc(glm::ivec3 position)
{
    //Get the chunk where the Block is
    Chunk *chunk = getChunk(position);
    //Translate position to inside the chunk
    position = getPositionInChunk(position);
    //return the bloc
    if (chunk != nullptr)
        return chunk->getBloc(position);
    else
        return Blocs::AIR_BLOC;
}

static glm::vec3 checkCollision(CGE::Entities::Entity *entity, World *world)
{
    Hitbox entityHitbox = entity->getHitbox();
    glm::vec3 movement = entity->getSpeed();

    std::vector<Hitbox> blocHitboxes = world->getBlocHitboxs(entityHitbox.expand(1));

    for (Hitbox hitbox : blocHitboxes)
    {
        for (int axis = 0; axis < 3; ++axis)
            movement[axis] = hitbox.checkIfCollideInAxis(entityHitbox, axis, movement[axis]);
    }

    return entity->getOldPosition() + movement;
}


World::World()
        : player_(new Entities::Player(nullptr, camera_, {0, 15, 0})),
          collisionFunction_(std::bind(&checkCollision, std::placeholders::_1, this)),
          chunkManager_(player_, this, chunks_)
{
    addEntity(std::shared_ptr<CGE::Entities::Entity>(player_));

    Bloc *blocs = new Bloc[(int) pow(CHUNK_SIZE, 3)];
    for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; ++i)
        blocs[i] = {1, 0};
    glm::ivec3 chunkPosition = {0, -1, 0};
    Chunk *newChunk = new Chunk(blocs, this, chunkPosition);
    chunks_[CHUNK_OFF_SET + chunkPosition.x][CHUNK_OFF_SET + chunkPosition.y][CHUNK_OFF_SET +
                                                                              chunkPosition.z] = newChunk;

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

std::vector<Hitbox> World::getBlocHitboxs(Hitbox area)
{
    std::vector<Hitbox> hitboxes;

    glm::ivec3 negLimit = glm::floor(area.getNegBorder());
    glm::ivec3 posLimit = glm::floor(area.getPosBorder());

    //Iterate through every blocs in the area
    for (int x = negLimit.x; x < posLimit.x; ++x)
        for (int y = negLimit.y; y < posLimit.y; ++y)
            for (int z = negLimit.z; z < posLimit.z; ++z)
            {
                //TODO: get the chunk then check inside the chunk (for better performance)
                //If it's not air, there is a bloc, so an hitbox
                if (getBloc({x, y, z}).ID != Blocs::AIR)
                    hitboxes.push_back(
                            Hitbox((float) x, (float) x + 1, (float) y, (float) y + 1, (float) z, (float) z + 1));
            }

    return hitboxes;
}

glm::ivec3 World::getPickedBloc(float raySize)
{
    glm::vec3 rayPosition = camera_.position_;
    glm::vec3 rayOrientation = camera_.getRotationInNormalizedVector();
    glm::ivec3 blocPosition = glm::floor(rayPosition);

    //Up or Down
    bool sens[] = {rayOrientation.x > 0.0f,
                   rayOrientation.y > 0.0f,
                   rayOrientation.z > 0.0f};

    while (true)
    {
        //Calculate the distance between the closest possible bloc from the ray position
        glm::vec3 delta;
        for (int axis = 0; axis < 3; ++axis)
        {
            delta[axis] = rayPosition[axis] - blocPosition[axis];
            if (sens[axis])
                delta[axis] = 1 - delta[axis];
        }

        //Calculate how many time the add ray orientation to get the next possible bloc
        glm::vec3 steps = delta / rayOrientation;
        glm::vec3 absSteps = glm::abs(steps);

        //Which is smaller
        //X
        if (absSteps.x < absSteps.y && absSteps.x < absSteps.z)
        {
            rayPosition += rayOrientation * (absSteps.x + (sens[0] ? 0 : 0.0001f));
            raySize -= (sens[0]) ? 1 : -1 * absSteps.x;
        }
            //Y
        else if (absSteps.y < absSteps.z)
        {
            rayPosition += rayOrientation * (absSteps.y + (sens[1] ? 0 : 0.0001f));
            raySize -= (sens[1]) ? 1 : -1 * absSteps.y;
        }
            //Z
        else
        {
            rayPosition += rayOrientation * (absSteps.z + (sens[2] ? 0 : 0.0001f));
            raySize -= absSteps.z;
        }

        //If there is no bloc
        if (raySize < 0.0f)
            return glm::ivec3(INT_MAX);


        blocPosition = glm::floor(rayPosition);

        if (getBloc(blocPosition) != Blocs::AIR_BLOC)
            return blocPosition;

    }
}

glm::ivec3 World::getPositionInChunk(glm::ivec3 blocPosition)
{
    blocPosition %= (int) CHUNK_SIZE;
    if (blocPosition.x < 0)
        blocPosition.x += 16;
    if (blocPosition.y < 0)
        blocPosition.y += 16;
    if (blocPosition.z < 0)
        blocPosition.z += 16;

    return blocPosition;
}

glm::ivec3 World::getChunkPosition(glm::ivec3 blocPosition)
{
    glm::ivec3 chunkPosition = blocPosition / 16;

    for (int axis = 0; axis < 3; ++axis)
        chunkPosition[axis] -= blocPosition[axis] < 0 ? 1 : 0;

    return chunkPosition;
}

void World::addChunk(Chunk *newChunk)
{
    const glm::ivec3 &chunkPosition = newChunk->getChunkPosition();
    chunks_[CHUNK_OFF_SET + chunkPosition.x]
    [CHUNK_OFF_SET + chunkPosition.y]
    [CHUNK_OFF_SET + chunkPosition.z] = newChunk;
}

World::~World()
{
    chunkManager_.stop();
}

void World::deleteChunk(Chunk *chunk)
{
    if (rendering_)
    {
        chunksToDelete_.push_back(chunk);
    } else
    {
        if (!chunksToDelete_.empty())
            if (chunksToDelete_.back() == chunk)
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
                    yChunks->second.erase(chunkPosition.z);
                    delete chunk;
                } else goto end;
                if (yChunks->second.empty())
                    xChunks->second.erase(chunkPosition.y);
            } else goto end;
            if (xChunks->second.empty())
                chunks_.erase(chunkPosition.x);
        } else goto end;

        end:
        if (!chunksToDelete_.empty())
        {
            deleteChunk(chunksToDelete_.back());
        }

    }
}
