//
// Created by tursh on 4/30/19.
//

#include <Utils/Log.h>
#include "World/Chunk.h"
#include <World/World.h>
#include <Loader/RessourceManager.h>

const unsigned int CHUNK_SIZE = 16;

void
Chunk::loadFace(const Bloc &currentBloc, std::vector<float> &vertices, std::vector<float> &texCoords,
                std::vector<unsigned int> &indices, int &x, int &y,
                int &z, Blocs::Face face)
{
    //Get vertices and indices
    auto data = Blocs::getFace(face);
    const auto *faceVertices = std::get<0>(data);
    const auto *faceIndices = std::get<1>(data);

    unsigned int index = vertices.size(),
            indicesSize = indices.size(),
            faceCount = indicesSize / 6;

    //TODO check state

    std::copy(faceVertices, faceVertices + Blocs::FACE_VERTICES_COUNT, std::back_inserter(vertices));
    std::copy(faceIndices, faceIndices + Blocs::FACE_INDICES_COUNT, std::back_inserter(indices));

    for (int i = index; i < vertices.size(); i += 3)
    {
        vertices[i] += x * Blocs::CUBE_SIZE * 2;
        vertices[i + 1] += y * Blocs::CUBE_SIZE * 2;
        vertices[i + 2] += z * Blocs::CUBE_SIZE * 2;
    }

    glm::vec4 currentTexCoords = texture_.get()->getTextureLimits(currentBloc.ID);
    currentTexCoords = {0.0625f, 1, 0.125f, 1 - 0.0625f};
    auto *texCoordsBuf = new float[4 * 2];
    texCoordsBuf[0] = currentTexCoords.z;
    texCoordsBuf[1] = currentTexCoords.w;
    texCoordsBuf[2] = currentTexCoords.x;
    texCoordsBuf[3] = currentTexCoords.w;
    texCoordsBuf[4] = currentTexCoords.x;
    texCoordsBuf[5] = currentTexCoords.y;
    texCoordsBuf[6] = currentTexCoords.z;
    texCoordsBuf[7] = currentTexCoords.y;

    std::copy(texCoordsBuf, texCoordsBuf + 8, std::back_inserter(texCoords));
    delete[] texCoordsBuf;

    for (int i = indicesSize; i < indices.size(); ++i)
    {
        indices[i] += faceCount * 4;
    }

}

