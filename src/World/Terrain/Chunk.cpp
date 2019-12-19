//
// Created by tursh on 4/30/19.
//

#include <Utils/Log.h>
#include "World/Terrain/Chunk.h"
#include <World/World.h>
#include <Loader/RessourceManager.h>
#include <glm/gtx/string_cast.hpp>

const unsigned int CHUNK_SIZE = 16, SQUARED_CHUNK_SIZE = CHUNK_SIZE * CHUNK_SIZE;

/*

void
Chunk::loadFace(const Block &currentBlock, std::vector<float> &vertices, std::vector<float> &texCoords,
                std::vector<unsigned int> &indices, const int &x, const int &y,
                const int &z, Blocks::Face face)
{
    //Get vertices and indices
    auto data = Blocks::getFace(face);
    const auto *faceVertices = std::get<0>(data);
    const auto *faceIndices = std::get<1>(data);

    unsigned int index = vertices.size(),
            indicesSize = indices.size(),
            faceCount = indicesSize / 6;

    //TODO check state

    std::copy(faceVertices, faceVertices + Blocks::POSITION_PER_FACE, std::back_inserter(vertices));
    std::copy(faceIndices, faceIndices + Blocks::INDICES_PER_FACE, std::back_inserter(indices));

    for (unsigned int i = index; i < (unsigned int) vertices.size(); i += 3)
    {
        vertices[i] += (float) x * Blocks::CUBE_SIZE;
        vertices[i + 1] += (float) y * Blocks::CUBE_SIZE;
        vertices[i + 2] += (float) z * Blocks::CUBE_SIZE;
    }

    glm::vec4 currentTexCoords = texture_.get()->getTextureLimits(currentBlock.ID);
    float texCoordsBuf[4 * 2] =
            {
                    currentTexCoords.z,
                    currentTexCoords.w,
                    currentTexCoords.x,
                    currentTexCoords.w,
                    currentTexCoords.x,
                    currentTexCoords.y,
                    currentTexCoords.z,
                    currentTexCoords.y,
            };

    std::copy(texCoordsBuf, texCoordsBuf + 8, std::back_inserter(texCoords));

    for (unsigned int i = indicesSize; i < (unsigned int) indices.size(); ++i)
    {
        indices[i] += faceCount * 4;
    }

}

void Chunk::loadBlock(const glm::ivec3 &position, std::vector<float> &vertices, std::vector<float> &texCoords,
                     std::vector<unsigned int> &indices, Chunk **chunkList)
{
    const Block &currentBlock = getBlock(position);

    if (currentBlock.ID == Blocks::AIR)
        return;

    const int &x = position.x, &y = position.y, &z = position.z;

    //Check extremities
    if (position.x == 0)
    {
        glm::ivec3 blockPosition = position;
        blockPosition.x = CHUNK_SIZE - 1;
        if (chunkList[0] != nullptr && chunkList[0]->getBlock(blockPosition).ID == Blocks::AIR)
        {
            loadFace(currentBlock, vertices, texCoords, indices, x, y, z, Blocks::LEFT);
        }
        blockPosition.x = x + 1;
        if (getBlock(blockPosition).ID == Blocks::AIR)
        {
            loadFace(currentBlock, vertices, texCoords, indices, x, y, z, Blocks::RIGHT);
        }

    } else if (x == CHUNK_SIZE - 1)
    {
        glm::ivec3 blockPosition = position;
        blockPosition.x = 0;
        if (chunkList[1] != nullptr && chunkList[1]->getBlock(blockPosition).ID == Blocks::AIR)
        {
            loadFace(currentBlock, vertices, texCoords, indices, x, y, z, Blocks::RIGHT);
        }
        blockPosition.x = x - 1;
        if (getBlock(blockPosition).ID == Blocks::AIR)
        {
            loadFace(currentBlock, vertices, texCoords, indices, x, y, z, Blocks::LEFT);
        }
    } else
    {
        glm::ivec3 blockPosition = position;
        blockPosition.x = x - 1;
        if (getBlock(blockPosition).ID == Blocks::AIR)
            loadFace(currentBlock, vertices, texCoords, indices, x, y, z, Blocks::LEFT);
        blockPosition.x = x + 1;
        if (getBlock(blockPosition).ID == Blocks::AIR)
            loadFace(currentBlock, vertices, texCoords, indices, x, y, z, Blocks::RIGHT);
    }

    //Check extremities
    if (y == 0)
    {
        glm::ivec3 blockPosition = position;
        blockPosition.y = CHUNK_SIZE - 1;
        if (chunkList[2] != nullptr && chunkList[2]->getBlock(blockPosition).ID == Blocks::AIR)
        {
            loadFace(currentBlock, vertices, texCoords, indices, x, y, z, Blocks::BOTTOM);
        }
        blockPosition.y = y + 1;
        if (getBlock(blockPosition).ID == Blocks::AIR)
        {
            loadFace(currentBlock, vertices, texCoords, indices, x, y, z, Blocks::TOP);
        }

    } else if (y == CHUNK_SIZE - 1)
    {
        glm::ivec3 blockPosition = position;
        blockPosition.y = 0;
        if (chunkList[3] != nullptr && chunkList[3]->getBlock(blockPosition).ID == Blocks::AIR)
        {
            loadFace(currentBlock, vertices, texCoords, indices, x, y, z, Blocks::TOP);
        }
        blockPosition.y = y - 1;
        if (getBlock(blockPosition).ID == Blocks::AIR)
        {
            loadFace(currentBlock, vertices, texCoords, indices, x, y, z, Blocks::BOTTOM);
        }
    } else
    {
        glm::ivec3 blockPosition = position;
        blockPosition.y = y - 1;
        if (getBlock(blockPosition).ID == Blocks::AIR)
            loadFace(currentBlock, vertices, texCoords, indices, x, y, z, Blocks::BOTTOM);
        blockPosition.y = y + 1;
        if (getBlock(blockPosition).ID == Blocks::AIR)
            loadFace(currentBlock, vertices, texCoords, indices, x, y, z, Blocks::TOP);
    }

    //Check extremities
    if (z == 0)
    {
        glm::ivec3 blockPosition = position;
        blockPosition.z = CHUNK_SIZE - 1;
        if (chunkList[4] != nullptr && chunkList[4]->getBlock(blockPosition).ID == Blocks::AIR)
        {
            loadFace(currentBlock, vertices, texCoords, indices, x, y, z, Blocks::BACK);
        }
        blockPosition.z = z + 1;
        if (getBlock(blockPosition).ID == Blocks::AIR)
        {
            loadFace(currentBlock, vertices, texCoords, indices, x, y, z, Blocks::FRONT);
        }

    } else if (z == CHUNK_SIZE - 1)
    {
        glm::ivec3 blockPosition = position;
        blockPosition.z = 0;
        if (chunkList[5] != nullptr && chunkList[5]->getBlock(blockPosition).ID == Blocks::AIR)
        {
            loadFace(currentBlock, vertices, texCoords, indices, x, y, z, Blocks::FRONT);
        }
        blockPosition.z = z - 1;
        if (getBlock(blockPosition).ID == Blocks::AIR)
        {
            loadFace(currentBlock, vertices, texCoords, indices, x, y, z, Blocks::BACK);
        }
    } else
    {
        glm::ivec3 blockPosition = position;
        blockPosition.z = z - 1;
        if (getBlock(blockPosition).ID == Blocks::AIR)
            loadFace(currentBlock, vertices, texCoords, indices, x, y, z, Blocks::BACK);
        blockPosition.z = z + 1;
        if (getBlock(blockPosition).ID == Blocks::AIR)
            loadFace(currentBlock, vertices, texCoords, indices, x, y, z, Blocks::FRONT);
    }
}
*/

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

