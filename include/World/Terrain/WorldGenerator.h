/*
 * WorldGenerator.h
 *
 * Created by tursh on 9/28/19.
*/

#pragma once


#include <glm/glm.hpp>
#include <vector>
#include <thread>
#include <condition_variable>
#include <Utils/ThreadProcess.h>
#include <Utils/PerlinNoise.h>

namespace SBG
{

    class ChunkManager;

    extern const unsigned int MAX_WORLD_GENERATION_HEIGHT;

    class WorldGenerator : public CGE::Utils::ThreadProcess
    {
        World *world_;

        CGE::Utils::PerlinNoise pn;

        ChunkManager &chunkManager_;

        void run() override;

    public:
        WorldGenerator(World *world, ChunkManager &chunkManager);

    };

}