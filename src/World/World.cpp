//
// Created by tursh on 4/30/19.
//

#include <glm/common.hpp>
#include <glm/gtx/string_cast.hpp>
#include <climits>
#include <Utils/Log.h>
#include <IO/Display.h>
#include <IO/Input.h>
#include <Loader/RessourceManager.h>
#include "World/World.h"

const unsigned int CHUNK_OFF_SET = UINT_MAX / 2;


void World::tick()
{
    //TODO create a world generator and check when the player move if chunks can be deleted
    glm::vec2 mouse = CGE::IO::input::getCursorShifting();

    camera_.rotation_.y += mouse.x;
    camera_.rotation_.x -= mouse.y;
}

std::shared_ptr<CGE::Loader::TexturedModel> texModel;
glm::mat4 matrix(1);

void World::render()
{
    shader.start();
    shader.loadMatrix(CGE::Shader::VIEW, camera_.toViewMatrix());
    matrix = glm::translate(matrix, {0, 0, -0.01f});
    shader.loadMatrix(CGE::Shader::TRANSFORMATION, matrix);

    for (auto &yChunks : chunks_)
        for (auto &zChunks : yChunks.second)
            for (auto &chunk : zChunks.second)
            {
                chunk.second->render();
            }
    texModel->render();
    shader.stop();
}


Chunk *World::getChunk(glm::ivec3 position)
{
    position = position / (int) CHUNK_SIZE + (int) CHUNK_OFF_SET;
    return getChunkByChunkPosition(position);
}

Chunk *World::getChunk(const glm::vec3 &position)
{
    return getChunk(glm::round(position));
}

Chunk **World::getAroundChunk(glm::ivec3 chunkPosition)
{
    auto **chunkList = new Chunk *[6];

    chunkList[0] = getChunkByChunkPosition(chunkPosition + glm::ivec3(1, 0, 0));
    chunkList[1] = getChunkByChunkPosition(chunkPosition + glm::ivec3(-1, 0, 0));
    chunkList[2] = getChunkByChunkPosition(chunkPosition + glm::ivec3(0, 1, 0));
    chunkList[3] = getChunkByChunkPosition(chunkPosition + glm::ivec3(0, -1, 0));
    chunkList[4] = getChunkByChunkPosition(chunkPosition + glm::ivec3(0, 0, 1));
    chunkList[5] = getChunkByChunkPosition(chunkPosition + glm::ivec3(0, 0, -1));

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


void World::setBloc(glm::ivec3 position, Bloc bloc)
{
    //Get the chunk where the Block is
    Chunk *chunk = getChunk(position);
    //Translate position to inside the chunk
    position -= position / (int) CHUNK_SIZE;
    //Set the bloc
    chunk->setBloc(position, bloc);
}

const Bloc &World::getBloc(glm::ivec3 position)
{
    //Get the chunk where the Block is
    Chunk *chunk = getChunk(position);
    //Translate position to inside the chunk
    position -= position / (int) CHUNK_SIZE;
    //return the bloc
    return chunk->getBloc(position);
}

World::World()
{
    texModel = CGE::Loader::resManager::getTexModel(1);
    Bloc *blocs = new Bloc[16 * 16 * 16];
    for(int i = 0; i < 16 * 16 * 16; ++i)
        blocs[i] = {1, 0};
    glm::ivec3 chunkPosition = {0, 0, 1};
    chunks_[0][0][1] = new Chunk(blocs, this, chunkPosition);
    auto display = CGE::IO::getDisplay();
    shader.start();
    shader.loadMatrix(CGE::Shader::PROJECTION, glm::perspectiveFov(45.0f, (float)display->width, (float)display->height, 0.1f, 100.0f));
    shader.stop();
}