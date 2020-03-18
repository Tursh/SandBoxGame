/*
 * CloudManager.h
 *
 * Created by tursh on 3/17/20.
*/

#pragma once


#include <World/Terrain/Chunk.h>
#include <World/Player.h>
#include <World/Terrain/WorldGenerator.h>
#include "CloudShader.h"

extern const unsigned int CLOUD_CHUNK_HEIGHT;

class CloudManager
{
    std::vector<Chunk *> cloudChunks_;

    std::vector<glm::ivec2> cloudChunksToLoad_;

    std::vector<unsigned int> chunkToDelete_;

    int radius_ = 20;

    unsigned int chunkCount_;

    bool *loaded_;

    CloudShader shader_;

    Entities::Player *player_;

    WorldGenerator& worldGenerator_;

    bool rendering = false;

public:

    CloudManager(Entities::Player *player, WorldGenerator& worldGenerator);

    ~CloudManager();

    void tick();

    void render(CGE::View::Camera camera);

    glm::ivec2 getChunkToLoad();

    void addChunk(Chunk *cloudChunk);

    void deleteChunk(unsigned int index);

    void updateCloudChunkAround(const glm::ivec2 &cloudPosition);

};


