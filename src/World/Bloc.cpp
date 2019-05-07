//
// Created by tursh on 5/2/19.
//

#include <World/Bloc.h>

namespace Blocs
{
    const short
            AIR = 0,
            DIRT = 1;

    const float CUBE_SIZE = 1.0f;

    const unsigned int FACE_VERTICES_COUNT = 4 * 3, FACE_INDICES_COUNT = 2 * 3;

    const float CUBE_VERTICES[] =
            {
                    //TOP
                    CUBE_SIZE, CUBE_SIZE, CUBE_SIZE,
                    CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE,
                    -CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE,
                    -CUBE_SIZE, CUBE_SIZE, CUBE_SIZE,
                    //BOTTOM
                    CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE,
                    CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE,
                    -CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE,
                    -CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE,
                    //RIGHT
                    CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE,
                    CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE,
                    CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE,
                    CUBE_SIZE, CUBE_SIZE, CUBE_SIZE,
                    //LEFT
                    -CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE,
                    -CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE,
                    -CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE,
                    -CUBE_SIZE, CUBE_SIZE, CUBE_SIZE,
                    //FRONT
                    CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE,
                    -CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE,
                    -CUBE_SIZE, CUBE_SIZE, CUBE_SIZE,
                    CUBE_SIZE, CUBE_SIZE, CUBE_SIZE,
                    //BACK
                    CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE,
                    -CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE,
                    -CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE,
                    CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE,
            };

    const unsigned int CUBE_INDICES[] =
            {
            0, 1, 2,
            0, 2, 3
            };

    std::tuple<const float *, const unsigned int*> getFace(Face face)
    {
        return std::make_tuple(&CUBE_VERTICES[face * FACE_VERTICES_COUNT], CUBE_INDICES);
    }
}