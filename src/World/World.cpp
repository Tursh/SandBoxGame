//
// Created by tursh on 4/30/19.
//

#include <glm/common.hpp>
#include <glm/gtx/string_cast.hpp>
#include <climits>
#include <IO/Input.h>
#include <Utils/Log.h>
#include <IO/Window.h>
#include <Loader/RessourceManager.h>
#include "World/World.h"

const unsigned int CHUNK_OFF_SET = UINT_MAX / 2;


void World::tick()
{
    //TODO create a world generator and check when the player move if chunks can be deleted


    player_.move(0.25f, this);
    camera_.followPlayer(&player_);

    if (CGE::IO::input::isKeyPressed(GLFW_KEY_E))
        setBloc(camera_.position_, {0, 0});
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
                if (chunk.second->isLoaded())
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
}


Chunk *World::getChunk(glm::ivec3 position)
{
    position = position / (int) CHUNK_SIZE;
    return getChunkByChunkPosition(position);
}

Chunk *World::getChunk(const glm::vec3 &position)
{
    return getChunk(glm::round(position));
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

    Bloc *blocs = new Bloc[16 * 16 * 16];
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
    position -= position / (int) CHUNK_SIZE;
    //Set the bloc
    if (chunk != nullptr)
        chunk->setBloc(position, bloc);
}

const Bloc &World::getBloc(glm::ivec3 position)
{
    //Get the chunk where the Block is
    Chunk *chunk = getChunk(position);
    //Translate position to inside the chunk
    position %= (int) CHUNK_SIZE;
    if(position.x < 0)
        position.x += 16;
    if(position.y < 0)
        position.y += 16;
    if(position.z < 0)
        position.z += 16;
    //return the bloc
    if (chunk != nullptr)
        return chunk->getBloc(position);
    else
        return {Blocs::AIR, 0};
}

World::World()
        : pn(1), player_(nullptr, {0, 50, 0})
{

    for (int i = -10; i <= 10; ++i)
        for (int j = -10; j <= 10; ++j)
        {
            Bloc *blocs = new Bloc[16 * 16 * 16];
            for (int k = 0; k < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; ++k)
                blocs[k] = {0, 0};
            for (int x = 0; x < CHUNK_SIZE; ++x)
                for (int z = 0; z < CHUNK_SIZE; ++z)
                {
                    int y = pn.noise((double) (i * CHUNK_SIZE + x) / (double) (CHUNK_SIZE),
                                     (double) (j * CHUNK_SIZE + z) / (double) (CHUNK_SIZE), 0) *
                            CHUNK_SIZE;
                    for (; y >= 0; --y)
                    {
                        blocs[x + CHUNK_SIZE * (z + CHUNK_SIZE * y)] = {1, 0};
                    }

                }
            glm::ivec3 chunkPosition = {i, 0, j};
            Chunk *newChunk = new Chunk(blocs, this, chunkPosition);
            chunks_[CHUNK_OFF_SET + chunkPosition.x][CHUNK_OFF_SET + chunkPosition.y][CHUNK_OFF_SET +
                                                                                      chunkPosition.z] = newChunk;
            newChunk->updateChunksAround();
        }

    auto display = CGE::IO::getWindow();
    shader.start();
    shader.loadMatrix(CGE::Shader::PROJECTION,
                      glm::perspectiveFov(45.0f, (float) display->getWidth(), (float) display->getHeight(), 0.1f, 1000.0f));
    shader.stop();
}
