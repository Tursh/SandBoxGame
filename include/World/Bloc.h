//
// Created by tursh on 5/2/19.
//

#pragma once

#include <tuple>


struct Bloc
{
    short ID;
    //first 4 digits are the Block shape and 4 last are rotation
    char state;

    //Compare their ID and state
    bool operator==(const Bloc &otherBloc) const;
};

namespace Blocs
{

    extern const short
            AIR,
            DIRT;

    extern const Bloc AIR_BLOC;

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