void Chunk::loadToTexModel()
{
    if (empty_)
        return;

    //First get the six around this one
    Chunk **neighborChunks = world_->getAroundChunk(chunkPosition_);

    //Create vertex arrays
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords;
    std::vector<unsigned int> indices;

    for (int x = 0; x < CHUNK_SIZE; ++x)
        for (int y = 0; y < CHUNK_SIZE; ++y)
            for (int z = 0; z < CHUNK_SIZE; ++z)
            {
                glm::ivec3 blockPosition = {x, y, z};
                Block *currentBlock =
                        blocks_ + blockPosition.x + CHUNK_SIZE * (blockPosition.y + CHUNK_SIZE * blockPosition.z);

                //If it's a block of air there is nothing to load
                if (currentBlock->ID == Blocks::AIR)
                    continue;

                //Get block neighbors
                std::array<const Block *, 6> neighbors = getBlockNeighbors(blockPosition, currentBlock, neighborChunks);

                bool isObstructed = true;
                for (int i = 0; i < 6; ++i)
                {
                    if (neighbors[i] == nullptr || neighbors[i]->ID || neighbors[i]->state)
                        isObstructed = false;
                }

                glm::vec4 blockTexCoords = texture_->getTextureLimits(currentBlock->ID);

                if (!isObstructed)
                    Blocks::loadBlock(vertices, texCoords, indices, blockPosition, currentBlock, neighbors.data(),
                                      blockTexCoords);
            }

    delete[] neighborChunks;

    //for (int i = 0; i < vertices.size(); ++i)
    //	logInfo(i << ": " << glm::to_string(vertices[i]));

    CGE::Loader::Data<float> verticesData((float *) vertices.data(), vertices.size() * 3);
    CGE::Loader::Data<float> texCoordsData((float *) texCoords.data(), texCoords.size() * 2);
    CGE::Loader::Data<unsigned int> indicesData(indices.data(), indices.size());

    if (indices.empty())
    {
        empty_ = true;
        return;
    }

    CGE::Loader::DataToVAO(model_, verticesData, texCoordsData, indicesData, true);
}


Chunk::Chunk(Block *blocks, World *world, glm::ivec3 &chunkPosition, bool empty)
        : TexturedModel(nullptr, CGE::Loader::resManager::getTexture(1), true),
          blocks_(blocks), world_(world), chunkPosition_(chunkPosition), empty_(empty)
{
    loadToTexModel();
}

Chunk::~Chunk()
{
    unload();
    delete[] blocks_;
}

void Chunk::update()
{
    //Reload the Blocks to the textured model
    loadToTexModel();
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

    if (empty_ && newBlock != Blocks::AIR_BLOC)
        empty_ = false;

    currentBlock = newBlock;
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
    return model_ != nullptr;
}

void Chunk::unload()
{
    if (!empty_ && isLoaded())
        model_.reset();
}

bool Chunk::isEmpty()
{
    return empty_;
}
