//
// Created by tursh on 1/17/19.
//

#include <glm/vec2.hpp>

#define _USE_MATH_DEFINES

#include <cmath>
#include <World/Player.h>
#include <World/World.h>
#include <Loader/Models/TwoDAnimatedModel.h>
#include <Text/TextRenderer.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/string_cast.hpp>


#include "IO/Input.h"

namespace Entities
{

    const glm::vec3 PLAYER_SIZE = {0.9f, 1.8f, 0.9f};

    const float hitCooldown = 0.5f;

    Player::Player(unsigned int texModelID, CGE::View::Camera &camera)
            : Entity(texModelID, glm::vec3(0, 0, 0.1f), glm::vec3(0.7853f * 2, 0, 0)), camera_(camera) {}

    Player::Player(CGE::Loader::TexturedModel *texModel, CGE::View::Camera &camera, glm::vec3 position,
                   glm::vec3 rotation)
            : Entity(std::shared_ptr<CGE::Loader::TexturedModel>(texModel), position, rotation),
              camera_(camera) {}

    void Player::move(float speed, World *world)
    {

        //We start with a 3D vector of (0, 0, 0)
        glm::vec3 relativeForces(0);

        //If there is a opened panel, rotate camera depending on the mouse forces
        if (!CGE::IO::input::isPanelVisible())
        {
            //Get mouse input
            glm::vec2 mouse = CGE::IO::input::getCursorShifting();
            rotate({-mouse.y, mouse.x, 0});


            //Get key inputs
            if (CGE::IO::input::isKeyPressed(GLFW_KEY_W))
                relativeForces.z -= 1;
            if (CGE::IO::input::isKeyPressed(GLFW_KEY_S))
                relativeForces.z += 1;
            if (CGE::IO::input::isKeyPressed(GLFW_KEY_A))
                relativeForces.x -= 1;
            if (CGE::IO::input::isKeyPressed(GLFW_KEY_D))
                relativeForces.x += 1;
            if (CGE::IO::input::isKeyPressed(GLFW_KEY_LEFT_SHIFT))
                relativeForces.y -= 1;
            if (CGE::IO::input::isKeyPressed(GLFW_KEY_SPACE))
                relativeForces.y += 1;
            //if (CGE::IO::input::isKeyPressed(GLFW_KEY_SPACE) && isOnGround())
            //{
            //    glm::vec3 speed = getSpeed();
            //    speed.y = 0;
            //    setSpeed(speed);
            //    addForce(3, glm::vec3(0, 0.2f, 0));
            //}

        }



        //Normalize the relative forces so that the speed is the same in all orientations
        if (!(glm::length(relativeForces) < 0.0001f))
            relativeForces = glm::normalize(relativeForces);

        relativeForces *= speed * (CGE::IO::input::isKeyPressed(GLFW_KEY_LEFT_CONTROL) ? 2.0f : 1.0f);

        //Orientate the forces
        glm::vec3 ar = getRenderRotation();

        auto s = (float) sin((double) ar.y);
        auto c = (float) cos((double) ar.y);

        glm::vec3 forces = relativeForces;

        forces.z = relativeForces.x * s + relativeForces.z * c;
        forces.x = relativeForces.x * c - relativeForces.z * s;

        addForce(1, forces);

    }

    const glm::vec3 &Player::getSize()
    {
        return PLAYER_SIZE;
    }

    double lastHit = 0.0f;

    CGE::Utils::PerlinNoise pn;

    void Player::checkAction(World *world)
    {
        if (CGE::IO::input::isButtonPressed(GLFW_MOUSE_BUTTON_1))
        {
            if (glfwGetTime() - lastHit > hitCooldown)
            {
                hit(world);
                lastHit = glfwGetTime();
            }
        }
        else if (CGE::IO::input::isButtonPressed(GLFW_MOUSE_BUTTON_2))
        {
            if (glfwGetTime() - lastHit > hitCooldown)
            {
                glm::vec3 hitBlockPosition = world->getPickedBlock(6.0f);

                unsigned char blockState = world->getBlock(hitBlockPosition).state;

                Chunk *chunk = world->getChunk(hitBlockPosition);

                glm::ivec3 chunkPosition = chunk->getChunkPosition();

                double groundLevel[19 * 19];
                double higher = 0, lower = 256;

                {
                    int xEndPosition = (chunkPosition.x + 1) * CHUNK_SIZE + 2,
                            zEndPosition = (chunkPosition.z + 1) * CHUNK_SIZE + 2,
                            i = 0;

                    for (int x = chunkPosition.x * CHUNK_SIZE - 1; x < xEndPosition; ++x)
                        for (int z = chunkPosition.z * CHUNK_SIZE - 1; z < zEndPosition; ++z)
                        {
                            groundLevel[i] = pn.noise(x / (double) (CHUNK_SIZE * 4), z / (double) (CHUNK_SIZE * 4), 0) *
                                             CHUNK_SIZE * 4;
                            higher = std::max(higher, groundLevel[i]);
                            lower = std::min(lower, groundLevel[i]);
                            ++i;
                        }
                }

                double averageGroundLevels[17 * 17];

                for (int xa = 0; xa < 17; ++xa)
                    for (int za = 0; za < 17; ++za)
                    {
                        double &averageGroundLevel = averageGroundLevels[xa * 17 + za] = 0;
                        for (int xb = 0; xb < 3; ++xb)
                            for (int zb = 0; zb < 3; ++zb)
                            {
                                averageGroundLevel += groundLevel[(xa + xb) * 19 + (za + zb)];
                            }

                        averageGroundLevel /= 9;
                    }

                glm::ivec3 blockPositionInChunk = world->getPositionInChunk(hitBlockPosition);

                int x = blockPositionInChunk.x, z = blockPositionInChunk.z;

                double cornerGroundLevels[4] =
                        {
                                averageGroundLevels[x * 17 + z],
                                averageGroundLevels[x * 17 + z + 1],
                                averageGroundLevels[(x + 1) * 17 + z],
                                averageGroundLevels[(x + 1) * 17 + z + 1]
                        };

                logInfo(glm::to_string(hitBlockPosition) << " => " << cornerGroundLevels[0] << ", "
                                                         << cornerGroundLevels[1] << ", "
                                                         << cornerGroundLevels[2] << ", " << cornerGroundLevels[3]
                                                         << " - "
                                                         << ((blockState >> 7) & 1) << ((blockState >> 6) & 1)
                                                         << ((blockState >> 5) & 1)
                                                         << ((blockState >> 4) & 1)
                                                         << ((blockState >> 3) & 1) << ((blockState >> 2) & 1)
                                                         << ((blockState >> 1) & 1)
                                                         << (blockState & 1));

                lastHit = glfwGetTime();
            }
        }
        else
            lastHit = 0.0f;
    }


    void Player::hit(World *world)
    {
        glm::vec3 hitBlockPosition = world->getPickedBlock(6.0f);

        world->setBlock(hitBlockPosition, Blocks::AIR_BLOCK);
    }
}