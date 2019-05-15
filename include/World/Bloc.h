//
// Created by tursh on 5/2/19.
//

#pragma once

#include <tuple>

namespace Blocs
{
    extern const short
            AIR,
            DIRT;

    extern const float CUBE_SIZE;

    extern const unsigned int FACE_VERTICES_COUNT, FACE_INDICES_COUNT;

    extern const float CUBE_VERTICES[];
    extern const unsigned int CUBE_INDICES[];

    enum Face
    {
        TOP = 0, BOTTOM = 1, RIGHT = 2, LEFT = 3, FRONT = 4, BACK = 5
    };

    std::tuple<const float *, const unsigned int *> getFace(Face face);
}

struct Bloc
{
    short ID;
    //first 4 digits are the Block shape and 4 last are rotation
    char state;

    bool operator ==(Bloc otherBloc);
};