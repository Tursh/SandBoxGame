//
// Created by tursh on 4/30/19.
//

#include <Utils/Log.h>
#include "World/Chunk.h"
#include <World/World.h>
#include <Loader/RessourceManager.h>

const unsigned int CHUNK_SIZE = 16, BLOCK_PER_CHUNK = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

CGE::Loader::Texture *blocsTextures;

static void
loadFace(const Bloc &currentBloc, std::vector<float> &vertices, std::vector<float> &texCoords,
         std::vector<unsigned int> &indices, int &x, int &y,
         int &z, Blocs::Face face)
{
    //Get vertices and indices
    auto data = Blocs::getFace(face);
    const auto *faceVertices = std::get<0>(data);
    const auto *faceIndices = std::get<1>(data);

    unsigned int index = vertices.size(),
            indicesSize = indices.size();

    //TODO check state

    std::copy(faceVertices, &faceVertices[Blocs::FACE_VERTICES_COUNT], std::back_inserter(vertices));
    std::copy(faceIndices, &faceIndices[Blocs::FACE_INDICES_COUNT], std::back_inserter(indices));

    for (int i = index; i < vertices.size(); i += 3)
    {
        vertices[i] += x;
        vertices[i + 1] += y;
        vertices[i + 2] += z;
    }

    glm::vec4 currentTexCoords = blocsTextures->getTextureCoords(currentBloc.ID);
    auto *texCoordsBuf = new float[4 * 2];
    texCoordsBuf[0] = currentTexCoords.x;
    texCoordsBuf[1] = currentTexCoords.y;
    texCoordsBuf[2] = currentTexCoords.x;
    texCoordsBuf[3] = currentTexCoords.w;
    texCoordsBuf[4] = currentTexCoords.z;
    texCoordsBuf[5] = currentTexCoords.w;
    texCoordsBuf[6] = currentTexCoords.z;
    texCoordsBuf[7] = currentTexCoords.y;

    std::copy(texCoordsBuf, &texCoordsBuf[7], std::back_inserter(texCoords));
    delete[] texCoordsBuf;

    for (int i = indicesSize; i < indices.size(); ++i)
    {
        indices[i] += index;
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
                    blocPosition = {15, y, z};
                    if (chunkList[0] == nullptr || chunkList[0]->getBloc(blocPosition).ID == Blocs::AIR)
                    {
                        loadFace(currentBloc, vertices, texCoords, indices, x, y, z, Blocs::LEFT);
                    }
                    blocPosition.x = x + 1;
                    if (getBloc(blocPosition).ID == Blocs::AIR)
                    {
                        loadFace(currentBloc, vertices, texCoords, indices, x, y, z, Blocs::RIGHT);
                    }

                } else if (x == 15)
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
                    blocPosition = {x, 15, z};
                    if (chunkList[2] == nullptr || chunkList[2]->getBloc(blocPosition).ID == Blocs::AIR)
                    {
                        loadFace(currentBloc, vertices, texCoords, indices, x, y, z, Blocs::BOTTOM);
                    }
                    blocPosition.y = y + 1;
                    if (getBloc(blocPosition).ID == Blocs::AIR)
                    {
                        loadFace(currentBloc, vertices, texCoords, indices, x, y, z, Blocs::TOP);
                    }

                } else if (y == 15)
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
                    blocPosition = {x, y, 15};
                    if (chunkList[4] == nullptr || chunkList[4]->getBloc(blocPosition).ID == Blocs::AIR)
                    {
                        loadFace(currentBloc, vertices, texCoords, indices, x, y, z, Blocs::BACK);
                    }
                    blocPosition.z = z + 1;
                    if (getBloc(blocPosition).ID == Blocs::AIR)
                    {
                        loadFace(currentBloc, vertices, texCoords, indices, x, y, z, Blocs::FRONT);
                    }

                } else if (z == 15)
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

    model_ = CGE::Loader::DataToVAO(verticesData, texCoordsData, indicesData, true);
}

Chunk::Chunk(Bloc *blocs, World *world, glm::ivec3 &chunkPosition)
        : blocs_(blocs), model_(nullptr), world_(world), chunkPosition_(chunkPosition)
{
    if (blocsTextures == nullptr)
    {
        blocsTextures = new CGE::Loader::Texture(glm::ivec2(256 / 16));
        blocsTextures->loadTexture("textureMap16x16.png");
    }
    loadToTexModel();
}

void Chunk::update()
{
    //Reload the Blocs to the textured model
    loadToTexModel();
}

void Chunk::render()
{
    if (model_ != nullptr)
    {
        blocsTextures->bind();
        model_->render();
    }
}

void Chunk::setBloc(glm::ivec3 &position, Bloc &newBloc)
{
#ifndef NDEBUG
    if (0 > position.x || position.x > CHUNK_SIZE)
    logError("The bloc you are trying to get is not in this chunk. Axis: x");

    if (0 > position.y || position.y > CHUNK_SIZE)
    logError("The bloc you are trying to get is not in this chunk. Axis: y");

    if (0 > position.z || position.z > CHUNK_SIZE)
    logError("The bloc you are trying to get is not in this chunk. Axis: z");
#endif

    blocs_[position.x + CHUNK_SIZE * (position.z + CHUNK_SIZE * position.y)] = newBloc;
    update();
}

const Bloc &Chunk::getBloc(glm::ivec3 &position)
{
#ifndef NDEBUG
    if (0 > position.x || position.x > CHUNK_SIZE)
    logError("The bloc you are trying to get is not in this chunk. Axis: x");

    if (0 > position.y || position.y > CHUNK_SIZE)
    logError("The bloc you are trying to get is not in this chunk. Axis: y");

    if (0 > position.z || position.z > CHUNK_SIZE)
    logError("The bloc you are trying to get is not in this chunk. Axis: z");
#endif
    return blocs_[position.x + CHUNK_SIZE * (position.z + CHUNK_SIZE * position.y)];
}

