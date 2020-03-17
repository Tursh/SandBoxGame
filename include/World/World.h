//
// Created by tursh on 4/30/19.
//

#pragma once

#include <World/Terrain/Block.h>
#include <World/Terrain/Chunk.h>
#include <World/Terrain/ChunkManager.h>
#include <World/Cloud/CloudManager.h>
#include <map>
#include <glm/vec3.hpp>
#include <View/Camera.h>
#include <Shader/ModelShader/BasicShader.h>
#include <Utils/PerlinNoise.h>
#include <World/Player.h>

class World
{
private:
    CGE::Shader::BasicShader shader;

    std::map<int, std::map<int, std::map<int, Chunk *>>> chunks_;

    std::vector<std::shared_ptr<CGE::Entities::Entity>> entities_;

    Entities::Player *player_;

    CGE::View::Camera camera_;

    const std::function<glm::vec3(CGE::Entities::Entity *)> collisionFunction_;

    ChunkManager chunkManager_;

    CloudManager cloudManager_;

    WorldGenerator worldGenerator_;

    //Chunk buffer for chunks that need to be deleted
    std::vector<Chunk *> chunksToDelete_;

    //Recursive function that delete buffered chunks
    void deleteBufferedChunks();

public:
    World();

    ~World();

    /**
     * Update the terrain and entities
     */
    void tick();

    /**
     * Render the terrain and the entities
     */
    void render();

    void addChunk(Chunk *chunk);

    /**
     * Get the chunk where the block at "position" location
     * @param position block location
     * @return the chunk where the block is located
     */
    Chunk *getChunk(glm::ivec3 position);

    /**
     * Get the chunk where the block at "position" location
     * @param position block location
     * @return the chunk where the block is located
     */
    Chunk *getChunk(const glm::vec3 &position);

    /**
     * Get neighbor chunks around the chunk at 'chunkPosition'
     * @param chunkPosition Chunk position to get neighbors
     * @return Chunk array (x-, x+, y-, y+, z-, z+)
     */
    Chunk **getAroundChunk(glm::ivec3 chunkPosition);

    /**
     * Get the chunk at "chunkPosition"
     * @param chunkPosition chunk location (in chunk position = block position / 16)
     * @return The chunk at "chunkPosition"
     */
    Chunk *getChunkByChunkPosition(glm::ivec3 chunkPosition);

    /**
     * Translate the position in world to position in chunk
     * @param blockPosition Block position in the world
     * @return Block position in its chunk
     */
    static glm::ivec3 getPositionInChunk(glm::ivec3 blockPosition);

    /**
     * Get the position of the chunk that the block is located
     * @param blockPosition The position of the block in the world
     * @return The position of the chunk
     */
    static glm::ivec3 getChunkPosition(glm::ivec3 blockPosition);

    /**
     * Set a block at a specific location
     * @param position Where the block will be placed
     * @param block The block
     */
    void setBlock(glm::ivec3 position, Block block);

    const Block &getBlock(glm::ivec3 position);

    /**
     * Return the list of hitbox in an area
     * @param area The to check for blocks
     * @return The list of block hitboxes
     */
    auto getBlockHitboxes(Hitbox area) -> std::vector<Hitbox>;

    void addEntity(std::shared_ptr<CGE::Entities::Entity> newEntity);

    /**
     * Get the position of the block the camera is looking at
     * @param raySize The maximum distance from the camera
     * @return The block location / If not block, return glm::ivec3(INT_MAX)
     */
    glm::ivec3 getPickedBlock(float raySize);

    void deleteChunk(Chunk *chunk);
};


