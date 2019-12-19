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


        Chunk *chunkUnder = world_->getChunkByChunkPosition(chunkPosition - glm::ivec3(0, 1, 0));


        for (int x = 0; x < CHUNK_SIZE; ++x)
            for (int z = 0; z < CHUNK_SIZE; ++z)
            {
                if (chunkPosition.y >= 0)
                {
                    glm::ivec3 topBloc = {x, CHUNK_SIZE - 1, z};
                    if (chunkUnder != nullptr && chunkUnder->getBlock(topBloc).ID == Blocks::AIR)
                        continue;

                    glm::ivec3 blocPositionInWorld = {chunkPosition.x * CHUNK_SIZE + x, 0,
                                                      chunkPosition.z * CHUNK_SIZE + z};

                    double groundLevel[4] =
                            {
                                    pn.noise((double) blocPositionInWorld.x / (double) (CHUNK_SIZE * 4),
                                             (double) blocPositionInWorld.z / (double) (CHUNK_SIZE * 4), 0) *
                                    CHUNK_SIZE * 6,
                                    pn.noise((double) blocPositionInWorld.x / (double) (CHUNK_SIZE * 4),
                                             (double) (blocPositionInWorld.z + 1) / (double) (CHUNK_SIZE * 4), 0) *
                                    CHUNK_SIZE * 6,
                                    pn.noise((double) (blocPositionInWorld.x + 1) / (double) (CHUNK_SIZE * 4),
                                             (double) blocPositionInWorld.z / (double) (CHUNK_SIZE * 4), 0) *
                                    CHUNK_SIZE * 6,
                                    pn.noise((double) (blocPositionInWorld.x + 1) / (double) (CHUNK_SIZE * 4),
                                             (double) (blocPositionInWorld.z + 1) / (double) (CHUNK_SIZE * 4), 0) *
                                    CHUNK_SIZE * 6,
                            };

                    //Get the higher ground level of the 4 corners
                    double higher = -1000000;
                    for (double &level : groundLevel)
                    {
                        level -= CHUNK_SIZE * chunkPosition.y;
                        if (level > higher)
                            higher = level;
                    }

                    //If ground level is under the chunk then there is no block to create
                    if (higher < 0)
                        continue;

                    //Top block y position in chunk
                    int higherBloc = (int) higher;


                    for (int y = std::min<int>(higherBloc, CHUNK_SIZE - 1); y >= 0; --y)
                    {
                        int cornerDownCount = 0, underBlockCorner = -1;

                        if (y >= higherBloc - 2)
                        {
                            unsigned char state = 0;

                            for (int corner = 0; corner < 4; ++corner)
                            {
                                bool cornerFlag = groundLevel[corner] - y < 0.0f;
                                if (cornerFlag)
                                {
                                    ++cornerDownCount;
                                    if (groundLevel[corner] - y < -Blocks::CUBE_SIZE)
                                        underBlockCorner = corner;
                                }
                                //Set the corner flag
                                state += (unsigned char) pow(2, corner) * cornerFlag;
                            }

                            //If the block under in another chunk has more than 1 corner down, no block.
                            if (y == 0 && state && chunkUnder != nullptr && chunkUnder->getBlock({x, 15, z}).state)
                            {

                                unsigned char underChunkBlockState = chunkUnder->getBlock({x, 15, z}).state;

                                int underChunkBlockCornerDownCount = 0;

                                for (int corner = 0; corner < 4; ++corner)
                                    underChunkBlockCornerDownCount += (underChunkBlockState >> corner) & 1;

                                if (underChunkBlockCornerDownCount > 1)
                                    state = 0B00001111;
                            }

                            //If the top block is stated and this block has more than 1 corner down, remove it
                            if (cornerDownCount > 1 && y < 15)
                                blocks[x + CHUNK_SIZE * ((y + 1) + CHUNK_SIZE * z)] = Blocks::AIR_BLOC;

                            //If the top block on the other chunk is stated while this one has more than 1 corner down, remove it.
                            if (y == 15 && cornerDownCount > 1)
                            {
                                Chunk *topChunk = world_->getChunkByChunkPosition(chunkPosition + glm::ivec3(0, 1, 0));
                                if (topChunk->getBlock({x, 0, z}).state)
                                {
                                    topChunk->setBlock({x, 0, z}, Blocks::AIR_BLOC);
                                }
                            }

                            //
                            if (underBlockCorner != -1)
                            {
                                if (!(state >> (underBlockCorner ^ 1) & 1))
                                    state += (unsigned char) pow(2, (underBlockCorner ^ 1));

                                if (!(state >> (underBlockCorner ^ 2) & 1))
                                    state += (unsigned char) pow(2, (underBlockCorner ^ 2));
                            }

                            //Set the block
                            blocks[x + CHUNK_SIZE * (y + CHUNK_SIZE * z)] = {1, state};

                        }
                        else
                        {
                            //Set grass block or stone block
                            blocks[x + CHUNK_SIZE * (y + CHUNK_SIZE * z)] =
                                    {(short) (y + chunkPosition.y * CHUNK_SIZE < higher - 3 ? 2 : 1), 0};
                        }
                    }
                }
                else if (chunkPosition.y < 0)
                {
                    //Set all stone blocks
                    for (int y = 0; y < CHUNK_SIZE; ++y)
                    {
                        blocks[x + CHUNK_SIZE * (y + CHUNK_SIZE * z)] = {2, 0};
                    }
                }

            }


        auto *newChunk = new Chunk(blocks, world_, chunkPosition, false);
        world_->addChunk(newChunk);
        newChunk->updateChunksAround();
    }
}