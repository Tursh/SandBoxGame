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
    CGE::Shader::BasicShader shader;

    std::map<unsigned int, std::map<unsigned int, std::map<unsigned int, Chunk *>>> chunks_;

    Entities::Player player_;

    CGE::View::Camera camera_;

    Chunk *getChunkByChunkPosition(glm::ivec3 chunkPosition);

    CGE::Utils::PerlinNoise pn;
public:
    World();

    void tick();

    void render();

    Chunk *getChunk(glm::ivec3 position);

    Chunk *getChunk(const glm::vec3 &position);
    //Get the 6 chunks around a specific chunk
    Chunk **getAroundChunk(glm::ivec3 chunkPosition);

    void setBloc(glm::ivec3 position, Bloc bloc);

    const Bloc &getBloc(glm::ivec3 position);
};


