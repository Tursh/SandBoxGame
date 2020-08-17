/*
 * ChunkManager.h
 *
 * Created by tursh on 9/27/19.
*/

#pragma once

#include <map>
#include "World/Player.h"
#include "Chunk.h"
#include "WorldGenerator.h"

namespace SBG
{

/*
 * Goal: Render and update only chunks around the player
 * How?: Load chunk that are close enough and unload too far ones
 */

    class ChunkManager : public CGE::Utils::ThreadProcess
    {

        SBG::Entities::Player *player_;

        std::map<int, std::map<int, std::map<int, Chunk *>>> &chunks_;

        /**
         * Distance in chunks before the chunk do not need to be loaded
         */
        int radius_ = 10;

        bool *loaded;

        int chunkCount_;

        glm::ivec3 centerChunkPosition_ = glm::ivec3(INT_MAX);

        std::vector<glm::ivec3> chunkToLoad_;

        World *world_;

        WorldGenerator worldGenerator_;

        void run() override;

    public:
        ChunkManager(SBG::Entities::Player *player,
                     World *world,
                     std::map<int, std::map<int, std::map<int, Chunk *>>> &chunks);

        ~ChunkManager();

        /**
         * Check if chunks are loaded
         */
        void tick();

        /**
         * Set the distance in chunks before the chunk do not need to be loaded
         * @param radius The distance in chunks (block position / 16)
         */
        void setRadius(int radius);

        /**
         * Search for a chunk that needs to be loaded and returns it
         * @return A chunk position to load a chunk
         */
        glm::vec3 getChunkToLoad();

    };

}