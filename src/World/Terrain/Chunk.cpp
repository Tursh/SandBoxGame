//
// Created by tursh on 4/30/19.
//

#include <Utils/Log.h>
#include "World/Terrain/Chunk.h"
#include <World/World.h>
#include <Loader/RessourceManager.h>
#include <glm/gtx/string_cast.hpp>
#include <Loader/Meshes/MeshBuilder.h>

const int CHUNK_SIZE = 16, SQUARED_CHUNK_SIZE = CHUNK_SIZE * CHUNK_SIZE, CUBED_CHUNK_SIZE =
        SQUARED_CHUNK_SIZE * CHUNK_SIZE;

// For a lack of pow function for integers
static int pow(int base, int power)
{
    int ans = 1;
    for (int i = 0; i < power; ++i)
        ans *= base;
    return ans;
}

static std::array<const Block *, 6>
getBlockNeighbors(const glm::ivec3 &blockPosition, const Block *block, Chunk **neighborChunks)
{
    std::array<const Block *, 6> neighbors{};

    for (int axis = 0; axis < 3; ++axis)
    {
        //-
        if (blockPosition[axis] == 0)
        {
            if (neighborChunks[axis * 2] != nullptr)
            {
                glm::ivec3 neighborsPosition = blockPosition;
                neighborsPosition[axis] = CHUNK_SIZE - 1;
                neighbors[axis * 2] = &(neighborChunks[axis * 2]->getBlock(neighborsPosition));
            }
        }
        else
            neighbors[axis * 2] = block - pow(CHUNK_SIZE, axis);
        //+
        if (blockPosition[axis] == CHUNK_SIZE - 1)
        {
            if (neighborChunks[axis * 2 + 1] != nullptr)
            {
                glm::ivec3 neighborsPosition = blockPosition;
                neighborsPosition[axis] = 0;
                neighbors[axis * 2 + 1] = &(neighborChunks[axis * 2 + 1]->getBlock(neighborsPosition));
            }
        }
        else
            neighbors[axis * 2 + 1] = block + pow(CHUNK_SIZE, axis);
    }

    return neighbors;
}

static bool isBlockObstructed(const std::array<const Block *, 6> &neighbors)
{
    bool isObstructed = true;

    for (int i = 0; i < 6; ++i)
        if (neighbors[i] == nullptr || !neighbors[i]->ID || neighbors[i]->state)
        {
            isObstructed = false;
            break;
        }

    return isObstructed;
}

void Chunk::loadToTexMesh()
{
    if (empty_)
        return;

    //First get the six around this one
    Chunk **neighborChunks = world_->getAroundChunk(chunkPosition_);

    CGE::Loader::MeshBuilder meshBuilder;

    for (int x = 0; x < CHUNK_SIZE; ++x)
        for (int y = 0; y < CHUNK_SIZE; ++y)
            for (int z = 0; z < CHUNK_SIZE; ++z)
            {
                glm::ivec3 blockPosition = {x, y, z};
                Block *currentBlock =
                        blocks_ + blockPosition.x + CHUNK_SIZE * (blockPosition.y + CHUNK_SIZE * blockPosition.z);

                //Get block neighbors
                std::array<const Block *, 6> neighbors = getBlockNeighbors(blockPosition, currentBlock, neighborChunks);

                //If it's a block of air or the block is obstructed there is nothing to load
                if (currentBlock->ID != Blocks::AIR && !isBlockObstructed(neighbors))
                {
                    const short *texAtlas = Blocks::TEX_ATLAS[currentBlock->ID];
                    glm::vec4 top = texture_->getTextureLimits(texAtlas[0]),
                            side = texture_->getTextureLimits(texAtlas[1]),
                            bottom = texture_->getTextureLimits(texAtlas[2]);

                    Blocks::loadBlock(meshBuilder, blockPosition, currentBlock, neighbors.data(), top, side, bottom);
                }
            }

    delete[] neighborChunks;

    if (!meshBuilder.isValid())
    {
        empty_ = true;
        return;
    }
    else
        empty_ = false;

    meshBuilder.loadToSharedMesh(mesh_);
}


Chunk::Chunk(Block *blocks, World *world, glm::ivec3 &chunkPosition, bool empty)
        : TexturedMesh(nullptr, CGE::Loader::resManager::getTexture(1), true),
          blocks_(blocks), world_(world), chunkPosition_(chunkPosition), empty_(empty)
{
    loadToTexMesh();
}

Chunk::~Chunk()
{
    unload();
    delete[] blocks_;
}

void Chunk::update()
{
    //Reload the Blocks to the textured model
    loadToTexMesh();
}

void Chunk::updateChunksAround()
{
    auto neighborChunks = world_->getAroundChunk(chunkPosition_);
    for (int i = 0; i < 6; ++i)
    {
        if (neighborChunks[i] != nullptr)
            neighborChunks[i]->update();
    }
    delete[] neighborChunks;
}

