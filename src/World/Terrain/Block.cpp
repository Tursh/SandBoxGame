//
// Created by tursh on 5/2/19.
//

#define GLM_FORCE_ONLY_XYZW

#include <glm/glm.hpp>

#include <World/Terrain/Block.h>
#include <Utils/Log.h>
#include <glm/gtx/string_cast.hpp>

namespace Blocks
{
    const short
            AIR = 0,
            DIRT = 1;

    const Block AIR_BLOC = {AIR, 0};

    const float CUBE_SIZE = 1.0f;

    const unsigned int
    //POSITION_PER_VERTEX = 3,
    //TEX_COORDS_PER_VERTEX = 2,

            VERTICES_PER_TRIANGLE = 3,
    //POSITION_PER_TRIANGLE = VERTICES_PER_TRIANGLE * POSITION_PER_VERTEX,
    //TEX_COORDS_PER_TRIANGLE = VERTICES_PER_TRIANGLE * TEX_COORDS_PER_VERTEX,

            VERTICES_PER_FACE = 4,
    //POSITION_PER_FACE = VERTICES_PER_FACE,
    //TEX_COORDS_PER_FACE = TEX_COORDS_PER_VERTEX * VERTICES_PER_FACE,
            INDICES_PER_FACE = 2 * VERTICES_PER_TRIANGLE;

    const glm::vec3 CUBE_FACE_VERTICES[] =
            {
                    //TOP
                    {CUBE_SIZE, CUBE_SIZE, CUBE_SIZE},
                    {CUBE_SIZE, CUBE_SIZE, 0},
                    {0,         CUBE_SIZE, 0},
                    {0,         CUBE_SIZE, CUBE_SIZE},
                    //BOTTOM
                    {CUBE_SIZE, 0,         CUBE_SIZE},
                    {CUBE_SIZE, 0,         0},
                    {0,         0,         0},
                    {0,         0,         CUBE_SIZE},
                    //RIGHT
                    {CUBE_SIZE, 0,         CUBE_SIZE},
                    {CUBE_SIZE, 0,         0},
                    {CUBE_SIZE, CUBE_SIZE, 0},
                    {CUBE_SIZE, CUBE_SIZE, CUBE_SIZE},
                    //LEFT
                    {0,         0,         CUBE_SIZE},
                    {0,         0,         0},
                    {0,         CUBE_SIZE, 0},
                    {0,         CUBE_SIZE, CUBE_SIZE},
                    //FRONT
                    {CUBE_SIZE, 0,         CUBE_SIZE},
                    {0,         0,         CUBE_SIZE},
                    {0,         CUBE_SIZE, CUBE_SIZE},
                    {CUBE_SIZE, CUBE_SIZE, CUBE_SIZE},
                    //BACK
                    {CUBE_SIZE, 0,         0},
                    {0,         0,         0},
                    {0,         CUBE_SIZE, 0},
                    {CUBE_SIZE, CUBE_SIZE, 0},
            };

    const glm::vec3 CUBE_VERTEX_POSITIONS[] =
            {
                    {0,         0,         0},
                    {0,         0,         CUBE_SIZE},
                    {CUBE_SIZE, 0,         0},
                    {CUBE_SIZE, 0,         CUBE_SIZE},
                    {0,         CUBE_SIZE, 0},
                    {0,         CUBE_SIZE, CUBE_SIZE},
                    {CUBE_SIZE, CUBE_SIZE, 0},
                    {CUBE_SIZE, CUBE_SIZE, CUBE_SIZE}
            };

    const unsigned int CUBE_FACE_INDICES[] =
            {
                    0, 1, 2,
                    2, 3, 0,
                    //FOR RIGHT, TOP or BACK
                    0, 3, 2,
                    2, 1, 0
            };

    const unsigned int TRIANGLE_INDICES[] =
            {
                    0, 1, 2,
                    2, 1, 0
            };

