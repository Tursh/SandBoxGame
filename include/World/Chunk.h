//
// Created by tursh on 4/30/19.
//

#pragma once


#include <glm/vec3.hpp>
#include <Loader/Models/TexturedModel.h>
#include "Bloc.h"

extern const unsigned int CHUNK_SIZE;

class World;

class Chunk
{
    World *world_;

    glm::ivec3 chunkPosition_;

    Bloc *blocs_;

    std::shared_ptr<CGE::Loader::Model> model_;

    void loadToTexModel();

public:
    Chunk(Bloc *blocs, World *world, glm::ivec3 &chunkPosition);

    void update();

    void render();

    void setBloc(glm::ivec3 &position, Bloc &newBloc);

    const Bloc &getBloc(glm::ivec3 &position);
};