void Chunk::loadToTexModel()
{


    //First get the six around this one
    Chunk **chunkList = world_->getAroundChunk(chunkPosition_);

    //Create vertex arrays
    std::vector<float> vertices;
    std::vector<float> texCoords;
    std::vector<unsigned int> indices;

    for (int x = 0; x < CHUNK_SIZE; ++x)
        for (int y = 0; y < CHUNK_SIZE; ++y)
            for (int z = 0; z < CHUNK_SIZE; ++z)
            {
                glm::ivec3 blocPosition = {x, y, z};
                const Bloc &currentBloc = getBloc(blocPosition);
                if (currentBloc.ID == Blocs::AIR)
                    continue;

                //Check extremities
                if (x == 0)
                {
                    blocPosition = {CHUNK_SIZE - 1, y, z};
                    if (chunkList[0] == nullptr || chunkList[0]->getBloc(blocPosition).ID == Blocs::AIR)
                    {
                        loadFace(currentBloc, vertices, texCoords, indices, x, y, z, Blocs::LEFT);
                    }
                    blocPosition.x = x + 1;
                    if (getBloc(blocPosition).ID == Blocs::AIR)
                    {
                        loadFace(currentBloc, vertices, texCoords, indices, x, y, z, Blocs::RIGHT);
                    }

                } else if (x == CHUNK_SIZE - 1)
                {
                    blocPosition = {0, y, z};
                    if (chunkList[1] == nullptr || chunkList[1]->getBloc(blocPosition).ID == Blocs::AIR)
                    {
                        loadFace(currentBloc, vertices, texCoords, indices, x, y, z, Blocs::RIGHT);
                    }
                    blocPosition.x = x - 1;
                    if (getBloc(blocPosition).ID == Blocs::AIR)
                    {
                        loadFace(currentBloc, vertices, texCoords, indices, x, y, z, Blocs::LEFT);
                    }
                } else
                {
                    blocPosition = {x - 1, y, z};
                    if (getBloc(blocPosition).ID == Blocs::AIR)
                        loadFace(currentBloc, vertices, texCoords, indices, x, y, z, Blocs::LEFT);
                    blocPosition.x = x + 1;
                    if (getBloc(blocPosition).ID == Blocs::AIR)
                        loadFace(currentBloc, vertices, texCoords, indices, x, y, z, Blocs::RIGHT);
                }

                //Check extremities
                if (y == 0)
                {
                    blocPosition = {x, CHUNK_SIZE - 1, z};
                    if (chunkList[2] == nullptr || chunkList[2]->getBloc(blocPosition).ID == Blocs::AIR)
                    {
                        loadFace(currentBloc, vertices, texCoords, indices, x, y, z, Blocs::BOTTOM);
                    }
                    blocPosition.y = y + 1;
                    if (getBloc(blocPosition).ID == Blocs::AIR)
                    {
                        loadFace(currentBloc, vertices, texCoords, indices, x, y, z, Blocs::TOP);
                    }

                } else if (y == CHUNK_SIZE - 1)
                {
                    blocPosition = {x, 0, z};
                    if (chunkList[3] == nullptr || chunkList[3]->getBloc(blocPosition).ID == Blocs::AIR)
                    {
                        loadFace(currentBloc, vertices, texCoords, indices, x, y, z, Blocs::TOP);
                    }
                    blocPosition.y = y - 1;
                    if (getBloc(blocPosition).ID == Blocs::AIR)
                    {
                        loadFace(currentBloc, vertices, texCoords, indices, x, y, z, Blocs::BOTTOM);
                    }
                } else
                {
                    blocPosition = {x, y - 1, z};
                    if (getBloc(blocPosition).ID == Blocs::AIR)
                        loadFace(currentBloc, vertices, texCoords, indices, x, y, z, Blocs::BOTTOM);
                    blocPosition.y = y + 1;
                    if (getBloc(blocPosition).ID == Blocs::AIR)
                        loadFace(currentBloc, vertices, texCoords, indices, x, y, z, Blocs::TOP);
                }


                //Check extremities
                if (z == 0)
                {
                    blocPosition = {x, y, CHUNK_SIZE - 1};
                    if (chunkList[4] == nullptr || chunkList[4]->getBloc(blocPosition).ID == Blocs::AIR)
                    {
                        loadFace(currentBloc, vertices, texCoords, indices, x, y, z, Blocs::BACK);
                    }
                    blocPosition.z = z + 1;
                    if (getBloc(blocPosition).ID == Blocs::AIR)
                    {
                        loadFace(currentBloc, vertices, texCoords, indices, x, y, z, Blocs::FRONT);
                    }

                } else if (z == CHUNK_SIZE - 1)
                {
                    blocPosition = {x, y, 0};
                    if (chunkList[5] == nullptr || chunkList[5]->getBloc(blocPosition).ID == Blocs::AIR)
                    {
                        loadFace(currentBloc, vertices, texCoords, indices, x, y, z, Blocs::FRONT);
                    }
                    blocPosition.z = z - 1;
                    if (getBloc(blocPosition).ID == Blocs::AIR)
                    {
                        loadFace(currentBloc, vertices, texCoords, indices, x, y, z, Blocs::BACK);
                    }
                } else
                {
                    blocPosition = {x, y, z - 1};
                    if (getBloc(blocPosition).ID == Blocs::AIR)
                        loadFace(currentBloc, vertices, texCoords, indices, x, y, z, Blocs::BACK);
                    blocPosition.z = z + 1;
                    if (getBloc(blocPosition).ID == Blocs::AIR)
                        loadFace(currentBloc, vertices, texCoords, indices, x, y, z, Blocs::FRONT);
                }
            }
    CGE::Loader::Data<float> verticesData(vertices.data(), vertices.size());
    CGE::Loader::Data<float> texCoordsData(texCoords.data(), texCoords.size());
    CGE::Loader::Data<unsigned int> indicesData(indices.data(), indices.size());

    CGE::Loader::DataToVAO(&model_, verticesData, texCoordsData, indicesData, true);
}

