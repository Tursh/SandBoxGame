//
// Created by tursh on 4/30/19.
//

#pragma once

#include "Bloc.h"
#include "Chunk.h"
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

    std::map<unsigned int, std::map<unsigned int, std::map<unsigned int, Chunk *>>> chunks_;

    std::vector<std::shared_ptr<CGE::Entities::Entity>> entities_;

    Entities::Player *player_;

    CGE::View::Camera camera_;

    CGE::Utils::PerlinNoise pn;

    const std::function<glm::vec3(CGE::Entities::Entity *)> collisionFunction_;

    /**
     * Get the chunk at "chunkPosition"
     * @param chunkPosition chunk location (in chunk position = bloc position / 16)
     * @return The chunk at "chunkPosition"
     */
    Chunk *getChunkByChunkPosition(glm::ivec3 chunkPosition);

    /**
     * Translate the position in world to position in chunk
     * @param blocPosition Bloc position in the world
     * @return Bloc position in its chunk
     */
    glm::ivec3 getPositionInChunk(glm::ivec3 &blocPosition) const;

public:
    World();

    /**
     * Update the terrain and entities
     */
    void tick();

    /**
     * Render the terrain and the entities
     */
    void render();

    /**
     * Get the chunk where the bloc at "position" location
     * @param position bloc location
     * @return the chunk where the bloc is located
     */
    Chunk *getChunk(glm::ivec3 position);

    /**
     * Get the chunk where the bloc at "position" location
     * @param position bloc location
     * @return the chunk where the bloc is located
     */
    Chunk *getChunk(const glm::vec3 &position);

    //Get the 6 chunks around a specific chunk
    Chunk **getAroundChunk(glm::ivec3 chunkPosition);

    /**
     * Set a bloc at a specific location
     * @param position Where the bloc will be placed
     * @param bloc The bloc
     */
    void setBloc(glm::ivec3 position, Bloc bloc);

    const Bloc &getBloc(glm::ivec3 position);

    /**
     * Return the list of hitbox in an area
     * @param area The to check for blocs
     * @return The list of bloc hitboxes
     */
    std::vector<Hitbox> getBlocHitboxs(Hitbox area);

    void addEntity(std::shared_ptr<CGE::Entities::Entity> newEntity);

    /**
     * Get the position of the bloc the camera is looking at
     * @param raySize The maximum distance from the camera
     * @return The bloc location / If not bloc, return glm::ivec3(INT_MAX)
     */
    glm::ivec3 getPickedBloc(float raySize);
};


