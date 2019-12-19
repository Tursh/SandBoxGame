//
// Created by tursh on 4/30/19.
//

#pragma once


#include <glm/vec3.hpp>
#include <Loader/Models/TexturedModel.h>
#include "Block.h"

extern const unsigned int CHUNK_SIZE, SQUARED_CHUNK_SIZE;

class World;

class Chunk : public CGE::Loader::TexturedModel
{
    World *world_;

    glm::ivec3 chunkPosition_;

    Block *blocks_;

    bool empty_ = false;

    void loadToTexModel();

    void loadBlock(const glm::ivec3 &position, std::vector<float> &vertices, std::vector<float> &texCoords,
                   std::vector<unsigned int> &indices, Chunk **chunkList);

    void loadFace(const Block &currentBlock, std::vector<float> &vertices, std::vector<float> &texCoords,
                  std::vector<unsigned int> &indices, const int &x, const int &y, const int &z, Blocks::Face face);

public:
    Chunk(Block *blocks, World *world, glm::ivec3 &chunkPosition, bool empty);

    ~Chunk();

    /**
     * Reload the textured model
     */
    void update();

    /**
     * Unload the textured model of the chunk
     */
    void unload();

    /**
     * Reload the 6 chunks that share borders with this chunk
     */
    void updateChunksAround();

    const glm::ivec3 &getChunkPosition() const;

    /**
     * Set the block at this position in the chunk
     * @param position The position in the chunk
     * @param newBlock The new block
     */
    void setBlock(const glm::ivec3 &position, const Block &newBlock);

    const Block &getBlock(const glm::ivec3 &position) const;

    bool isLoaded();

    bool isEmpty();
};


