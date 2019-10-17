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

    bool empty_ = false;

    void loadToTexModel();

    void loadBloc(const glm::ivec3 &position, std::vector<float> &vertices, std::vector<float> &texCoords,
                  std::vector<unsigned int> &indices, Chunk **chunkList);

    void loadFace(const Bloc &currentBloc, std::vector<float> &vertices, std::vector<float> &texCoords,
                  std::vector<unsigned int> &indices, const int &x, const int &y, const int &z, Blocs::Face face);

public:
    Chunk(Bloc *blocs, World *world, glm::ivec3 &chunkPosition);

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
     * Set the bloc at this position in the chunk
     * @param position The position in the chunk
     * @param newBloc The new bloc
     */
    void setBloc(glm::ivec3 &position, Bloc &newBloc);

    const Bloc &getBloc(const glm::ivec3 &position);

    bool isLoaded();

    bool isEmpty();
};


