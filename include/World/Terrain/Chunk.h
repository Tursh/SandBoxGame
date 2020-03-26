//
// Created by tursh on 4/30/19.
//

#pragma once


#include <glm/vec3.hpp>
#include <Loader/Meshes/TexturedMesh.h>
#include "Block.h"

extern const int CHUNK_SIZE, SQUARED_CHUNK_SIZE, CUBED_CHUNK_SIZE;

class World;

class Chunk : public CGE::Loader::TexturedMesh
{
    World *world_;

    glm::ivec3 chunkPosition_;

    Block *blocks_;

    bool empty_ = false;

    void loadToTexMesh();

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


