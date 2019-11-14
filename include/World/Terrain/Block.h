//
// Created by tursh on 5/2/19.
//

#pragma once

#include <tuple>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>


struct Block
{
	short ID;
	//first 4 digits are the Blockk shape and 4 last are rotation
	char state;
	
	//Compare their ID and state
	bool operator==(const Block &otherBlock) const;
	
	//Compare their ID and state
	bool operator!=(const Block &otherBlock) const;
};

namespace Blocks
{
	
	extern const short
			AIR,
			DIRT;
	
	extern const Block AIR_BLOC;
	
	extern const float CUBE_SIZE;
	
	extern const unsigned int VERTICES_PER_FACE, INDICES_PER_FACE;
	
	extern const glm::vec3 CUBE_FACE_VERTICES[];
	extern const unsigned int CUBE_FACE_INDICES[];
	
	enum Face
	{
		TOP = 0, BOTTOM = 1, RIGHT = 2, LEFT = 3, FRONT = 4, BACK = 5
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
	loadBlock(std::vector<glm::vec3> &positions, std::vector<glm::vec2> &texCoords, std::vector<unsigned int> &indices,
			 glm::ivec3 &blockPosition, Block *blockToLoad, const Block **neighbors, glm::vec4 &texCoordsOffset);
}