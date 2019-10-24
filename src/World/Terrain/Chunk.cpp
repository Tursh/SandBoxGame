//
// Created by tursh on 4/30/19.
//

#include <Utils/Log.h>
#include "World/Terrain/Chunk.h"
#include <World/World.h>
#include <Loader/RessourceManager.h>

const unsigned int CHUNK_SIZE = 16;

/*

void
Chunk::loadFace(const Bloc &currentBloc, std::vector<float> &vertices, std::vector<float> &texCoords,
                std::vector<unsigned int> &indices, const int &x, const int &y,
                const int &z, Blocs::Face face)
{
    //Get vertices and indices
    auto data = Blocs::getFace(face);
    const auto *faceVertices = std::get<0>(data);
    const auto *faceIndices = std::get<1>(data);

    unsigned int index = vertices.size(),
            indicesSize = indices.size(),
            faceCount = indicesSize / 6;

    //TODO check state

    std::copy(faceVertices, faceVertices + Blocs::POSITION_PER_FACE, std::back_inserter(vertices));
    std::copy(faceIndices, faceIndices + Blocs::INDICES_PER_FACE, std::back_inserter(indices));

    for (unsigned int i = index; i < (unsigned int) vertices.size(); i += 3)
    {
        vertices[i] += (float) x * Blocs::CUBE_SIZE;
        vertices[i + 1] += (float) y * Blocs::CUBE_SIZE;
        vertices[i + 2] += (float) z * Blocs::CUBE_SIZE;
    }

    glm::vec4 currentTexCoords = texture_.get()->getTextureLimits(currentBloc.ID);
    float texCoordsBuf[4 * 2] =
            {
                    currentTexCoords.z,
                    currentTexCoords.w,
                    currentTexCoords.x,
                    currentTexCoords.w,
                    currentTexCoords.x,
                    currentTexCoords.y,
                    currentTexCoords.z,
                    currentTexCoords.y,
            };

    std::copy(texCoordsBuf, texCoordsBuf + 8, std::back_inserter(texCoords));

    for (unsigned int i = indicesSize; i < (unsigned int) indices.size(); ++i)
    {
        indices[i] += faceCount * 4;
    }

}

void Chunk::loadBloc(const glm::ivec3 &position, std::vector<float> &vertices, std::vector<float> &texCoords,
                     std::vector<unsigned int> &indices, Chunk **chunkList)
{
    const Bloc &currentBloc = getBloc(position);

    if (currentBloc.ID == Blocs::AIR)
        return;

    const int &x = position.x, &y = position.y, &z = position.z;

    //Check extremities
    if (position.x == 0)
    {
        glm::ivec3 blocPosition = position;
        blocPosition.x = CHUNK_SIZE - 1;
        if (chunkList[0] != nullptr && chunkList[0]->getBloc(blocPosition).ID == Blocs::AIR)
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
        glm::ivec3 blocPosition = position;
        blocPosition.x = 0;
        if (chunkList[1] != nullptr && chunkList[1]->getBloc(blocPosition).ID == Blocs::AIR)
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
        glm::ivec3 blocPosition = position;
        blocPosition.x = x - 1;
        if (getBloc(blocPosition).ID == Blocs::AIR)
            loadFace(currentBloc, vertices, texCoords, indices, x, y, z, Blocs::LEFT);
        blocPosition.x = x + 1;
        if (getBloc(blocPosition).ID == Blocs::AIR)
            loadFace(currentBloc, vertices, texCoords, indices, x, y, z, Blocs::RIGHT);
    }

    //Check extremities
    if (y == 0)
    {
        glm::ivec3 blocPosition = position;
        blocPosition.y = CHUNK_SIZE - 1;
        if (chunkList[2] != nullptr && chunkList[2]->getBloc(blocPosition).ID == Blocs::AIR)
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
        glm::ivec3 blocPosition = position;
        blocPosition.y = 0;
        if (chunkList[3] != nullptr && chunkList[3]->getBloc(blocPosition).ID == Blocs::AIR)
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
        glm::ivec3 blocPosition = position;
        blocPosition.y = y - 1;
        if (getBloc(blocPosition).ID == Blocs::AIR)
            loadFace(currentBloc, vertices, texCoords, indices, x, y, z, Blocs::BOTTOM);
        blocPosition.y = y + 1;
        if (getBloc(blocPosition).ID == Blocs::AIR)
            loadFace(currentBloc, vertices, texCoords, indices, x, y, z, Blocs::TOP);
    }

    //Check extremities
    if (z == 0)
    {
        glm::ivec3 blocPosition = position;
        blocPosition.z = CHUNK_SIZE - 1;
        if (chunkList[4] != nullptr && chunkList[4]->getBloc(blocPosition).ID == Blocs::AIR)
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
        glm::ivec3 blocPosition = position;
        blocPosition.z = 0;
        if (chunkList[5] != nullptr && chunkList[5]->getBloc(blocPosition).ID == Blocs::AIR)
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
        glm::ivec3 blocPosition = position;
        blocPosition.z = z - 1;
        if (getBloc(blocPosition).ID == Blocs::AIR)
            loadFace(currentBloc, vertices, texCoords, indices, x, y, z, Blocs::BACK);
        blocPosition.z = z + 1;
        if (getBloc(blocPosition).ID == Blocs::AIR)
            loadFace(currentBloc, vertices, texCoords, indices, x, y, z, Blocs::FRONT);
    }
}
*/

