//
// Created by tursh on 5/2/19.
//

#define GLM_FORCE_ONLY_XYZW
#include <glm/glm.hpp>

#include <World/Terrain/Bloc.h>

namespace Blocs
{
	const short
			AIR = 0,
			DIRT = 1;
	
	const Bloc AIR_BLOC = {AIR, 0};
	
	const float CUBE_SIZE = 1.0f;
	
	const unsigned int
	//POSITION_PER_VERTEX = 3,
	//TEX_COORDS_PER_VERTEX = 2,
			
			VERTICES_PER_TRIANGLE = 3,
	//POSITION_PER_TRIANGLE = VERTICES_PER_TRIANGLE * POSITION_PER_VERTEX,
	//TEX_COORDS_PER_TRIANGLE = VERTICES_PER_TRIANGLE * TEX_COORDS_PER_VERTEX,
			
			VERTICES_PER_FACE = 4,
	//POSITION_PER_FACE = VERTICES_PER_FACE,
	//TEX_COORDS_PER_FACE = TEX_COORDS_PER_VERTEX * VERTICES_PER_FACE,
			INDICES_PER_FACE = 2 * VERTICES_PER_TRIANGLE;
	
	const glm::vec3 CUBE_FACE_VERTICES[] =
			{
					//TOP
					{CUBE_SIZE, CUBE_SIZE, CUBE_SIZE},
					{CUBE_SIZE, CUBE_SIZE, 0},
					{0,         CUBE_SIZE, 0},
					{0,         CUBE_SIZE, CUBE_SIZE},
					//BOTTOM
					{CUBE_SIZE, 0,         CUBE_SIZE},
					{CUBE_SIZE, 0,         0},
					{0,         0,         0},
					{0,         0,         CUBE_SIZE},
					//RIGHT
					{CUBE_SIZE, 0,         CUBE_SIZE},
					{CUBE_SIZE, 0,         0},
					{CUBE_SIZE, CUBE_SIZE, 0},
					{CUBE_SIZE, CUBE_SIZE, CUBE_SIZE},
					//LEFT
					{0,         0,         CUBE_SIZE},
					{0,         0,         0},
					{0,         CUBE_SIZE, 0},
					{0,         CUBE_SIZE, CUBE_SIZE},
					//FRONT
					{CUBE_SIZE, 0,         CUBE_SIZE},
					{0,         0,         CUBE_SIZE},
					{0,         CUBE_SIZE, CUBE_SIZE},
					{CUBE_SIZE, CUBE_SIZE, CUBE_SIZE},
					//BACK
					{CUBE_SIZE, 0,         0},
					{0,         0,         0},
					{0,         CUBE_SIZE, 0},
					{CUBE_SIZE, CUBE_SIZE, 0},
			};
	
