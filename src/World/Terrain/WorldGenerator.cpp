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

const unsigned int MAX_WORLD_GENERATION_HEIGHT = 256;

WorldGenerator::WorldGenerator(World *world, ChunkManager &chunkManager)
        : world_(world), chunkManager_(chunkManager) {}

static void fillChunk(Block *blocks, Block block)
{
    for (int i = 0; i < CUBED_CHUNK_SIZE; ++i)
        blocks[i] = block;
}

const unsigned int MID_X = 1 << 6, MID_Y = 1 << 5, MID_Z = 1 << 4, INV = 1 << 7;

static unsigned char generateState(float *cornerGroundLevels, int &cornerDownCount, int &underBlockCorner, int &y)
{
    unsigned char state = 0;
    int sections[6] = {0, 0, 0, 0, 0, 0};
    int cornerSections[4] = {0, 0, 0, 0};

    float average = 0;

    for (int corner = 0; corner < 4; ++corner)
    {
        auto relativeLevel = (float)(cornerGroundLevels[corner] - y);

        average += relativeLevel;

        int section = (int) std::floor(relativeLevel / 0.25) + 1;
        ++sections[section < 0 ? 0 : (section > 5 ? 5 : section)];

        cornerSections[corner] = section;
    }

    //Set the midY flag
    if (sections[2] + sections[3] >= 1 && !((sections[4] + sections[5]) && (sections[1] + sections[0])))
    {
        average /= 4;
        state += MID_Y;
        if (average < 0.5f)
        {
            for (int corner = 0; corner < 4; ++corner)
                if (cornerSections[corner] < 2)
                {
                    state += 1 << corner;
                    ++cornerDownCount;
                }
        }
        else
        {
            state += MID_X + MID_Z;
            for (int corner = 0; corner < 4; ++corner)
                if (cornerSections[corner] < 4)
                {
                    state += 1 << corner;
                    ++cornerDownCount;
                }
        }
    }
    else
    {
        for (int corner = 0; corner < 4; ++corner)
            if (cornerSections[corner] < 4)
            {
                state += 1 << corner;
                ++cornerDownCount;
            }
    }

    return state;
}

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

        //Create the block 3D matrix and fill it with air
        Block *blocks = new Block[CUBED_CHUNK_SIZE];
        fillChunk(blocks, Blocks::AIR_BLOC);

        //If the chunk is under the world generation range, it is going to be only stone blocs
        if (chunkPosition.y < 0)
        {
            //Set all stone blocks
            fillChunk(blocks, {2, 0});
        }
            //If the chunk is higher than the world generation range, it is going to be only air blocs
        else if (chunkPosition.y > MAX_WORLD_GENERATION_HEIGHT / CHUNK_SIZE); //Noting to do, chunk is already air blocs
        else
        {
            //Get all ground level in chunk and chunk border blocs
            float groundLevel[19 * 19];
            float higher = 0, lower = 256;

            {
                int xEndPosition = (chunkPosition.x + 1) * CHUNK_SIZE + 2,
                        zEndPosition = (chunkPosition.z + 1) * CHUNK_SIZE + 2,
                        i = 0;

                for (int x = chunkPosition.x * CHUNK_SIZE - 1; x < xEndPosition; ++x)
                    for (int z = chunkPosition.z * CHUNK_SIZE - 1; z < zEndPosition; ++z)
                    {
                        groundLevel[i] = (float)pn.noise(x / (double) (CHUNK_SIZE * 4), z / (double) (CHUNK_SIZE * 4), 0) *
                                         CHUNK_SIZE * 4;
                        higher = std::max(higher, groundLevel[i]);
                        lower = std::min(lower, groundLevel[i]);
                        ++i;
                    }
            }

            if (higher / CHUNK_SIZE < chunkPosition.y)
                goto end; //If there is no ground high enough to be in the chunk, then the chunk is empty

            if (lower / CHUNK_SIZE - 1 >= chunkPosition.y)
            {
                fillChunk(blocks, {2, 0});
                goto end; // If there is no ground lower enough to have
            }

            //Make ground level relative to the chunk
            for (unsigned int i = 0; i < 19 * 19; ++i)
                groundLevel[i] -= chunkPosition.y * CHUNK_SIZE;

            float averageGroundLevels[17 * 17];

            for (int xa = 0; xa < 17; ++xa)
                for (int za = 0; za < 17; ++za)
                {
                    float &averageGroundLevel = averageGroundLevels[xa * 17 + za] = 0;
                    for (int xb = 0; xb < 3; ++xb)
                        for (int zb = 0; zb < 3; ++zb)
                        {
                            averageGroundLevel += groundLevel[(xa + xb) * 19 + (za + zb)];
                        }

                    averageGroundLevel /= 9;
                }

            for (int x = 0; x < CHUNK_SIZE; ++x)
                for (int z = 0; z < CHUNK_SIZE; ++z)
                {
                    float cornerGroundLevels[4] =
                            {
                                    averageGroundLevels[x * 17 + z],
                                    averageGroundLevels[x * 17 + z + 1],
                                    averageGroundLevels[(x + 1) * 17 + z],
                                    averageGroundLevels[(x + 1) * 17 + z + 1]
                            };

                    float blockGroundLevel = 0;

                    for (int corner = 0; corner < 4; ++corner)
                        blockGroundLevel = std::max(blockGroundLevel, cornerGroundLevels[corner]);

                    for (int y = std::min<int>(blockGroundLevel, CHUNK_SIZE - 1); y >= 0; --y)
                    {
                        if (y >= blockGroundLevel - 2)
                        {
                            if(chunkPosition * 16 + glm::ivec3(x, -chunkPosition.y * 16, z) == glm::ivec3{30,0, 19})
                                logInfo("");
                            int cornerDownCount = 0, underBlockCorner = -1;
                            unsigned char state = generateState(cornerGroundLevels, cornerDownCount, underBlockCorner,
                                                                y);

                            if (cornerDownCount == 4)
                            {
                                if (state & MID_Y)
                                    state = MID_Y;
                                else
                                {
                                    blocks[x + CHUNK_SIZE * (y + CHUNK_SIZE * z)] = Blocks::AIR_BLOC;
                                    continue;
                                }
                            }


                            //If the block under in another chunk has more than 1 corner down, no block.
                            if (y == 0 && state && underBlockCorner == -1)
                            {
                                glm::ivec3 underChunkPosition = chunkPosition;
                                --underChunkPosition.y;

                                Chunk *underChunk = world_->getChunkByChunkPosition(
                                        underChunkPosition);
                                if (underChunk != nullptr)
                                {
                                    unsigned char underChunkBlockState = underChunk->getBlock({x, 15, z}).state;

                                    if (underChunkBlockState)
                                    {
                                        int underChunkBlockCornerDownCount = 0;

                                        for (int corner = 0; corner < 4; ++corner)
                                            underChunkBlockCornerDownCount += (underChunkBlockState >> corner) & 1;

                                        if (underChunkBlockCornerDownCount > 1)
                                            state = 0B00001111;
                                    }
                                }
                            }

                            //If the top block is stated and this block has more than 1 corner down, remove it
                            if (cornerDownCount > 1 && y < CHUNK_SIZE - 1)
                                blocks[x + CHUNK_SIZE * ((y + 1) + CHUNK_SIZE * z)] = Blocks::AIR_BLOC;

                            //If the top block on the other chunk is stated while this one has more than 1 corner down, remove it.
                            if (y == 15 && cornerDownCount > 1)
                            {
                                Chunk *topChunk = world_->getChunkByChunkPosition(
                                        chunkPosition + glm::ivec3(0, 1, 0));
                                if (topChunk != nullptr && topChunk->getBlock({x, 0, z}).state)
                                {
                                    topChunk->setBlock({x, 0, z}, Blocks::AIR_BLOC);
                                }
                            }

                            //
                            if (underBlockCorner != -1)
                            {
                                if (!(state >> (underBlockCorner ^ 1) & 1))
                                    state += (unsigned char) 1 << (underBlockCorner ^ 1);

                                if (!(state >> (underBlockCorner ^ 2) & 1))
                                    state += (unsigned char) 1 << (underBlockCorner ^ 2);
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
        }

        end:

        auto *newChunk = new Chunk(blocks, world_, chunkPosition, false);
        world_->addChunk(newChunk);
        newChunk->updateChunksAround();

    }

}
