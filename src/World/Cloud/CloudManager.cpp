/*
 * CloudManager.cpp
 *
 * Created by tursh on 3/17/20.
*/

#include <Utils/TimeUtils.h>
#include <World/Cloud/CloudManager.h>
#include <World/World.h>
#include <glm/ext/matrix_transform.hpp>
#include <IO/Window.h>
#include <glm/ext/matrix_clip_space.hpp>

const unsigned int CLOUD_CHUNK_HEIGHT = 8;

CloudManager::CloudManager(Entities::Player *player, WorldGenerator &worldGenerator)
        : player_(player), worldGenerator_(worldGenerator)
{
    unsigned int diameter = radius_ * 2 + 1;
    chunkCount_ = diameter * diameter;
    loaded_ = new bool[chunkCount_];

    auto display = CGE::IO::getWindow();
    shader_.start();

    shader_.loadMatrix(CGE::Shader::PROJECTION,
                      glm::perspectiveFov(45.0f, (float) display->getWidth(), (float) display->getHeight(), 0.1f,
                                          1000.0f)
    );
    shader_.stop();
}

CloudManager::~CloudManager()
{
    shader_.destroy();
}

void CloudManager::tick()
{
    if (CGE::Utils::TPSClock::shouldTick(2))
    {
        for(int i = 0; i < chunkCount_; ++i)
            loaded_[i] = false;
        unsigned int diameter = radius_ * 2 + 1;
        glm::ivec3 playerChunkPosition_ = World::getChunkPosition(player_->getPosition());
        glm::ivec2 centerChunkPosition_ = glm::ivec2(playerChunkPosition_.x, playerChunkPosition_.z);

        if (std::abs((int) CLOUD_CHUNK_HEIGHT - playerChunkPosition_.y) >= radius_)
        {
            if (!cloudChunks_.empty())
            {
                for (Chunk *chunk : cloudChunks_)
                    delete chunk;
                cloudChunks_.clear();
            }
            return;
        }

        for (int i = 0; i < cloudChunks_.size(); ++i)
        {
            //Get position relative to player
            glm::ivec3 chunkPosition = cloudChunks_[i]->getChunkPosition();
            glm::ivec2 relativeChunkPosition = glm::ivec2(chunkPosition.x, chunkPosition.z) - centerChunkPosition_;

            //Remove chunk if too far from player
            glm::ivec2 absDelta = glm::abs(relativeChunkPosition);
            for (int axis = 0; axis < 2; ++axis)
                if (absDelta[axis] > radius_ + 2)
                {
                    //delete cloudChunks_[i];
                    //cloudChunks_.erase(cloudChunks_.begin() + i);
                    --i;
                    continue;
                }

            if (absDelta.x <= radius_ && absDelta.y <= radius_)
            {
                relativeChunkPosition += radius_;
                int index = relativeChunkPosition.x + diameter * relativeChunkPosition.y;
                if (0 <= index && index < chunkCount_)
                    loaded_[index] = true;
            }
        }

        cloudChunksToLoad_.clear();


        for (int x = -radius_; x <= radius_; ++x)
            for (int y = -radius_; y <= radius_; ++y)
            {

                if (!loaded_[x + radius_ + (y + radius_) * diameter])
                    cloudChunksToLoad_.push_back(centerChunkPosition_ + glm::ivec2{x, y});
            }

        //If there are chunk to load notify the world generator
        if (!cloudChunksToLoad_.empty())
            worldGenerator_.notify();
    }
}

void CloudManager::render(CGE::View::Camera camera)
{
    shader_.start();

    shader_.loadMatrix(CGE::Shader::VIEW, camera.toViewMatrix());

    for (Chunk *chunk : cloudChunks_)
    {
        shader_.loadMatrix(CGE::Shader::TRANSFORMATION,
                          glm::translate(glm::mat4(1), (glm::vec3) chunk->getChunkPosition() *
                                                       (float) CHUNK_SIZE));
        chunk->render();
    }

    shader_.stop();
}

glm::ivec2 CloudManager::getChunkToLoad()
{
    if (cloudChunksToLoad_.empty())
        return glm::ivec3(INT_MIN);
    glm::ivec2 chunkPosition = cloudChunksToLoad_.back();
    cloudChunksToLoad_.pop_back();
    return chunkPosition;
}

void CloudManager::addChunk(Chunk *loadedChunk)
{
    cloudChunks_.push_back(loadedChunk);
}

void CloudManager::updateCloudChunkAround(const glm::ivec2 &cloudPosition)
{
    for (Chunk *chunk : cloudChunks_)
    {
        glm::ivec3 chunkPosition = chunk->getChunkPosition();
        glm::ivec2 absDelta = glm::abs(glm::ivec2(chunkPosition.x, chunkPosition.z) - cloudPosition);
        if ((absDelta.x == 1 && absDelta.y == 0) || (absDelta.x == 0 && absDelta.y == 1))
            chunk->update();
    }
}