    const unsigned int STAIR_INDICES[4][6] =
            {
                    {4, 3, 2, 5, 4, 1},
                    {2, 3, 4, 0, 4, 5},
                    {0, 3, 4, 2, 4, 5},
                    {4, 3, 0, 5, 4, 1}
            };

    std::tuple<const glm::vec3 *, const unsigned int *> getFace(Face face)
    {
        int indicesOffset = (face == RIGHT || face == TOP || face == BACK) ? INDICES_PER_FACE : 0;
        return std::make_tuple(CUBE_FACE_VERTICES + face * VERTICES_PER_FACE, CUBE_FACE_INDICES + indicesOffset);
    }

    static void
    loadFace(std::vector<glm::vec3> &positions, std::vector<glm::vec2> &texCoords, std::vector<unsigned int> &indices,
             glm::ivec3 &blockPosition, Face face, glm::vec4 blockTexCoordsOffset)
    {
        const unsigned int *faceIndices =
                CUBE_FACE_INDICES + ((face == RIGHT || face == TOP || face == BACK) ? INDICES_PER_FACE : 0);

        indices.insert(indices.end(), faceIndices, faceIndices + INDICES_PER_FACE);
        for (unsigned int i = indices.size() - INDICES_PER_FACE; i < (unsigned int) indices.size(); ++i)
            indices[i] += positions.size();

        const glm::vec3 *faceVertices = CUBE_FACE_VERTICES + face * VERTICES_PER_FACE;
        positions.insert(positions.end(), faceVertices, faceVertices + VERTICES_PER_FACE);
        for (unsigned int i = positions.size() - VERTICES_PER_FACE; i < (unsigned int) positions.size(); ++i)
            positions[i] += (blockPosition.operator*=(CUBE_SIZE));

        glm::vec2 texCoordsBuf[VERTICES_PER_FACE] =
                {
                        {blockTexCoordsOffset.z,
                                blockTexCoordsOffset.w},
                        {blockTexCoordsOffset.x,
                                blockTexCoordsOffset.w},
                        {blockTexCoordsOffset.x,
                                blockTexCoordsOffset.y},
                        {blockTexCoordsOffset.z,
                                blockTexCoordsOffset.y},
                };

        texCoords.insert(texCoords.end(), texCoordsBuf, texCoordsBuf + VERTICES_PER_FACE);
    }

    static void
    loadTriangle(std::vector<glm::vec3> &positions, std::vector<glm::vec2> &texCoords,
                 std::vector<unsigned int> &indices,
                 glm::ivec3 &blockPosition,
                 glm::vec3 *triangleVertexPositions, glm::vec4 &texCoordsOffset, bool invIndices = false)
    {
        indices.insert(indices.end(),
                       TRIANGLE_INDICES + invIndices * VERTICES_PER_TRIANGLE,
                       TRIANGLE_INDICES + invIndices * VERTICES_PER_TRIANGLE + VERTICES_PER_TRIANGLE);
        for (unsigned int i = indices.size() - VERTICES_PER_TRIANGLE; i < (unsigned int) indices.size(); ++i)
            indices[i] += positions.size();

        positions.insert(positions.end(), triangleVertexPositions, triangleVertexPositions + VERTICES_PER_TRIANGLE);
        for (unsigned int i = positions.size() - VERTICES_PER_TRIANGLE; i < (unsigned int) positions.size(); ++i)
            positions[i] += blockPosition;

        glm::vec2 texSize = {texCoordsOffset.z - texCoordsOffset.x, texCoordsOffset.w - texCoordsOffset.y};

        glm::vec2 texCoordsBuf[VERTICES_PER_TRIANGLE] =
                {
                        {texCoordsOffset.x + triangleVertexPositions[0].x / CUBE_SIZE * texSize.x,
                                texCoordsOffset.y + triangleVertexPositions[0].z / CUBE_SIZE * texSize.y},
                        {texCoordsOffset.x + triangleVertexPositions[1].x / CUBE_SIZE * texSize.x,
                                texCoordsOffset.y + triangleVertexPositions[1].z / CUBE_SIZE * texSize.y},
                        {texCoordsOffset.x + triangleVertexPositions[2].x / CUBE_SIZE * texSize.x,
                                texCoordsOffset.y + triangleVertexPositions[2].z / CUBE_SIZE * texSize.y},
                };

        texCoords.insert(texCoords.end(), texCoordsBuf, texCoordsBuf + VERTICES_PER_TRIANGLE);
        for (int i = indices.size() - 3; i < indices.size(); ++i)
            logInfo(indices[i] << " - " << glm::to_string(positions[indices[i]]));
        std::cout << std::endl;
    }