Chunk::Chunk(Bloc *blocs, World *world, glm::ivec3 &chunkPosition)
        : TexturedModel(nullptr, CGE::Loader::resManager::getTexture(1), true),
          blocs_(blocs), world_(world), chunkPosition_(chunkPosition)
{
    loadToTexModel();
}

void Chunk::update()
{
    //Reload the Blocs to the textured model
    loadToTexModel();
}

void Chunk::updateChunksAround()
{
    auto neiborChunk = world_->getAroundChunk(chunkPosition_);
    for (int i = 0; i < 6; ++i)
    {
        if (neiborChunk[i] != nullptr)
            neiborChunk[i]->update();
    }
}

void Chunk::setBloc(glm::ivec3 &position, Bloc &newBloc)
{
#ifndef NDEBUG
    if (0 > position.x || position.x > CHUNK_SIZE)
    logError("The bloc you are trying to get is not in this chunk. Axis: x")

    if (0 > position.y || position.y > CHUNK_SIZE)
    logError("The bloc you are trying to get is not in this chunk. Axis: y")

    if (0 > position.z || position.z > CHUNK_SIZE)
    logError("The bloc you are trying to get is not in this chunk. Axis: z")
#endif

    Bloc &currentBloc = blocs_[position.x + CHUNK_SIZE * (position.z + CHUNK_SIZE * position.y)];

    if (currentBloc == newBloc)
        return;

    currentBloc = newBloc;
    update();

    // If the new bloc is air than check if it is at the extremities
    // of the chunk and update the chunks that are touching that bloc.
    if (newBloc.ID == Blocs::AIR)
    {
        if (position.x == 0)
        {
            glm::ivec3 chunkPosition = chunkPosition_;
            --chunkPosition.x;
            Chunk *neighborChunk = world_->getChunk(chunkPosition);
            if (neighborChunk != nullptr) neighborChunk->update();
        } else if (position.x == CHUNK_SIZE - 1)
        {
            glm::ivec3 chunkPosition = chunkPosition_;
            ++chunkPosition.x;
            Chunk *neighborChunk = world_->getChunk(chunkPosition);
            if (neighborChunk != nullptr) neighborChunk->update();
        }
        if (position.y == 0)
        {
            glm::ivec3 chunkPosition = chunkPosition_;
            --chunkPosition.y;
            Chunk *neighborChunk = world_->getChunk(chunkPosition);
            if (neighborChunk != nullptr) neighborChunk->update();
        } else if (position.y == CHUNK_SIZE - 1)
        {
            glm::ivec3 chunkPosition = chunkPosition_;
            ++chunkPosition.y;
            Chunk *neighborChunk = world_->getChunk(chunkPosition);
            if (neighborChunk != nullptr) neighborChunk->update();
        }
        if (position.z == 0)
        {
            glm::ivec3 chunkPosition = chunkPosition_;
            --chunkPosition.z;
            Chunk *neighborChunk = world_->getChunk(chunkPosition);
            if (neighborChunk != nullptr) neighborChunk->update();
        } else if (position.z == CHUNK_SIZE - 1)
        {
            glm::ivec3 chunkPosition = chunkPosition_;
            ++chunkPosition.z;
            Chunk *neighborChunk = world_->getChunk(chunkPosition);
            if (neighborChunk != nullptr) neighborChunk->update();
        }
    }
}

const Bloc &Chunk::getBloc(glm::ivec3 &position)
{
#ifndef NDEBUG
    if (0 > position.x || position.x > CHUNK_SIZE)
    logError("The bloc you are trying to get is not in this chunk. Axis: x")

    if (0 > position.y || position.y > CHUNK_SIZE)
    logError("The bloc you are trying to get is not in this chunk. Axis: y")

    if (0 > position.z || position.z > CHUNK_SIZE)
    logError("The bloc you are trying to get is not in this chunk. Axis: z")
#endif
    return blocs_[position.x + CHUNK_SIZE * (position.z + CHUNK_SIZE * position.y)];
}

const glm::ivec3 &Chunk::getChunkPosition() const
{
    return chunkPosition_;
}

bool Chunk::isLoaded()
{
    return model_ != nullptr;
}
