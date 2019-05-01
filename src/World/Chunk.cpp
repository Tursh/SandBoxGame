//
// Created by tursh on 4/30/19.
//

#include <Utils/Log.h>
#include "World/Chunk.h"

const unsigned int CHUNK_SIZE = 16;

void Chunk::loadToTexModel()
{
    //First get the six around this one

}

Chunk::Chunk(Bloc *blocs)
        : blocs_(blocs), texModel_(nullptr)
{
    loadToTexModel();
}

void Chunk::update()
{
    //Reload the Blocs to the textured model
    loadToTexModel();
}

void Chunk::render()
{
    texModel_->render();
}

void Chunk::setBloc(glm::ivec3 &position, Bloc &newBloc)
{
#ifndef NDEBUG
    if (0 > position.x || position.x > CHUNK_SIZE)
    logError("The bloc you are trying to get is not in this chunk. Axis: x");

    if (0 > position.y || position.y > CHUNK_SIZE)
    logError("The bloc you are trying to get is not in this chunk. Axis: x");

    if (0 > position.z || position.z > CHUNK_SIZE)
    logError("The bloc you are trying to get is not in this chunk. Axis: x");
#endif

    blocs_[position.x + CHUNK_SIZE * (position.z + CHUNK_SIZE * position.y)] = newBloc;
    update();
}

Bloc Chunk::getBloc(glm::ivec3 &position)
{
#ifndef NDEBUG
    if (0 > position.x || position.x > CHUNK_SIZE)
    logError("The bloc you are trying to get is not in this chunk. Axis: x");

    if (0 > position.y || position.y > CHUNK_SIZE)
    logError("The bloc you are trying to get is not in this chunk. Axis: x");

    if (0 > position.z || position.z > CHUNK_SIZE)
    logError("The bloc you are trying to get is not in this chunk. Axis: x");
#endif
    return blocs_[position.x + CHUNK_SIZE * (position.z + CHUNK_SIZE * position.y)];
}

