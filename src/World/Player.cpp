//
// Created by tursh on 1/17/19.
//

#include <glm/vec2.hpp>

#define _USE_MATH_DEFINES

#include <cmath>
#include <World/Player.h>
#include <World/World.h>
#include <Loader/Models/TwoDAnimatedModel.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/string_cast.hpp>


#include "IO/Input.h"

namespace Entities
{

    const float PLAYER_HEIGHT = 3, PLAYER_WIDTH = 2;

    Player::Player(unsigned int texModelID)
            : Entity(texModelID, glm::vec3(0, 0, 0.1f), glm::vec3(0.7853f * 2, 0, 0)) {}

    Player::Player(CGE::Loader::TexturedModel *texModel, glm::vec3 position, glm::vec3 rotation)
            : Entity(std::shared_ptr<CGE::Loader::TexturedModel>(texModel), position, rotation) {}

    void Player::move(float speed, World *world)
    {
        //We start with a 3D vector of (0, 0, 0)
        glm::vec3 relativeMovement(0);

        //If there is a opened panel, the
        if (!CGE::IO::input::isPanelVisible())
        {
            //I added this scope to reuse the variable name "rotation"
            {
                glm::vec2 mouse = CGE::IO::input::getCursorShifting();
                glm::vec3 rotation(mouse.y, mouse.x, 0);
                rotate(rotation);
            }

            //Get inputs
            if (CGE::IO::input::isKeyPressed(GLFW_KEY_W))
                relativeMovement.z -= 1;
            if (CGE::IO::input::isKeyPressed(GLFW_KEY_S))
                relativeMovement.z += 1;
            if (CGE::IO::input::isKeyPressed(GLFW_KEY_A))
                relativeMovement.x -= 1;
            if (CGE::IO::input::isKeyPressed(GLFW_KEY_D))
                relativeMovement.x += 1;
        }

        relativeMovement.y -= 1;

        //If there was no movement
        if (!(glm::length(relativeMovement) < 0.0001f))
            relativeMovement = glm::normalize(relativeMovement);

        relativeMovement *= speed;

        //Get player rotation vector
        glm::vec3 ar = getRotation();

        auto s = (float) sin((double) ar.y);
        auto c = (float) cos((double) ar.y);

        glm::vec3 movement = relativeMovement;

        movement.z = relativeMovement.x * s + relativeMovement.z * c;
        movement.x = relativeMovement.x * c - relativeMovement.z * s;

        //Get player position vector
        glm::vec3 ap = getPosition();

        glm::vec3 futurPosition = ap + movement;


        const Bloc &floorBloc = world->getBloc(
                {futurPosition.x, (int) futurPosition.y - 1, futurPosition.z});

        if (floorBloc.ID != Blocs::AIR)
        {
            futurPosition.y = std::floor(futurPosition.y) + 0.5f;

            if (CGE::IO::input::isKeyPressed(GLFW_KEY_SPACE))
                futurPosition.y += 1;
        }

        movement = futurPosition - ap;

        CGE::Entities::Entity::move(movement);

    }

}