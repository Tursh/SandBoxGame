/*
 * WorldGenerator.cpp
 *
 * Created by tursh on 9/28/19.
*/

#include <Utils/Log.h>
#include <World/Terrain/Bloc.h>
#include <World/Terrain/Chunk.h>
#include "World/Terrain/WorldGenerator.h"
#include <World/World.h>


WorldGenerator::WorldGenerator(World *world, ChunkManager &chunkManager)
        : world_(world), chunkManager_(chunkManager) {}


void WorldGenerator::run()
{
    while (running_)
    {
        glm::ivec3 chunkPosition = chunkManager_.getChunkToLoad();

        //If the chunk manager returns this position, it means, there no chunk to load
        if (chunkPosition == glm::ivec3(INT_MIN))
        {
            wait();
            continue;
        }

        //Create the bloc matrix and fill it with air
        Bloc *blocs = new Bloc[16 * 16 * 16];
        for (int k = 0; k < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; ++k)
            blocs[k] = {0, 0};


        for (int x = 0; x < CHUNK_SIZE; ++x)
            for (int z = 0; z < CHUNK_SIZE; ++z)
            {
                if (chunkPosition.y == 0)
                {
                    int y = pn.noise((double) (chunkPosition.x * CHUNK_SIZE + x) / (double) (CHUNK_SIZE),
                                     (double) (chunkPosition.z * CHUNK_SIZE + z) / (double) (CHUNK_SIZE), 0) *
                            CHUNK_SIZE;
                    for (; y >= 0; --y)
                    {
                        blocs[x + CHUNK_SIZE * (z + CHUNK_SIZE * y)] = {1, 0};
                    }
                } else if (chunkPosition.y < 0)
                {
                    for (int y = 0; y < CHUNK_SIZE; ++y)
                    {
                        blocs[x + CHUNK_SIZE * (z + CHUNK_SIZE * y)] = {1, 0};
                    }
                }

            }

        //TODO: Check why there are untracked chunks
        Chunk *newChunk = new Chunk(blocs, world_, chunkPosition);
        world_->addChunk(newChunk);
        newChunk->updateChunksAround();
    }
}