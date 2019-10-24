//
// Created by tursh on 5/2/19.
//

#include <World/Terrain/Bloc.h>
#include <glm/gtc/type_ptr.hpp>

namespace Blocs
{
    const short
            AIR = 0,
            DIRT = 1;

    const Bloc AIR_BLOC = {AIR, 0};

    const float CUBE_SIZE = 1.0f;

    const unsigned int
            POSITION_PER_VERTEX = 3,
            TEX_COORDS_PER_VERTEX = 2,

            VERTICES_PER_TRIANGLE = 3,
            POSITION_PER_TRIANGLE = VERTICES_PER_TRIANGLE * POSITION_PER_VERTEX,
            TEX_COORDS_PER_TRIANGLE = VERTICES_PER_TRIANGLE * TEX_COORDS_PER_VERTEX,

            VERTICES_PER_FACE = 4,
            POSITION_PER_FACE = POSITION_PER_VERTEX * VERTICES_PER_FACE,
            TEX_COORDS_PER_FACE = TEX_COORDS_PER_VERTEX * VERTICES_PER_FACE,
            INDICES_PER_FACE = 2 * VERTICES_PER_TRIANGLE;

    const float CUBE_FACE_VERTICES[] =
            {
                    //TOP
                    CUBE_SIZE, CUBE_SIZE, CUBE_SIZE,
                    CUBE_SIZE, CUBE_SIZE, 0,
                    0, CUBE_SIZE, 0,
                    0, CUBE_SIZE, CUBE_SIZE,
                    //BOTTOM
                    CUBE_SIZE, 0, CUBE_SIZE,
                    CUBE_SIZE, 0, 0,
                    0, 0, 0,
                    0, 0, CUBE_SIZE,
                    //RIGHT
                    CUBE_SIZE, 0, CUBE_SIZE,
                    CUBE_SIZE, 0, 0,
                    CUBE_SIZE, CUBE_SIZE, 0,
                    CUBE_SIZE, CUBE_SIZE, CUBE_SIZE,
                    //LEFT
                    0, 0, CUBE_SIZE,
                    0, 0, 0,
                    0, CUBE_SIZE, 0,
                    0, CUBE_SIZE, CUBE_SIZE,
                    //FRONT
                    CUBE_SIZE, 0, CUBE_SIZE,
                    0, 0, CUBE_SIZE,
                    0, CUBE_SIZE, CUBE_SIZE,
                    CUBE_SIZE, CUBE_SIZE, CUBE_SIZE,
                    //BACK
                    CUBE_SIZE, 0, 0,
                    0, 0, 0,
                    0, CUBE_SIZE, 0,
                    CUBE_SIZE, CUBE_SIZE, 0,
            };