    static unsigned int
    loadVertex(std::vector<glm::vec3> &positions, std::vector<glm::vec2> &texCoords, glm::vec3 &vertexPosition,
               glm::ivec3 &blockPosition, glm::vec4 &texCoordsOffset)
    {
        glm::vec2 texSize = {texCoordsOffset.z - texCoordsOffset.x, texCoordsOffset.w - texCoordsOffset.y};

        glm::vec2 vertexTexCoord =
                {texCoordsOffset.x + vertexPosition.x / CUBE_SIZE * texSize.x,
                 texCoordsOffset.y + vertexPosition.z / CUBE_SIZE * texSize.y};
        texCoords.push_back(vertexTexCoord);

        positions.push_back(vertexPosition + ((glm::vec3) (blockPosition) * CUBE_SIZE));

        return positions.size() - 1;
    }


    void
    loadBlock(std::vector<glm::vec3> &positions, std::vector<glm::vec2> &texCoords, std::vector<unsigned int> &indices,
              glm::ivec3 &blockPosition, Block *blockToLoad, const Block **neighbors, glm::vec4 &texCoordsOffset)
    {

        char &shape = blockToLoad->state;
        //Get flags
        bool xnzn = shape & 1;
        bool xnzp = (shape >> 1) & 1;
        bool xpzn = (shape >> 2) & 1;
        bool xpzp = (shape >> 3) & 1;
        bool midX = (shape >> 4) & 1;
        bool midY = (shape >> 5) & 1;
        bool midZ = (shape >> 6) & 1;
        bool invY = (shape >> 7) & 1;

        int cornerFlagCount = xpzp + xnzp + xpzn + xnzn;
        int midCount = midX + midZ; //We don't count midY because of the way the block are made

        //If there are no corner up and the midY flag is down, then it should be a air block
        if (cornerFlagCount == 4 && !midY)
        {
            blockToLoad->ID = Blocks::AIR;
            blockToLoad->state = 0;
            return;
        }

        bool xnd = xnzn && xnzp, znd = xnzn && xpzn, xnu = !xnzn && !xnzp, znu = !xnzn && !xpzn;

        //Bottom
        if (neighbors[2 + invY] == nullptr || neighbors[2 + invY]->ID == Blocks::AIR)
        {
            if (midY)
                loadFace(positions, texCoords, indices, blockPosition, BOTTOM, texCoordsOffset); //Load face bottom 0
            else if (midCount == 0)
            {
                if (cornerFlagCount <= 2)
                    loadFace(positions, texCoords, indices, blockPosition, BOTTOM,
                             texCoordsOffset); //Load face bottom 0
                else
                {
                    //Load face bottom 1
                    glm::vec3 triangleVertexPositions[VERTICES_PER_TRIANGLE];
                    int i = 0;
                    for (int point = 0; point < 4; ++point)
                    {
                        if ((shape >> (3 - point)) & 1)
                        {
                            std::copy(CUBE_VERTEX_POSITIONS + point,
                                      CUBE_VERTEX_POSITIONS + (point + 1),
                                      triangleVertexPositions + i);
                            ++i;
                        }
                    }
                    loadTriangle(positions, texCoords, indices, blockPosition,
                                 triangleVertexPositions, texCoordsOffset, xnd);
                }
            } else
            {
                //If there is only one corner up, face bottom 1 has to be drawn
                if (cornerFlagCount == 1)
                {
                    unsigned int startIndex = positions.size();
                    {
                        glm::vec3 triangleVertexPositions[VERTICES_PER_TRIANGLE];
                        int i = 0;
                        for (int point = 0; point < 4; ++point)
                        {
                            if (!((shape >> (3 - point)) & 1))
                            {
                                std::copy(CUBE_VERTEX_POSITIONS + point,
                                          CUBE_VERTEX_POSITIONS + (point + 1),
                                          triangleVertexPositions + i);
                                ++i;
                            }
                        }
                        loadTriangle(positions, texCoords, indices, blockPosition,
                                     triangleVertexPositions, texCoordsOffset, xnu);
                    }
                    //If there is only on mid flag, load face bottom 2
                    if (midCount == 1)
                    {
                        //Add the missing vertex
                        glm::vec3 vertexPosition = {midX ? CUBE_SIZE / 2 : xnu ? 0 : CUBE_SIZE, 0,
                                                    midZ ? CUBE_SIZE / 2 : znu ? 0 : CUBE_SIZE};
                        loadVertex(positions, texCoords, vertexPosition, blockPosition, texCoordsOffset);

                        //Create new triangle from existing positions
                        for (int i = startIndex;
                             i < positions.size(); ++i)
                        {
                            //If the vertex is not the corner up, we use it to make the second triangle
                            if (!(((positions[i].x == blockPosition.x) ^ xnu)
                                  && ((positions[i].z == blockPosition.z) ^ znu)))
                                indices.push_back(i);
                        }
                        //If the corner up is a znd we need to flip the indices so it renders it in clockwise direction
                        if (!znu)
                            std::iter_swap(indices.end() - 3, indices.end() - 1);
                    }

                        //If the 2 mid flags are up, load face bottom 3 (stair face)
                    else if (midCount == 2)
                    {

                        //We have to add 3 vertex to add 2 triangles
                        glm::vec3 vertex1Position = {xnu ? 0 : CUBE_SIZE, 0, 0.5f};
                        loadVertex(positions, texCoords, vertex1Position, blockPosition, texCoordsOffset);

                        glm::vec3 vertex2Position = {CUBE_SIZE / 2, 0, CUBE_SIZE / 2};
                        loadVertex(positions, texCoords, vertex2Position, blockPosition, texCoordsOffset);

                        glm::vec3 vertex3Position = {0.5f, 0, znu ? 0 : CUBE_SIZE};
                        loadVertex(positions, texCoords, vertex3Position, blockPosition, texCoordsOffset);

                        const unsigned int *stairIndices = STAIR_INDICES[xnu * 2 + znu];
                        indices.insert(indices.end(), stairIndices, stairIndices + VERTICES_PER_TRIANGLE * 2);

                        for (int i = indices.size() - VERTICES_PER_TRIANGLE * 2; i < indices.size(); ++i)
                            indices[i] += startIndex;
                    }
                } else if (cornerFlagCount == 2)
                {

                }
                    //Else the cornerFlagCount = 1
                else
                {

                }
            }
        }

        //Block sides
        for (unsigned char side = 0; side < 4; ++side)
        {
            if (neighbors[side + (side >> 1) * 2] == nullptr || neighbors[side + (side >> 1) * 2])
            {
                //Get positive and negative borders
                bool
                        n = side & 1 ? side >> 1 ? xnzp : xpzn : xnzn,
                        p = side & 1 ? xpzp : side >> 1 ? xpzn : xnzp;

                if (!n && !p)
                    loadFace(positions, texCoords, indices, blockPosition, static_cast<Face>(2 + side),
                             texCoordsOffset);

            }
        }


    }

}

bool Block::operator==(const Block &otherBlock) const
{
    return ID == otherBlock.ID && state == otherBlock.state;
}

bool Block::operator!=(const Block &otherBlock) const
{
    return !(*this == otherBlock);
}
