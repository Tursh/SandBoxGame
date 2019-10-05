//
// Created by tursh on 5/2/19.
//

#include <World/Terrain/Bloc.h>

namespace Blocs
{
    const short
            AIR = 0,
            DIRT = 1;

    const Bloc AIR_BLOC = {AIR, 0};

    const float CUBE_SIZE = 1.0f;

    const unsigned int FACE_VERTICES_COUNT = 4 * 3, FACE_INDICES_COUNT = 2 * 3;

    const float CUBE_VERTICES[] =
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

    const unsigned int CUBE_INDICES[] =
            {
            0, 1, 2,
            2, 3, 0,
            //FOR RIGHT, TOP or BACK
            0, 3, 2,
            2, 1, 0
            };

    std::tuple<const float *, const unsigned int*> getFace(Face face)
    {
        int indicesOffset = (face == RIGHT || face == TOP || face == BACK) ? FACE_INDICES_COUNT : 0;
        return std::make_tuple(CUBE_VERTICES + face * FACE_VERTICES_COUNT, CUBE_INDICES + indicesOffset);
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