	const glm::vec3 CUBE_VERTEX_POSITIONS[] =
			{
					{0,         0,         0},
					{0,         0,         CUBE_SIZE},
					{CUBE_SIZE, 0,         0},
					{CUBE_SIZE, 0,         CUBE_SIZE},
					{0,         CUBE_SIZE, 0},
					{0,         CUBE_SIZE, CUBE_SIZE},
					{CUBE_SIZE, CUBE_SIZE, 0},
					{CUBE_SIZE, CUBE_SIZE, CUBE_SIZE}
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
	
	const unsigned int STAIR_INDICES[4][6] =
			{
					{4, 3, 2, 5, 4, 1},
					{2, 3, 4, 0, 4, 5},
					{0, 3, 4, 2, 4, 5},
					{4, 3, 0, 5, 4, 1}
			};
	
	std::tuple<const glm::vec3 *, const unsigned int *> getFace(Face face)
	{
		int indicesOffset = (face == RIGHT || face == TOP || face == BACK) ? INDICES_PER_FACE : 0;
		return std::make_tuple(CUBE_FACE_VERTICES + face * VERTICES_PER_FACE, CUBE_FACE_INDICES + indicesOffset);
	}
	
	static void
	loadFace(std::vector<glm::vec3> &positions, std::vector<glm::vec2> &texCoords, std::vector<unsigned int> &indices,
			 glm::ivec3 &blocPosition, Face face, glm::vec4 blocTexCoordsOffset)
	{
		const unsigned int *faceIndices =
				CUBE_FACE_INDICES + ((face == RIGHT || face == TOP || face == BACK) ? INDICES_PER_FACE : 0);
		
		indices.insert(indices.end(), faceIndices, faceIndices + INDICES_PER_FACE);
		for (unsigned int i = indices.size() - INDICES_PER_FACE - 1; i < (unsigned int) indices.size(); ++i)
			indices[i] += positions.size();
		
		const glm::vec3 *faceVertices = CUBE_FACE_VERTICES + face * VERTICES_PER_FACE;
		positions.insert(positions.end(), faceVertices, faceVertices + VERTICES_PER_FACE);
		for (unsigned int i = positions.size() - VERTICES_PER_FACE; i < (unsigned int) positions.size(); ++i)
			positions[i] += (blocPosition.operator*=(CUBE_SIZE));
		
		glm::vec2 texCoordsBuf[VERTICES_PER_FACE] =
				{
						{blocTexCoordsOffset.z,
								blocTexCoordsOffset.w},
						{blocTexCoordsOffset.x,
								blocTexCoordsOffset.w},
						{blocTexCoordsOffset.x,
								blocTexCoordsOffset.y},
						{blocTexCoordsOffset.z,
								blocTexCoordsOffset.y},
				};
		
		texCoords.insert(texCoords.end(), texCoordsBuf, texCoordsBuf + VERTICES_PER_FACE);
	}
	
	static void
	loadTriangle(std::vector<glm::vec3> &positions, std::vector<glm::vec2> &texCoords,
				 std::vector<unsigned int> &indices,
				 glm::ivec3 &blocPosition,
				 glm::vec3 *triangleVertexPositions, glm::vec4 &texCoordsOffset, bool invIndices = false)
	{
		indices.insert(indices.end(),
					   TRIANGLE_INDICES + invIndices * VERTICES_PER_TRIANGLE,
					   TRIANGLE_INDICES + invIndices * VERTICES_PER_TRIANGLE + VERTICES_PER_TRIANGLE);
		for (unsigned int i = indices.size() - VERTICES_PER_TRIANGLE; i < (unsigned int) indices.size(); ++i)
			indices[i] += positions.size();
		
		positions.insert(positions.end(), triangleVertexPositions, triangleVertexPositions + VERTICES_PER_TRIANGLE);
		for (unsigned int i = positions.size() - VERTICES_PER_TRIANGLE; i < (unsigned int) positions.size(); ++i)
			positions[i] += blocPosition;
		
		glm::vec2 texSize = {texCoordsOffset.z - texCoordsOffset.x, texCoordsOffset.w - texCoordsOffset.y};
		
		glm::vec2 texCoordsBuf[VERTICES_PER_TRIANGLE] =
				{
						{texCoordsOffset.x + triangleVertexPositions[0].x / CUBE_SIZE * texSize.x,
								texCoordsOffset.y + triangleVertexPositions[0].z / CUBE_SIZE * texSize.y},
						{texCoordsOffset.x + triangleVertexPositions[1].x / CUBE_SIZE * texSize.x,
								texCoordsOffset.y + triangleVertexPositions[1].z / CUBE_SIZE * texSize.y},
						{texCoordsOffset.x + triangleVertexPositions[2].x / CUBE_SIZE * texSize.x,
								texCoordsOffset.y + triangleVertexPositions[2].z / CUBE_SIZE * texSize.y},
				};
		
		texCoords.insert(texCoords.end(), texCoordsBuf, texCoordsBuf + VERTICES_PER_TRIANGLE);
	}
	
	
	static unsigned int
	loadVertex(std::vector<glm::vec3> &positions, std::vector<glm::vec2> &texCoords, glm::vec3 &vertexPosition,
			   glm::ivec3 &blocPosition, glm::vec4 &texCoordsOffset)
	{
		glm::vec2 texSize = {texCoordsOffset.z - texCoordsOffset.x, texCoordsOffset.w - texCoordsOffset.y};
		
		glm::vec2 vertexTexCoord =
				{texCoordsOffset.x + vertexPosition.x / CUBE_SIZE * texSize.x,
				 texCoordsOffset.y + vertexPosition.z / CUBE_SIZE * texSize.y};
		texCoords.push_back(vertexTexCoord);
		
		positions.push_back(vertexPosition + ((glm::vec3)(blocPosition) * CUBE_SIZE));
		
		return positions.size() - 1;
	}
	
	
	void
	loadBloc(std::vector<glm::vec3> &positions, std::vector<glm::vec2> &texCoords, std::vector<unsigned int> &indices,
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
		
		//If there are no corner up and the midY flag is down, then it should be a air bloc
		if (cornerFlagCount == 4 && !midY)
		{
			blocToLoad->ID = Blocs::AIR;
			blocToLoad->state = 0;
			return;
		}
		
		bool xn = !xnzn || !xnzp, zn = !xnzn || !xpzn, xz = !xnzn || !xpzp;
		
		//Bottom
		if (neighbors[2 + invY] == nullptr || neighbors[2 + invY]->ID == Blocs::AIR)
		{
			if (midY)
				loadFace(positions, texCoords, indices, blocPosition, BOTTOM, texCoordsOffset); //Load face bottom 0
			else if (midCount == 0)
			{
				if (cornerFlagCount <= 2)
					loadFace(positions, texCoords, indices, blocPosition, BOTTOM,
							 texCoordsOffset); //Load face bottom 0
				else
				{
					//Load face bottom 1
					glm::vec3 triangleVertexPositions[VERTICES_PER_TRIANGLE];
					int i = 0;
					for (int point = 0; point < 4; ++point)
					{
						if ((shape >> (3 - point)) & 1)
						{
							std::copy(CUBE_VERTEX_POSITIONS + point,
									  CUBE_VERTEX_POSITIONS + (point + 1),
									  triangleVertexPositions + i);
							++i;
						}
					}
					loadTriangle(positions, texCoords, indices, blocPosition,
								 triangleVertexPositions, texCoordsOffset, !xn);
				}
			} else
			{
				//If there is only one corner up, face bottom 1 has to be drawn
				if (cornerFlagCount == 3)
				{
					unsigned int startIndex = positions.size();
					{
						glm::vec3 triangleVertexPositions[VERTICES_PER_TRIANGLE];
						int i = 0;
						for (int point = 0; point < 4; ++point)
						{
							if ((shape >> (3 - point)) & 1)
							{
								std::copy(CUBE_VERTEX_POSITIONS + point,
										  CUBE_VERTEX_POSITIONS + (point + 1),
										  triangleVertexPositions + i);
								++i;
							}
						}
						loadTriangle(positions, texCoords, indices, blocPosition,
									 triangleVertexPositions, texCoordsOffset, !xn);
					}
					//If there is only on mid flag, load face bottom 2
					if (midCount == 1)
					{
						//Add the missing vertex
						glm::vec3 vertexPosition = {midX ? CUBE_SIZE / 2 : xn ? CUBE_SIZE : 0, 0,
													midZ ? CUBE_SIZE / 2 : zn ? CUBE_SIZE : 0};
						loadVertex(positions, texCoords, vertexPosition, blocPosition, texCoordsOffset);
						
						//Create new triangle from existing positions
						for (int i = startIndex;
							 i < positions.size(); ++i)
						{
							//If the vertex is not the corner up, we use it to make the second triangle
							if (!(((positions[i].x == blocPosition.x) ^ !xn)
								  && ((positions[i].z == blocPosition.z) ^ !zn)))
								indices.push_back(i);
						}
						//If the corner up is a zn we need to flip the indices so it renders it in clockwise direction
						if (zn)
							std::iter_swap(indices.end() - 3, indices.end() - 1);
					}
						
						//If the 2 mid flags are up, load face bottom 3 (stair face)
					else if (midCount == 2)
					{
						
						//We have to add 3 vertex to add 2 triangles
						glm::vec3 vertex1Position = {xn ? CUBE_SIZE : 0, 0, 0.5f};
						loadVertex(positions, texCoords, vertex1Position, blocPosition, texCoordsOffset);
						
						glm::vec3 vertex2Position = {CUBE_SIZE / 2, 0, CUBE_SIZE / 2};
						loadVertex(positions, texCoords, vertex2Position, blocPosition, texCoordsOffset);
						
						glm::vec3 vertex3Position = {0.5f, 0, zn ? CUBE_SIZE : 0};
						loadVertex(positions, texCoords, vertex3Position, blocPosition, texCoordsOffset);
						
						const unsigned int *stairIndices = STAIR_INDICES[!xn * 2 + !zn];
						indices.insert(indices.end(), stairIndices, stairIndices + VERTICES_PER_TRIANGLE * 2);
						
						for (int i = indices.size() - VERTICES_PER_TRIANGLE * 2; i < indices.size(); ++i)
							indices[i] += startIndex;
					}
				} else if (cornerFlagCount == 2)
				{
				
				}
					//Else the cornerFlagCount = 1
				else
				{
					
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