void Chunk::setBlock(const glm::ivec3 &position, const Block &newBlock)
{
#ifndef NDEBUG
    if (0 > position.x || position.x > CHUNK_SIZE)
    logError(
            "The block you are trying to get is not in this chunk. Axis: x")

    if (0 > position.y || position.y > CHUNK_SIZE)
    logError(
            "The block you are trying to get is not in this chunk. Axis: y")

    if (0 > position.z || position.z > CHUNK_SIZE)
    logError(
            "The block you are trying to get is not in this chunk. Axis: z")
#endif

    Block &currentBlock = blocks_[position.x + CHUNK_SIZE * (position.y + CHUNK_SIZE * position.z)];

    if (currentBlock == newBlock)
        return;

    if (empty_)
        empty_ = false;

    //Set block in chunk
    currentBlock = newBlock;

    if (position.x == 0)
    {
        Chunk *neighbourChunk = world_->getChunkByChunkPosition(
                glm::ivec3(chunkPosition_.x - 1, chunkPosition_.y, chunkPosition_.z));
        if (neighbourChunk != nullptr)
        {
            neighbourChunk->empty_ = false;
            neighbourChunk->loadToTexMesh();
        }
    }
    else if (position.x == CHUNK_SIZE - 1)
    {
        Chunk *neighbourChunk = world_->getChunkByChunkPosition(
                glm::ivec3(chunkPosition_.x + 1, chunkPosition_.y, chunkPosition_.z));
        if (neighbourChunk != nullptr)
        {
            neighbourChunk->empty_ = false;
            neighbourChunk->loadToTexMesh();
        }
    }
    if (position.y == 0)
    {
        Chunk *neighbourChunk = world_->getChunkByChunkPosition(
                glm::ivec3(chunkPosition_.x, chunkPosition_.y - 1, chunkPosition_.z));
        if (neighbourChunk != nullptr)
        {
            neighbourChunk->empty_ = false;
            neighbourChunk->loadToTexMesh();
        }
    }
    else if (position.y == CHUNK_SIZE - 1)
    {
        Chunk *neighbourChunk = world_->getChunkByChunkPosition(
                glm::ivec3(chunkPosition_.x, chunkPosition_.y + 1, chunkPosition_.z));
        if (neighbourChunk != nullptr)
        {
            neighbourChunk->empty_ = false;
            neighbourChunk->loadToTexMesh();
        }
    }

    if (position.z == 0)
    {
        Chunk *neighbourChunk = world_->getChunkByChunkPosition(
                glm::ivec3(chunkPosition_.x, chunkPosition_.y, chunkPosition_.z - 1));
        if (neighbourChunk != nullptr)
        {
            neighbourChunk->empty_ = false;
            neighbourChunk->loadToTexMesh();
        }
    }
    else if (position.z == CHUNK_SIZE - 1)
    {
        Chunk *neighbourChunk = world_->getChunkByChunkPosition(
                glm::ivec3(chunkPosition_.x, chunkPosition_.y, chunkPosition_.z + 1));
        if (neighbourChunk != nullptr)
        {
            neighbourChunk->empty_ = false;
            neighbourChunk->loadToTexMesh();
        }
    }


    update();

    // If the new block is air than check if it is at the extremities
    // of the chunk and update the chunks that are touching that block.
    if (newBlock.ID == Blocks::AIR)
    {
        if (position.x == 0)
        {
            glm::ivec3 chunkPosition = chunkPosition_;
            --chunkPosition.x;
            Chunk *neighborChunk = world_->getChunkByChunkPosition(chunkPosition);
            if (neighborChunk != nullptr) neighborChunk->update();
        }
        else if (position.x == CHUNK_SIZE - 1)
        {
            glm::ivec3 chunkPosition = chunkPosition_;
            ++chunkPosition.x;
            Chunk *neighborChunk = world_->getChunkByChunkPosition(chunkPosition);
            if (neighborChunk != nullptr) neighborChunk->update();
        }
        if (position.y == 0)
        {
            glm::ivec3 chunkPosition = chunkPosition_;
            --chunkPosition.y;
            Chunk *neighborChunk = world_->getChunkByChunkPosition(chunkPosition);
            if (neighborChunk != nullptr) neighborChunk->update();
        }
        else if (position.y == CHUNK_SIZE - 1)
        {
            glm::ivec3 chunkPosition = chunkPosition_;
            ++chunkPosition.y;
            Chunk *neighborChunk = world_->getChunkByChunkPosition(chunkPosition);
            if (neighborChunk != nullptr) neighborChunk->update();
        }
        if (position.z == 0)
        {
            glm::ivec3 chunkPosition = chunkPosition_;
            --chunkPosition.z;
            Chunk *neighborChunk = world_->getChunkByChunkPosition(chunkPosition);
            if (neighborChunk != nullptr) neighborChunk->update();
        }
        else if (position.z == CHUNK_SIZE - 1)
        {
            glm::ivec3 chunkPosition = chunkPosition_;
            ++chunkPosition.z;
            Chunk *neighborChunk = world_->getChunkByChunkPosition(chunkPosition);
            if (neighborChunk != nullptr) neighborChunk->update();
        }
    }
}

const Block &Chunk::getBlock(const glm::ivec3 &position) const
{
#ifndef NDEBUG
    if (0 > position.x || position.x > CHUNK_SIZE)
    logError(
            "The block you are trying to get is not in this chunk. Axis: x")

    if (0 > position.y || position.y > CHUNK_SIZE)
    logError(
            "The block you are trying to get is not in this chunk. Axis: y")

    if (0 > position.z || position.z > CHUNK_SIZE)
    logError(
            "The block you are trying to get is not in this chunk. Axis: z")
#endif
    return blocks_[position.x + CHUNK_SIZE * (position.y + CHUNK_SIZE * position.z)];
}

const glm::ivec3 &Chunk::getChunkPosition() const
{
    return chunkPosition_;
}

bool Chunk::isLoaded()
{
    return mesh_ != nullptr;
}

void Chunk::unload()
{
    if (!empty_ && isLoaded())
        mesh_.reset();
}

bool Chunk::isEmpty()
{
    return empty_;
}
