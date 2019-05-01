//
// Created by tursh on 4/30/19.
//

#include <glm/common.hpp>
#include <glm/gtx/string_cast.hpp>
#include <climits>
#include <Utils/Log.h>
#include "World/World.h"

const unsigned int CHUNK_OFF_SET = UINT_MAX / 2;


void World::tick()
{
    //TODO create a world generator and check when the player move if chunks can be deleted
}

void World::render()
{
    for(auto &yChunks : chunks_)
        for(auto &zChunks : yChunks.second)
            for(auto & chunk : zChunks.second)
            {
                chunk.second.render();
            }
}


Chunk &World::getChunk(glm::ivec3 position)
{
    position = position / (int) CHUNK_SIZE + (int) CHUNK_OFF_SET;

    auto xChunks = chunks_.find(position.x);
    if (xChunks != chunks_.end())
    {
        auto yChunks = xChunks->second.find(position.y);
        if (yChunks != xChunks->second.end())
        {
            auto zChunks = yChunks->second.find(position.z);
            if (zChunks != yChunks->second.end())
            {
                return zChunks->second;
            }
        }
    }

    logWarning("The chunk at " << glm::to_string(position - (int) CHUNK_OFF_SET) << "don't exist");
}

Chunk &World::getChunk(const glm::vec3 &position)
{
    return getChunk(glm::round(position));
}

void World::setBloc(glm::ivec3 position, Bloc bloc)
{
    //Get the chunk where the Block is
    Chunk &chunk = getChunk(position);
    //Translate position to inside the chunk
    position -= position / (int) CHUNK_SIZE;
    //Set the bloc
    chunk.setBloc(position, bloc);
}

Bloc World::getBloc(glm::ivec3 position)
{
    //Get the chunk where the Block is
    Chunk &chunk = getChunk(position);
    //Translate position to inside the chunk
    position -= position / (int) CHUNK_SIZE;
    //return the bloc
    return chunk.getBloc(position);
}