    const float CUBE_VERTEX_POSITIONS[] =
            {
                    0, 0, 0,
                    0, 0, CUBE_SIZE,
                    CUBE_SIZE, 0, 0,
                    CUBE_SIZE, 0, CUBE_SIZE,
                    0, CUBE_SIZE, 0,
                    0, CUBE_SIZE, CUBE_SIZE,
                    CUBE_SIZE, CUBE_SIZE, 0,
                    CUBE_SIZE, CUBE_SIZE, CUBE_SIZE
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

    std::tuple<const float *, const unsigned int *> getFace(Face face)
    {
        int indicesOffset = (face == RIGHT || face == TOP || face == BACK) ? INDICES_PER_FACE : 0;
        return std::make_tuple(CUBE_FACE_VERTICES + face * POSITION_PER_FACE, CUBE_FACE_INDICES + indicesOffset);
    }

    static void
    loadFace(std::vector<float> &positions, std::vector<float> &texCoords, std::vector<unsigned int> &indices,
             glm::ivec3 &blocPosition, Face face, glm::vec4 blocTexCoordsOffset)
    {
        const unsigned int *faceIndices =
                CUBE_FACE_INDICES + ((face == RIGHT || face == TOP || face == BACK) ? INDICES_PER_FACE : 0);

        indices.insert(indices.end(), faceIndices, faceIndices + INDICES_PER_FACE);
        for (unsigned int i = indices.size() - INDICES_PER_FACE - 1; i < (unsigned int) indices.size(); ++i)
            indices[i] += positions.size() / POSITION_PER_VERTEX;

        const float *faceVertices = CUBE_FACE_VERTICES + face * POSITION_PER_FACE;
        positions.insert(positions.end(), faceVertices, faceVertices + POSITION_PER_FACE);
        for (unsigned int i = positions.size() - POSITION_PER_FACE - 1; i < (unsigned int) positions.size(); ++i)
            positions[i] += CUBE_SIZE * blocPosition[i % POSITION_PER_VERTEX];

        float texCoordsBuf[TEX_COORDS_PER_FACE] =
                {
                        blocTexCoordsOffset.z,
                        blocTexCoordsOffset.w,
                        blocTexCoordsOffset.x,
                        blocTexCoordsOffset.w,
                        blocTexCoordsOffset.x,
                        blocTexCoordsOffset.y,
                        blocTexCoordsOffset.z,
                        blocTexCoordsOffset.y,
                };

        texCoords.insert(texCoords.end(), texCoordsBuf, texCoordsBuf + TEX_COORDS_PER_FACE);
    }

    static void
    loadTriangle(std::vector<float> &positions, std::vector<float> &texCoords, std::vector<unsigned int> &indices,
                 glm::ivec3 &blocPosition,
                 float *triangleVertexPositions, glm::vec4 &texCoordsOffset, bool invIndices = false)
    {
        indices.insert(indices.end(),
                       TRIANGLE_INDICES + invIndices * VERTICES_PER_TRIANGLE,
                       TRIANGLE_INDICES + invIndices * VERTICES_PER_TRIANGLE + VERTICES_PER_TRIANGLE);
        for (unsigned int i = indices.size() - 3; i < (unsigned int) indices.size(); ++i)
            indices[i] += positions.size() / POSITION_PER_VERTEX;

        positions.insert(positions.end(), triangleVertexPositions, triangleVertexPositions + POSITION_PER_TRIANGLE);
        for (unsigned int i = positions.size() - POSITION_PER_TRIANGLE; i < (unsigned int) positions.size(); ++i)
            positions[i] += blocPosition[i % POSITION_PER_VERTEX];

        glm::vec2 texSize = {texCoordsOffset.z - texCoordsOffset.x, texCoordsOffset.w - texCoordsOffset.y };

        float texCoordsBuf[TEX_COORDS_PER_TRIANGLE] =
                {
                        texCoordsOffset.x + triangleVertexPositions[0] / CUBE_SIZE * texSize.x,
                        texCoordsOffset.y + triangleVertexPositions[2] / CUBE_SIZE * texSize.y,
                        texCoordsOffset.x + triangleVertexPositions[3] / CUBE_SIZE * texSize.x,
                        texCoordsOffset.y + triangleVertexPositions[5] / CUBE_SIZE * texSize.y,
                        texCoordsOffset.x + triangleVertexPositions[6] / CUBE_SIZE * texSize.x,
                        texCoordsOffset.y + triangleVertexPositions[8] / CUBE_SIZE * texSize.y,
                };

        texCoords.insert(texCoords.end(), texCoordsBuf, texCoordsBuf + TEX_COORDS_PER_TRIANGLE);
    }

    void loadBloc(std::vector<float> &positions, std::vector<float> &texCoords, std::vector<unsigned int> &indices,
                  glm::ivec3 &blocPosition, Bloc *blocToLoad, const Bloc **neighbors, glm::vec4 &texCoordsOffset)
    {

        char &shape = blocToLoad->state;
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
        int midCount = midX + midZ; //We don't count midY because of the way the bloc are made

        bool xNeg = !xnzn || !xnzp, zNeg = !xnzn || !xpzn, xzSame = !xnzn || !xpzp;

        //If there are no corner up and the midY flag is down, then it should be a air bloc
        if (cornerFlagCount == 4 && !midY)
        {
            blocToLoad->ID = Blocs::AIR;
            blocToLoad->state = 0;
            return;
        }

        //Bottom
        if (neighbors[2 + invY] == nullptr || neighbors[2 + invY]->ID == Blocs::AIR)
        {
            if (midY)
                loadFace(positions, texCoords, indices, blocPosition, BOTTOM, texCoordsOffset); //Load face bottom 0
            else if (!midX && !midZ)
            {
                if (cornerFlagCount <= 2)
                    loadFace(positions, texCoords, indices, blocPosition, BOTTOM,
                             texCoordsOffset); //Load face bottom 0
                else
                {
                    //Load face bottom 1
                    float triangleVertexPositions[POSITION_PER_VERTEX * 3];
                    int i = 0;
                    for (int point = 0; point < 4; ++point)
                    {
                        if ((shape >> point) & 1)
                        {
                            std::copy(CUBE_VERTEX_POSITIONS + point * POSITION_PER_VERTEX,
                                      CUBE_VERTEX_POSITIONS + (point + 1) * POSITION_PER_VERTEX,
                                      triangleVertexPositions + i * POSITION_PER_VERTEX);
                            ++i;
                        }
                    }
                    loadTriangle(positions, texCoords, indices, blocPosition,
                            triangleVertexPositions, texCoordsOffset, xNeg);
                }
            } else
            {
                //If there is only one corner up, face bottom 1 has to be drawn
                if (cornerFlagCount == 3)
                {

                    float midFaceTriangleVertexPositions[POSITION_PER_VERTEX * 3];

                    for (int i = 0; i < 4; ++i)
                    {
                        //To make triangle drawn in clockwise direction
                        int point = xNeg ? i : 3 - i;

                        if ((shape >> point) & 1)
                            std::copy(CUBE_VERTEX_POSITIONS + point * POSITION_PER_VERTEX,
                                      CUBE_VERTEX_POSITIONS + (point + 1) * POSITION_PER_VERTEX,
                                      midFaceTriangleVertexPositions + point * POSITION_PER_VERTEX);
                    }
                    loadTriangle(positions, texCoords, indices, blocPosition, midFaceTriangleVertexPositions,
                                 texCoordsOffset);

                    //If there is only on mid flag, load face bottom 2
                    if (midCount == 1)
                    {
                        //Add the missing vertex
                        float newVertex[POSITION_PER_VERTEX];
                        newVertex[0] = blocPosition.x * CUBE_SIZE + (midX ? CUBE_SIZE / 2 : xNeg ? CUBE_SIZE : 0);
                        newVertex[1] = blocPosition.y * CUBE_SIZE;
                        newVertex[2] = blocPosition.z * CUBE_SIZE + (midZ ? CUBE_SIZE / 2 : zNeg ? CUBE_SIZE : 0);
                        positions.insert(positions.end(), newVertex, newVertex + POSITION_PER_VERTEX);

                        //Create new triangle from existing positions
                        unsigned int newIndices[VERTICES_PER_TRIANGLE];
                        for (int i = positions.size() / POSITION_PER_VERTEX - 5;
                             i < positions.size() / POSITION_PER_VERTEX; ++i)
                        {
                            //If the vertex is not the corner up, we use it to make the second triangle
                            if (!((positions[i * 3] == blocPosition.x && xNeg)
                                  && (positions[i * 3 + 2] == blocPosition.z && zNeg)))
                                indices.push_back(i);
                        }
                    }

                    //If the 2 mid flags are up, load face bottom 3 (stair face)
                    else if (midCount == 2)
                    {
                        
                    }
                }

            }
        }

    }

}

bool Bloc::operator==(const Bloc &otherBloc) const
{
    return ID == otherBloc.ID && state == otherBloc.state;
}

bool Bloc::operator!=(const Bloc &otherBloc) const
{
    return !(*this == otherBloc);
}
