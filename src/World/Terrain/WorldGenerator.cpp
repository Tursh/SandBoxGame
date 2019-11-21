/*
 * WorldGenerator.cpp
 *
 * Created by tursh on 9/28/19.
*/

#include <Utils/Log.h>
#include <World/Terrain/Block.h>
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

        //Create the block matrix and fill it with air
        Block *blocks = new Block[16 * 16 * 16];
        for (int k = 0; k < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; ++k)
            blocks[k] = {0, 0};


        for (int x = 0; x < CHUNK_SIZE; ++x)
            for (int z = 0; z < CHUNK_SIZE; ++z)
            {
                if (chunkPosition.y >= 0)
                {
                    int groundLevel = pn.noise((double) (chunkPosition.x * CHUNK_SIZE + x) / (double) (CHUNK_SIZE * 4),
                                               (double) (chunkPosition.z * CHUNK_SIZE + z) / (double) (CHUNK_SIZE * 4),
                                               0) *
                                      CHUNK_SIZE * 6;
                    for (int y = std::min<int>(groundLevel - CHUNK_SIZE * chunkPosition.y, CHUNK_SIZE - 1); y >= 0; --y)
                    {
                        blocks[x + CHUNK_SIZE * (z + CHUNK_SIZE * y)] = {
                                (short) (y + chunkPosition.y * CHUNK_SIZE < groundLevel - 3 ? 2 : 1), 0};
                    }
                } else if (chunkPosition.y < 0)
                {
                    for (int y = 0; y < CHUNK_SIZE; ++y)
                    {
                        blocks[x + CHUNK_SIZE * (z + CHUNK_SIZE * y)] = {2, 0};
                    }
                }

            }

        Chunk *newChunk = new Chunk(blocks, world_, chunkPosition);
        world_->addChunk(newChunk);
        newChunk->updateChunksAround();
    }
}