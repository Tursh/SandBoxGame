//
// Created by tursh on 5/2/19.
//

#define GLM_FORCE_ONLY_XYZW

#include <glm/glm.hpp>

#include <World/Terrain/Block.h>
#include <Utils/Log.h>
#include <glm/gtx/string_cast.hpp>
#include <algorithm>
#include <CLucene.h>
#include <CLucene/search/PhraseQuery.h>
#include <iterator>

using namespace CGE::Loader;

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
                    //BOTTOM
                    {CUBE_SIZE, 0,         CUBE_SIZE},
                    {CUBE_SIZE, 0,         0},
                    {0,         0,         0},
                    {0,         0,         CUBE_SIZE},
                    //TOP
                    {CUBE_SIZE, CUBE_SIZE, CUBE_SIZE},
                    {CUBE_SIZE, CUBE_SIZE, 0},
                    {0,         CUBE_SIZE, 0},
                    {0,         CUBE_SIZE, CUBE_SIZE},
                    //LEFT
                    {0,         0,         CUBE_SIZE},
                    {0,         0,         0},
                    {0,         CUBE_SIZE, 0},
                    {0,         CUBE_SIZE, CUBE_SIZE},
                    //RIGHT
                    {CUBE_SIZE, 0,         CUBE_SIZE},
                    {CUBE_SIZE, 0,         0},
                    {CUBE_SIZE, CUBE_SIZE, 0},
                    {CUBE_SIZE, CUBE_SIZE, CUBE_SIZE},
                    //BACK
                    {CUBE_SIZE, 0,         0},
                    {0,         0,         0},
                    {0,         CUBE_SIZE, 0},
                    {CUBE_SIZE, CUBE_SIZE, 0},
                    //FRONT
                    {CUBE_SIZE, 0,         CUBE_SIZE},
                    {0,         0,         CUBE_SIZE},
                    {0,         CUBE_SIZE, CUBE_SIZE},
                    {CUBE_SIZE, CUBE_SIZE, CUBE_SIZE},
            };

    const glm::vec3 CUBE_VERTEX_POSITIONS[] =
            {
                    {0,         0,         0},      //xnzn
                    {0,         0,         CUBE_SIZE},  //xnzp
                    {CUBE_SIZE, 0,         0},          //xpzn
                    {CUBE_SIZE, 0,         CUBE_SIZE},      //xpzp

                    {0,         CUBE_SIZE, 0},          //xnzn
                    {0,         CUBE_SIZE, CUBE_SIZE},              //xpzn
                    {CUBE_SIZE, CUBE_SIZE, 0},      //xnzp
                    {CUBE_SIZE, CUBE_SIZE, CUBE_SIZE}           //xpzp
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
                    {4, 3, 0, 5, 4, 1},
                    {2, 4, 5, 0, 3, 4},
                    {2, 3, 4, 4, 5, 0},
                    {2, 4, 3, 4, 1, 5}
            };

    const unsigned int BOTTOM_FACE_2[4][3] =
            {
                    {0, 1, 3},
                    {2, 0, 3},
                    {0, 2, 3},
                    {2, 1, 3}
            };

    std::tuple<const glm::vec3 *, const unsigned int *> getFace(Face face)
    {
        int indicesOffset = (face == RIGHT || face == TOP || face == BACK) ? INDICES_PER_FACE : 0;
        return std::make_tuple(CUBE_FACE_VERTICES + face * VERTICES_PER_FACE, CUBE_FACE_INDICES + indicesOffset);
    }

    static void
    loadFace(MeshBuilder &meshBuilder, glm::ivec3 &blockPosition, Face face, glm::vec4 blockTexCoordsOffset)
    {

        //Get the face vertices
        const glm::vec3 *faceVertices = CUBE_FACE_VERTICES + face * VERTICES_PER_FACE;

        //Get the texCoords
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

        //Load the vertices
        unsigned int firstVertex = meshBuilder.loadVertices(faceVertices, texCoordsBuf, nullptr,
                                                            VERTICES_PER_FACE);

        //Translate the vertices to the block position
        meshBuilder.translateVertices(blockPosition.operator*=(CUBE_SIZE), firstVertex);

        //Get the face indices
        const unsigned int *faceIndices =
                CUBE_FACE_INDICES + ((face == RIGHT || face == TOP || face == BACK) ? INDICES_PER_FACE : 0);

        //Load them
        unsigned int firstIndex = meshBuilder.loadIndices({faceIndices, INDICES_PER_FACE});

        //Translate them to the face vertices
        meshBuilder.incrementIndices(firstVertex, firstIndex);

    }

    /**
     * Load half a face
     * @param positions position vector
     * @param texCoords texture coordinates vector
     * @param indices indices vector
     * @param blockPosition position of the bloc in the chunk
     * @param face Face to load
     * @param axis The axis in which the face is cut (x = 0, y = 1, z = 2, down / up = axis + 0B0000u00 -> u = 0 / 1
     * @param blockTexCoordsOffset block texture coordinates offset in texture atlas
     */
    static void
    loadMidFace(MeshBuilder &meshBuilder, glm::ivec3 &blockPosition, Face face, char axis,
                glm::vec4 blockTexCoordsOffset)
    {
        //Get the face indices
        const unsigned int *faceIndices =
                CUBE_FACE_INDICES + ((face == RIGHT || face == TOP || face == BACK) ? INDICES_PER_FACE : 0);

        //Load them
        unsigned int firstIndex = meshBuilder.loadIndices({faceIndices, INDICES_PER_FACE});

        //Translate them to the face vertices
        meshBuilder.incrementIndices(meshBuilder.vertexCount(), firstIndex);

        //The mid face must be drawn on the positive of negative part of the axis
        bool up = axis >> 2;
        //Get only the axis
        axis &= 0B00000011;

        //Get the face positions
        const glm::vec3 *faceVertices = CUBE_FACE_VERTICES + face * VERTICES_PER_FACE;

        //Get the face texCoords
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

        //Load the vertices
        unsigned int firstVertex = meshBuilder.loadVertices(faceVertices, texCoordsBuf, nullptr, VERTICES_PER_FACE);

        meshBuilder.transformVertices([&](glm::vec3 &position, glm::vec2 &texCoords, glm::vec3 &normal){
            if((position[axis] < 0.0001f) ^ !up) position[axis] = CUBE_SIZE / 2;
        }, firstVertex);

        //Translate the positions to the block position
        meshBuilder.translateVertices(blockPosition.operator*=(CUBE_SIZE), firstVertex);

    }

    static void
    loadTriangle(MeshBuilder &meshBuilder,
                 glm::ivec3 &blockPosition,
                 glm::vec3 *triangleVertexPositions, glm::vec4 &texCoordsOffset, bool invIndices = false)
    {
        const unsigned int *indices = TRIANGLE_INDICES + invIndices * VERTICES_PER_TRIANGLE;
        unsigned int firstIndexPosition = meshBuilder.loadIndices({indices, VERTICES_PER_TRIANGLE});
        meshBuilder.incrementIndices(meshBuilder.vertexCount(), firstIndexPosition);

        glm::vec2 texSize = {texCoordsOffset.z - texCoordsOffset.x, texCoordsOffset.w - texCoordsOffset.y};

        int neutralAxis = -1;

        //Find what axis is not moved
        for (int axis = 0; axis < 3; ++axis)
            if (fabsf(triangleVertexPositions[0][axis] - triangleVertexPositions[1][axis] +
                      triangleVertexPositions[0][axis] -
                      triangleVertexPositions[2][axis]) < 0.001f)
            {
                neutralAxis = axis;
                break;
            }

        glm::vec2 texCoordsBuf[VERTICES_PER_TRIANGLE] =
                {
                        {texCoordsOffset.x +
                         (neutralAxis == 0 ? triangleVertexPositions[0].y : triangleVertexPositions[0].x) / CUBE_SIZE *
                         texSize.x,
                                texCoordsOffset.y +
                                (neutralAxis == 2 ? triangleVertexPositions[0].y : triangleVertexPositions[0].z) /
                                CUBE_SIZE * texSize.y},
                        {texCoordsOffset.x +
                         (neutralAxis == 0 ? triangleVertexPositions[1].y : triangleVertexPositions[1].x) / CUBE_SIZE *
                         texSize.x,
                                texCoordsOffset.y +
                                (neutralAxis == 2 ? triangleVertexPositions[1].y : triangleVertexPositions[1].z) /
                                CUBE_SIZE * texSize.y},
                        {texCoordsOffset.x +
                         (neutralAxis == 0 ? triangleVertexPositions[2].y : triangleVertexPositions[2].x) / CUBE_SIZE *
                         texSize.x,
                                texCoordsOffset.y +
                                (neutralAxis == 2 ? triangleVertexPositions[2].y : triangleVertexPositions[2].z) /
                                CUBE_SIZE * texSize.y},
                };

        unsigned int firstVertex = meshBuilder.loadTriangle(triangleVertexPositions, texCoordsBuf, nullptr, invIndices);

        meshBuilder.translateVertices(blockPosition, firstVertex);
    }


    static unsigned int
    loadVertex(MeshBuilder &meshBuilder, glm::vec3 &vertexPosition,
               glm::ivec3 &blockPosition, glm::vec4 &texCoordsOffset)
    {
        glm::vec2 texSize = {texCoordsOffset.z - texCoordsOffset.x, texCoordsOffset.w - texCoordsOffset.y};
        glm::vec2 vertexTexCoord =
                {texCoordsOffset.x + vertexPosition.x / CUBE_SIZE * texSize.x,
                 texCoordsOffset.y + vertexPosition.z / CUBE_SIZE * texSize.y};

        return meshBuilder.loadVertex(vertexPosition + ((glm::vec3) (blockPosition) * CUBE_SIZE), vertexTexCoord);
    }

    void
    loadBlock(CGE::Loader::MeshBuilder &meshBuilder,
              glm::ivec3 &blockPosition, Block *blockToLoad, const Block **neighbors, glm::vec4 &texCoordsOffset)
    {

        unsigned char &shape = blockToLoad->state;
        //Get flags
        bool xnzn = shape & 1;
        bool xnzp = (shape >> 1) & 1;
        bool xpzn = (shape >> 2) & 1;
        bool xpzp = (shape >> 3) & 1;
        bool midX = (shape >> 4) & 1;
        bool midY = (shape >> 5) & 1;
        bool midZ = (shape >> 6) & 1;
        bool invY = (shape >> 7) & 1;

        bool corners[] = {xnzn, xnzp, xpzn, xpzp};

        int cornerFlagCount = xpzp + xnzp + xpzn + xnzn;
        int midCount = midX + midZ; //We don't count midY because of the way the block are made

        //If there are no corner up and the midY flag is down, then it should be a air block
        if (cornerFlagCount == 4 && !midY)
        {
            blockToLoad->ID = Blocks::AIR;
            blockToLoad->state = 0;
            return;
        }

        bool xnd = xnzn && xnzp, znd = xnzn && xpzn, xnu = !xnzn && !xnzp, znu = !xnzn && !xpzn, xzd = xnzn && xpzp;

        unsigned int startPosition = meshBuilder.vertexCount(), startBlockIndex = meshBuilder.indexCount();

        //Bottom
        if (neighbors[2 + invY] != nullptr && neighbors[2 + invY]->ID == Blocks::AIR)
        {
            if (midY)
                loadFace(meshBuilder, blockPosition, BOTTOM, texCoordsOffset); //Load face bottom 0
            else if (midCount == 0)
            {
                if (cornerFlagCount <= 2)
                    loadFace(meshBuilder, blockPosition, BOTTOM,
                             texCoordsOffset); //Load face bottom 0
                else
                {
                    //Load face bottom 1
                    glm::vec3 triangleVertexPositions[VERTICES_PER_TRIANGLE];
                    int i = 0;
                    for (int point = 0; point < 4; ++point)
                    {
                        if (!corners[point] || corners[point] ^ !corners[point ^ 3])
                        {
                            std::copy(CUBE_VERTEX_POSITIONS + point,
                                      CUBE_VERTEX_POSITIONS + (point + 1),
                                      triangleVertexPositions + i++);
                        }
                    }
                    loadTriangle(meshBuilder, blockPosition,
                                 triangleVertexPositions, texCoordsOffset, xnd);
                }
            }
            else
            {
                //If there is only one corner up, face bottom 1 has to be drawn
                if (cornerFlagCount == 1)
                {
                    unsigned int startVertex = meshBuilder.vertexCount();

                    //Load regular isoscele triangle
                    {
                        glm::vec3 triangleVertexPositions[VERTICES_PER_TRIANGLE];
                        int i = 0;
                        for (int point = 0; point < 4; ++point)
                        {
                            if (!corners[point] || corners[point] ^ !corners[point ^ 3])
                            {
                                std::copy(CUBE_VERTEX_POSITIONS + point,
                                          CUBE_VERTEX_POSITIONS + (point + 1),
                                          triangleVertexPositions + i);
                                ++i;
                            }
                        }
                        loadTriangle(meshBuilder, blockPosition,
                                     triangleVertexPositions, texCoordsOffset, !xnu);
                    }

                    //If there is only on mid flag, load face bottom 2
                    if (midCount == 1)
                    {
                        //Add the missing vertex
                        glm::vec3 vertexPosition = {midX ? CUBE_SIZE / 2 : xnu ? CUBE_SIZE : 0, 0,
                                                    midZ ? CUBE_SIZE / 2 : znu ? CUBE_SIZE : 0};
                        loadVertex(meshBuilder, vertexPosition, blockPosition, texCoordsOffset);

                        //Create new triangle from existing positions
                        const unsigned int *triangleIndices = BOTTOM_FACE_2[xnu * 2 + znu];
                        unsigned int startIndex = meshBuilder.loadIndices({triangleIndices, 3});
                        meshBuilder.incrementIndices(startVertex, startIndex);
                    }

                        //If the 2 mid flags are up, load face bottom 3 (stair face)
                    else if (midCount == 2)
                    {
                        //We have to add 3 vertex to add 2 triangles
                        glm::vec3 vertex1Position = {xnu ? CUBE_SIZE : 0, 0, 0.5f};
                        loadVertex(meshBuilder, vertex1Position, blockPosition, texCoordsOffset);

                        glm::vec3 vertex2Position = {CUBE_SIZE / 2, 0, CUBE_SIZE / 2};
                        loadVertex(meshBuilder, vertex2Position, blockPosition, texCoordsOffset);

                        glm::vec3 vertex3Position = {0.5f, 0, znu ? CUBE_SIZE : 0};
                        loadVertex(meshBuilder, vertex3Position, blockPosition, texCoordsOffset);

                        const unsigned int *stairIndices = STAIR_INDICES[xnu * 2 + znu];
                        unsigned int startIndex = meshBuilder.loadIndices({stairIndices, VERTICES_PER_TRIANGLE * 2});
                        meshBuilder.incrementIndices(startVertex, startIndex);
                    }
                }
                else if (cornerFlagCount == 2)
                {
                    loadMidFace(meshBuilder, blockPosition, Face::BOTTOM,
                                (midZ ? 2 : 0) + (xnd || znd ? 4 : 0), texCoordsOffset);
                }
                    //Else the cornerFlagCount = 1
                else
                {

                }
            }
        }

        //Block side faces
        for (unsigned char side = 0; side < 4; ++side)
            if (neighbors[side + (side >> 1) * 2] != nullptr && (neighbors[side + (side >> 1) * 2]->ID == Blocks::AIR
                                                                 || neighbors[side + (side >> 1) * 2]->state))
            {
                //Get positive and negative borders
                bool
                        n = !(side & 1 ? side >> 1 ? xnzp : xpzn : xnzn),
                        p = !(side & 1 ? xpzp : side >> 1 ? xpzn : xnzp);

                bool zSide = side / 2;

                //If the 2 corners are up than draw a full face
                if (n && p)
                {
                    if (midY)
                        loadMidFace(meshBuilder, blockPosition, static_cast<Face>(2 + side), 1,
                                    texCoordsOffset);
                    else
                        loadFace(meshBuilder, blockPosition, static_cast<Face>(2 + side),
                                 texCoordsOffset);
                }
                    //If there is only 1 corner
                else if (n || p)
                {
                    unsigned int startIndex = meshBuilder.vertexCount();
                    if (midCount == 0)
                    {


                        bool pairSideIndex = !(side % 2), extSideIndex = !((side & 1) ^ (side >> 1));
                        //We have to add 3 vertex to add side triangle
                        glm::vec3 triangleVertexPositions[] = {
                                {pairSideIndex ? 0 : CUBE_SIZE,                                 0,
                                        extSideIndex ? CUBE_SIZE : 0},

                                {extSideIndex ? 0 : CUBE_SIZE,                                  0,
                                        pairSideIndex ? 0 : CUBE_SIZE},

                                {zSide ? (p ? CUBE_SIZE : 0) : (pairSideIndex ? 0 : CUBE_SIZE), CUBE_SIZE,
                                        zSide ? (pairSideIndex ? 0 : CUBE_SIZE) : (p ? CUBE_SIZE : 0)}
                        };
                        loadTriangle(meshBuilder, blockPosition, triangleVertexPositions,
                                     texCoordsOffset);
                    }

                        //If there is a mid flag draw vertical rectangle
                    else if ((midX && zSide) || (midZ && !zSide))
                    {
                        loadMidFace(meshBuilder, blockPosition, static_cast<Face>(2 + side),
                                    (!(zSide && midX) ? 2 : 0) + (p ? 4 : 0), texCoordsOffset);
                    }
                    if (midY)
                    {
                        meshBuilder.transformVertices(
                                [&](glm::vec3 &position, glm::vec2 &texCoord, glm::vec3 &normal)
                                {
                                    position.y -= (position.y - blockPosition.y < 0.01f) ? 0.0f : 0.5f;
                                }, startIndex);
                    }
                }

            }

        unsigned int startTopFaceIndex = meshBuilder.vertexCount();

        //TOP
        if ((neighbors[3 - invY] == nullptr || neighbors[3 - invY]->ID == Blocks::AIR || neighbors[3 - invY]->state))
        {
            if (!cornerFlagCount)
                loadFace(meshBuilder, blockPosition, Face::TOP, texCoordsOffset);
            else if (!midCount)
            {
                if (cornerFlagCount == 1)
                {
                    glm::vec3 triangleVertexPositions[VERTICES_PER_TRIANGLE];
                    int i = 0;
                    for (int point = 0; point < 4; ++point)
                    {
                        if (!corners[point])
                        {
                            std::copy(CUBE_VERTEX_POSITIONS + 4 + point,
                                      CUBE_VERTEX_POSITIONS + 4 + (point + 1),
                                      triangleVertexPositions + i);
                            ++i;
                        }
                    }
                    loadTriangle(meshBuilder, blockPosition,
                                 triangleVertexPositions, texCoordsOffset, xnu);
                }
            }
            else
            {
                if (cornerFlagCount == 1)
                {
                    unsigned int startVertex = meshBuilder.vertexCount();

                    {
                        glm::vec3 triangleVertexPositions[VERTICES_PER_TRIANGLE];
                        int i = 0;
                        for (int point = 0; point < 4; ++point)
                        {
                            if (!corners[point])
                            {
                                std::copy(CUBE_VERTEX_POSITIONS + 4 + point,
                                          CUBE_VERTEX_POSITIONS + 4 + (point + 1),
                                          triangleVertexPositions + i);
                                ++i;
                            }
                        }
                        loadTriangle(meshBuilder, blockPosition,
                                     triangleVertexPositions, texCoordsOffset, xnu);
                    }
                    //If there is only on mid flag, load face bottom 2
                    if (midCount == 1)
                    {
                        //Add the missing vertex
                        glm::vec3 vertexPosition = {midX ? CUBE_SIZE / 2 : xnu ? CUBE_SIZE : 0, CUBE_SIZE,
                                                    midZ ? CUBE_SIZE / 2 : znu ? CUBE_SIZE : 0};
                        loadVertex(meshBuilder, vertexPosition, blockPosition, texCoordsOffset);

                        const unsigned int *triangleIndices = BOTTOM_FACE_2[xnu * 2 + znu];

                        //Create new triangle from existing positions
                        unsigned int startIndex = meshBuilder.loadIndices({triangleIndices, 3}, true);
                        meshBuilder.incrementIndices(startVertex, startIndex);
                    }
                    else if (midCount == 2)
                    {

                        //We have to add 3 vertex to add 2 triangles
                        glm::vec3 vertex1Position = {xnu ? CUBE_SIZE : 0, CUBE_SIZE, 0.5f};
                        loadVertex(meshBuilder, vertex1Position, blockPosition, texCoordsOffset);

                        glm::vec3 vertex2Position = {CUBE_SIZE / 2, CUBE_SIZE, CUBE_SIZE / 2};
                        loadVertex(meshBuilder, vertex2Position, blockPosition, texCoordsOffset);

                        glm::vec3 vertex3Position = {0.5f, CUBE_SIZE, znu ? CUBE_SIZE : 0};
                        loadVertex(meshBuilder, vertex3Position, blockPosition, texCoordsOffset);

                        const unsigned int *stairIndices = STAIR_INDICES[xnu * 2 + znu];
                        unsigned int startIndex = meshBuilder.loadIndices({stairIndices, VERTICES_PER_TRIANGLE * 2},
                                                                          true);
                        meshBuilder.incrementIndices(startVertex, startIndex);
                    }
                    else
                    {

                    }
                }
                else if (cornerFlagCount == 2)
                {
                    loadMidFace(meshBuilder, blockPosition, Face::TOP,
                                (midZ ? 2 : 0) + (xnd || znd ? 4 : 0), texCoordsOffset);
                }
            }
        }

        //Inside faces
        if (cornerFlagCount)
        {
            int startVertex = meshBuilder.vertexCount();
            if (midCount)
            {
                if (cornerFlagCount == 1)
                {
                    int corner = 0;
                    for (; corner < 4; ++corner)
                        if (corners[corner])
                            break;

                    if (midCount == 1)
                    {
                        glm::vec3 triangleVertexPositions[VERTICES_PER_FACE] = {
                                {xnu ^ midZ ? CUBE_SIZE : 0,                             0,
                                        !znu ^ midZ ? CUBE_SIZE : 0},
                                {midX ? CUBE_SIZE / 2 : corner / 2 == 1 ? CUBE_SIZE : 0, 0,
                                        midZ ? CUBE_SIZE / 2 : (xnzp || xpzp) ? CUBE_SIZE : 0},
                                {xnu ^ midZ ? CUBE_SIZE : 0,                             CUBE_SIZE,
                                        !znu ^ midZ ? CUBE_SIZE : 0},
                                {midX ? CUBE_SIZE / 2 : corner / 2 == 1 ? CUBE_SIZE : 0, CUBE_SIZE,
                                        midZ ? CUBE_SIZE / 2 : (xnzp || xpzp) ? CUBE_SIZE : 0},

                        };

                        loadTriangle(meshBuilder, blockPosition,
                                     triangleVertexPositions, texCoordsOffset, !(xnzn || xpzp) ^ midZ);
                        loadTriangle(meshBuilder, blockPosition,
                                     triangleVertexPositions + 1, texCoordsOffset, (xnzn || xpzp) ^ midZ);
                    }
                    else
                    {
                        if (cornerFlagCount == 1)
                        {


                            loadMidFace(meshBuilder, blockPosition,
                                        (Face) (2 + xnu), 2 + (znu ? 4 : 0), texCoordsOffset);

                            meshBuilder.transformVertices(
                                    [&](glm::vec3 &position, glm::vec2 &texCoords, glm::vec3 &normal)
                                    {
                                        position.x += 0.5f * (xnu ? -1 : 1);
                                    }, startVertex);

                            loadMidFace(meshBuilder, blockPosition,
                                        (Face) (4 + znu), xnu ? 4 : 0, texCoordsOffset);

                            meshBuilder.transformVertices(
                                    [&](glm::vec3 &position, glm::vec2 &texCoords, glm::vec3 &normal)
                                    {
                                        position.z += 0.5f * (znu ? -1 : 1);
                                    }, startVertex);
                        }
                    }
                }
                else if (cornerFlagCount == 2)
                {
                    if (midCount == 1)
                    {
                        int i = meshBuilder.vertexCount();
                        loadFace(meshBuilder, blockPosition, (Face) (2 + xnu + znu * 3 + znd * 2),
                                 texCoordsOffset);
                        meshBuilder.transformVertices(
                                [&](glm::vec3 &position, glm::vec2 &texCoords, glm::vec3 &normal)
                                {
                                    position[xnu || xnd ? 0 : 2] += (xnu || znu ? -1 : 1) * 0.5f;
                                }, i);
                    }
                }
            }
            else
            {
                if (cornerFlagCount == 1)
                {
                    int corner = 0;
                    for (; corner < 4; ++corner)
                        if (corners[corner])
                            break;

                    glm::vec3 triangleVertexPositions[VERTICES_PER_TRIANGLE] = {
                            {(corner >> 1) & 1 ? CUBE_SIZE : 0, 0,         corner & 1 ? CUBE_SIZE : 0},
                            {(corner >> 1) & 1 ? 0 : CUBE_SIZE, CUBE_SIZE, corner & 1 ? CUBE_SIZE : 0},
                            {(corner >> 1) & 1 ? CUBE_SIZE : 0, CUBE_SIZE, corner & 1 ? 0 : CUBE_SIZE}
                    };
                    loadTriangle(meshBuilder, blockPosition,
                                 triangleVertexPositions, texCoordsOffset, !(xnzn || xpzp));
                }
                else if (cornerFlagCount == 2)
                {
                    glm::vec3 triangleVertexPositions[VERTICES_PER_FACE];

                    for (int corner = 0; corner < 4; ++corner)
                        triangleVertexPositions[corner] = {(corner >> 1) & 1, corners[corner] ? 0 : CUBE_SIZE,
                                                           corner & 1};

                    loadTriangle(meshBuilder, blockPosition,
                                 triangleVertexPositions, texCoordsOffset, (xnzn || xpzp));
                    loadTriangle(meshBuilder, blockPosition,
                                 triangleVertexPositions + 1, texCoordsOffset, !(xnzn || xpzp));
                }

                else
                {
                    int corner = 0;
                    for (; corner < 4; ++corner)
                        if (!corners[corner])
                            break;

                    glm::vec3 triangleVertexPositions[VERTICES_PER_TRIANGLE] = {
                            {(corner >> 1) & 1 ? CUBE_SIZE : 0, CUBE_SIZE, corner & 1 ? CUBE_SIZE : 0},
                            {(corner >> 1) & 1 ? 0 : CUBE_SIZE, 0,         corner & 1 ? CUBE_SIZE : 0},
                            {(corner >> 1) & 1 ? CUBE_SIZE : 0, 0,         corner & 1 ? 0 : CUBE_SIZE}
                    };
                    loadTriangle(meshBuilder, blockPosition,
                                 triangleVertexPositions, texCoordsOffset, (!xnzp || !xpzn));
                }
            }
        }


        if (midY)
        {
            meshBuilder.transformVertices(
                    [&](glm::vec3 &position, glm::vec2 &texCoords, glm::vec3 &normal)
                    {
                        position.y -= (position.y - blockPosition.y < 0.01f) ? 0.0f : 0.5f;
                    }, startTopFaceIndex);
        }

        else if (invY)
        {
            meshBuilder.transformVertices(
                    [&](glm::vec3 &position, glm::vec2 &texCoords, glm::vec3 &normal)
                    {
                        float &y = position.y;
                        float yBase = y - blockPosition.y;
                        y = (fabsf(yBase) < 0.001f ? CUBE_SIZE : (fabsf(yBase - CUBE_SIZE) < 0.001f) ? 0 : yBase) +
                            blockPosition.y;
                    }, startPosition);

            meshBuilder.invertIndices(startBlockIndex);
        }
    }
}

bool Block::operator==(const Block &otherBlock) const { return ID == otherBlock.ID && state == otherBlock.state; }

bool Block::operator!=(const Block &otherBlock) const { return !(*this == otherBlock); }