// For a lack of pow function for integers
static int pow(int base, int power)
{
    int ans = 1;
    for (int i = 0; i < power; ++i)
        ans *= base;
    return ans;
}

static std::array<const Bloc *, 6>
getBlocNeighbors(const glm::ivec3 &blocPosition, const Bloc *bloc, Chunk **neighborChunks)
{
    std::array<const Bloc *, 6> neighbors{};

    for (int axis = 0; axis < 3; ++axis)
    {
        //-
        if (blocPosition[axis] == 0)
        {
            if (neighborChunks[axis * 2] != nullptr)
            {
                glm::ivec3 neighborsPosition = blocPosition;
                neighborsPosition[axis] = CHUNK_SIZE - 1;
                neighbors[axis * 2] = &(neighborChunks[axis * 2]->getBloc(neighborsPosition));
            }
        } else
            neighbors[axis * 2] = bloc - pow(CHUNK_SIZE, axis);
        //+
        if (blocPosition[axis] == CHUNK_SIZE - 1)
        {
            if (neighborChunks[axis * 2] != nullptr)
            {
                glm::ivec3 neighborsPosition = blocPosition;
                neighborsPosition[axis] = 0;
                neighbors[axis * 2 + 1] = &(neighborChunks[axis * 2 + 1]->getBloc(neighborsPosition));
            }
        } else
            neighbors[axis * 2 + 1] = bloc + pow(CHUNK_SIZE, axis);
    }

    return neighbors;
}

