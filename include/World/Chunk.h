//
// Created by tursh on 4/30/19.
//

#pragma once


#include <glm/vec3.hpp>
#include <Loader/Models/TexturedModel.h>
#include "Bloc.h"

extern const unsigned int CHUNK_SIZE;

class World;

class Chunk : public CGE::Loader::TexturedModel
{
    World *world_;

    glm::ivec3 chunkPosition_;

    Bloc *blocs_;

    void loadToTexModel();

    void loadFace(const Bloc &currentBloc, std::vector<float> &vertices, std::vector<float> &texCoords,
                  std::vector<unsigned int> &indices, int &x, int &y,
                  int &z, Blocs::Face face);

public:
    Chunk(Bloc *blocs, World *world, glm::ivec3 &chunkPosition);

    //Reload the chunk
    void update();

    //Reload the 6 chunk around this chunk
    void updateChunksAround();

    const glm::ivec3 &getChunkPosition() const;

    void setBloc(glm::ivec3 &position, Bloc &newBloc);

    const Bloc &getBloc(glm::ivec3 &position);

    bool isLoaded();
};


