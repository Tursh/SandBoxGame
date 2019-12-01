/*
 * ChunkManager.cpp
 *
 * Created by tursh on 9/27/19.
*/

#include <Utils/TimeUtils.h>
#include <Utils/Log.h>
#include "World/Terrain/ChunkManager.h"
#include "World/World.h"
#include <glm/gtx/string_cast.hpp>
#include <Loader/RessourceManager.h>
#include <IO/Window.h>

std::shared_ptr<CGE::Loader::Texture> tex;

ChunkManager::ChunkManager(Entities::Player *player,
                           World *world,
                           std::map<int, std::map<int, std::map<int, Chunk *>>> &chunks)
        : player_(player), chunks_(chunks), worldGenerator_(world, *this), world_(world)
{
    tex = CGE::Loader::resManager::getTexture(1);
    CGE::Utils::TPSClock::init(2);
    CGE::Utils::TPSClock::setTPS(2, 2);

    int diameter = radius_ * 2 + 1;
    chunkCount_ = (int) pow(diameter, 3);
    loaded = new bool[chunkCount_];

    worldGenerator_.start();
}

ChunkManager::~ChunkManager()
{
    delete[] loaded;
    worldGenerator_.stop();
}

void ChunkManager::tick()
{
    if (CGE::Utils::TPSClock::shouldTick(2))
    {
        glm::ivec3 playerChunkPosition = World::getChunkPosition(player_->getPosition());

        if (centerChunkPosition_ == playerChunkPosition)
            return;

        centerChunkPosition_ = playerChunkPosition;

        //Create loaded array
        int diameter = radius_ * 2 + 1;
        for (int i = 0; i < chunkCount_; ++i)
            loaded[i] = false;

        auto chunks = chunks_;

        for (auto &chunkMapMap : chunks)
            for (auto &chunkMap : chunkMapMap.second)
                for (auto chunkPair : chunkMap.second)
                {
                    //Get position relative to player
                    Chunk *chunk = chunkPair.second;
                    if (chunk == nullptr)
                        continue;
                    glm::ivec3 relativeChunkPosition = chunk->getChunkPosition() - centerChunkPosition_;

                    //Remove chunk if too far from player
                    glm::ivec3 absDelta = glm::abs(relativeChunkPosition);
                    for (int axis = 0; axis < 3; ++axis)
                        if (absDelta[axis] > radius_ + 2)
                        {
                            world_->deleteChunk(chunk);
                            continue;
                        }


                    if (absDelta.x <= radius_ && absDelta.y <= radius_ && absDelta.z <= radius_)
                    {
                        relativeChunkPosition += radius_;
                        int index = relativeChunkPosition.x +
                                    diameter * (relativeChunkPosition.y + diameter * relativeChunkPosition.z);
                        if (0 <= index && index < chunkCount_)
                            loaded[index] = true;
                    }
                }


        chunkToLoad_.clear();

        for (int i = radius_; i >= 0; --i)
            for (int axis = 0; axis < 3; ++axis)
                for (int x = -i + (axis > 1); x <= i - (axis > 1); ++x)
                    for (int y = -i + (axis > 0); y <= i - (axis > 0); ++y)
                    {
                        glm::ivec3 relativeChunkPosition(i);
                        relativeChunkPosition[(axis + 1) % 3] = x;
                        relativeChunkPosition[(axis + 2) % 3] = y;

                        glm::ivec3 indexPosition = relativeChunkPosition + radius_;

                        if (!loaded[indexPosition.x + diameter * (indexPosition.y + diameter * indexPosition.z)])
                            chunkToLoad_.push_back(centerChunkPosition_ + relativeChunkPosition);
                        if (relativeChunkPosition[axis] != 0)
                        {
                            relativeChunkPosition[axis] *= -1;
                            indexPosition = relativeChunkPosition + radius_;
                            if (!loaded[indexPosition.x +
                                        diameter * (indexPosition.y +
                                                    diameter * indexPosition.z)])
                                chunkToLoad_.push_back(centerChunkPosition_ + relativeChunkPosition);
                        }

                    }

        //If there are chunk to load notify the world generator
        if (!chunkToLoad_.empty())
            worldGenerator_.notify();

    }
}

void ChunkManager::setRadius(int radius)
{
    radius_ = radius;
}

glm::vec3 ChunkManager::getChunkToLoad()
{
    if (chunkToLoad_.empty())
        return glm::ivec3(INT_MIN);
    glm::ivec3 chunkPosition = chunkToLoad_.back();
    chunkToLoad_.pop_back();
    return chunkPosition;
}

void ChunkManager::run()
{
    while (running_)
    {
        tick();
    }
}