void Chunk::loadToTexModel()
{
    if (empty_)
        return;

    //First get the six around this one
    Chunk **neighborChunks = world_->getAroundChunk(chunkPosition_);

    //Create vertex arrays
    std::vector<float> vertices;
    std::vector<float> texCoords;
    std::vector<unsigned int> indices;

    for (int x = 0; x < CHUNK_SIZE; ++x)
        for (int y = 0; y < CHUNK_SIZE; ++y)
            for (int z = 0; z < CHUNK_SIZE; ++z)
            {
                glm::ivec3 blocPosition = {x, y, z};
                Bloc *currentBloc =
                        blocs_ + blocPosition.x + CHUNK_SIZE * (blocPosition.z + CHUNK_SIZE * blocPosition.y);

                //If it's a bloc of air there is nothing to load
                if (currentBloc->ID == Blocs::AIR)
                    continue;

                //Get bloc neighbors
                std::array<const Bloc *, 6> neighbors = getBlocNeighbors(blocPosition, currentBloc, neighborChunks);

                bool isObstructed = true;
                for (int i = 0; i < 6; ++i)
                {
                    if (neighbors[i] == nullptr || neighbors[i]->ID || neighbors[i]->state)
                        isObstructed = false;
                }

                glm::vec4 blocTexCoords = texture_->getTextureLimits(currentBloc->ID);

                if (!isObstructed)
                    Blocs::loadBloc(vertices, texCoords, indices, blocPosition, currentBloc, neighbors.data(),
                                    blocTexCoords);
            }

    delete[] neighborChunks;

    CGE::Loader::Data<float> verticesData(vertices.data(), vertices.size());
    CGE::Loader::Data<float> texCoordsData(texCoords.data(), texCoords.size());
    CGE::Loader::Data<unsigned int> indicesData(indices.data(), indices.size());

    if (indices.empty())
    {
        empty_ = true;
        return;
    }

    CGE::Loader::DataToVAO(model_, verticesData, texCoordsData, indicesData, true);
}


Chunk::Chunk(Bloc *blocs, World *world, glm::ivec3 &chunkPosition)
        : TexturedModel(nullptr, CGE::Loader::resManager::getTexture(1), true),
          blocs_(blocs), world_(world), chunkPosition_(chunkPosition)
{
    loadToTexModel();
}

Chunk::~Chunk()
{
    unload();
    delete[] blocs_;
}

void Chunk::update()
{
    //Reload the Blocs to the textured model
    loadToTexModel();
}

void Chunk::updateChunksAround()
{
    auto neighborChunks = world_->getAroundChunk(chunkPosition_);
    for (int i = 0; i < 6; ++i)
    {
        if (neighborChunks[i] != nullptr)
            neighborChunks[i]->update();
    }
    delete[] neighborChunks;
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

    if (empty_ && newBloc != Blocs::AIR_BLOC)
        empty_ = false;

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
            Chunk *neighborChunk = world_->getChunkByChunkPosition(chunkPosition);
            if (neighborChunk != nullptr) neighborChunk->update();
        } else if (position.x == CHUNK_SIZE - 1)
        {
            glm::ivec3 chunkPosition = chunkPosition_;
            ++chunkPosition.x;
            Chunk *neighborChunk = world_->getChunkByChunkPosition(chunkPosition);
            if (neighborChunk != nullptr) neighborChunk->update();
        }
        if (position.y == 0)
        {
            glm::ivec3 chunkPosition = chunkPosition_;
            --chunkPosition.y;
            Chunk *neighborChunk = world_->getChunkByChunkPosition(chunkPosition);
            if (neighborChunk != nullptr) neighborChunk->update();
        } else if (position.y == CHUNK_SIZE - 1)
        {
            glm::ivec3 chunkPosition = chunkPosition_;
            ++chunkPosition.y;
            Chunk *neighborChunk = world_->getChunkByChunkPosition(chunkPosition);
            if (neighborChunk != nullptr) neighborChunk->update();
        }
        if (position.z == 0)
        {
            glm::ivec3 chunkPosition = chunkPosition_;
            --chunkPosition.z;
            Chunk *neighborChunk = world_->getChunkByChunkPosition(chunkPosition);
            if (neighborChunk != nullptr) neighborChunk->update();
        } else if (position.z == CHUNK_SIZE - 1)
        {
            glm::ivec3 chunkPosition = chunkPosition_;
            ++chunkPosition.z;
            Chunk *neighborChunk = world_->getChunkByChunkPosition(chunkPosition);
            if (neighborChunk != nullptr) neighborChunk->update();
        }
    }
}

const Bloc &Chunk::getBloc(const glm::ivec3 &position) const
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

void Chunk::unload()
{
    if (!empty_ && isLoaded())
        model_.reset();
}

bool Chunk::isEmpty()
{
    return empty_;
}
