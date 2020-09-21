//
// Created by tursh on 5/2/19.
//

#pragma once

#include <tuple>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>
#include <Loader/Meshes/MeshBuilder.h>

namespace SBG
{

    struct Block
    {
        short ID;
        //first 4 digits are the Block shape and 4 last are rotation
        unsigned char state;

        //Compare their ID and state
        bool operator==(const Block &otherBlock) const;

        //Compare their ID and state
        bool operator!=(const Block &otherBlock) const;
    };

    namespace Blocks
    {

        extern const short
                AIR,
                DIRT,
                STONE,
                CLOUD;

        extern const short
                DIRT_TEX_ATLAS[3],
                STONE_TEX_ATLAS[3],
                CLOUD_TEX_ATLAS[3];

        extern const short *TEX_ATLAS[];

        extern const Block AIR_BLOCK, CLOUD_BLOCK;

        extern const float CUBE_SIZE;

        extern const unsigned int VERTICES_PER_FACE, INDICES_PER_FACE;

        extern const glm::vec3 CUBE_FACE_VERTICES[];
        extern const unsigned int CUBE_FACE_INDICES[];

        enum Face
        {
            BOTTOM = 0, TOP = 1, LEFT = 2, RIGHT = 3, BACK = 4, FRONT = 5
        };

        std::tuple<const glm::vec3 *, const unsigned int *> getFace(Face face);

        /**
         * Load a block to vertices and texture coordinates
         * @param positions Vertices list to load to
         * @param texCoords Texture coordinates list to load to
         * @param blockPosition The position of the block
         * @param blockToLoad The block to load
         * @param neighbors The block neighbors (x+, x-, y+, y-, z+, z-)
         */
        void
        loadBlock(CGE::Loader::MeshBuilder &meshBuilder,
                  glm::ivec3 &blockPosition, Block *blockToLoad, const Block **neighbors, const glm::vec4 &topTexCoords,
                  glm::vec4 sideTexCoords = glm::vec4(-1), glm::vec4 bottomTexCoords = glm::vec4(-1));
    }

}