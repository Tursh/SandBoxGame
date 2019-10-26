//
// Created by tursh on 5/2/19.
//

#pragma once

#include <tuple>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>


struct Bloc
{
    short ID;
    //first 4 digits are the Block shape and 4 last are rotation
    char state;

    //Compare their ID and state
    bool operator==(const Bloc &otherBloc) const;

    //Compare their ID and state
    bool operator!=(const Bloc &otherBloc) const;
};

namespace Blocs
{

    extern const short
            AIR,
            DIRT;

    extern const Bloc AIR_BLOC;

    extern const float CUBE_SIZE;

    extern const unsigned int POSITION_PER_FACE, INDICES_PER_FACE;

    extern const float CUBE_FACE_VERTICES[];
    extern const unsigned int CUBE_FACE_INDICES[];

    enum Face
    {
        TOP = 0, BOTTOM = 1, RIGHT = 2, LEFT = 3, FRONT = 4, BACK = 5
    };

    std::tuple<const float *, const unsigned int *> getFace(Face face);

    /**
     * Load a bloc to vertices and texture coordinates
     * @param positions Vertices list to load to
     * @param texCoords Texture coordinates list to load to
     * @param blocPosition The position of the bloc
     * @param blocToLoad The bloc to load
     * @param neighbors The bloc neighbors (x+, x-, y+, y-, z+, z-)
     */
    void loadBloc(std::vector<float> &positions, std::vector<float> &texCoords, std::vector<unsigned int> &indices,
                  glm::ivec3 &blocPosition, Bloc *blocToLoad, const Bloc **neighbors, glm::vec4 &texCoordsOffset);
}