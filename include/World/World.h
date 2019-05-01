//
// Created by tursh on 4/30/19.
//

#pragma once

#include "Chunk.h"
#include <map>

class World
{
    std::map<unsigned int, std::map<unsigned int, std::map<unsigned int, Chunk>>> chunks_;
public:
    void tick();

    void render();

    Chunk &getChunk(glm::ivec3 position);

    Chunk &getChunk(const glm::vec3 &position);

    void setBloc(glm::ivec3 position, Bloc bloc);

    Bloc getBloc(glm::ivec3 position);
};


