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

    const glm::vec3 PLAYER_SIZE(1, 2, 1);

    Player::Player(unsigned int texModelID)
            : Entity(texModelID, glm::vec3(0, 0, 0.1f), glm::vec3(0.7853f * 2, 0, 0)) {}

    Player::Player(CGE::Loader::TexturedModel *texModel, glm::vec3 position, glm::vec3 rotation)
            : Entity(std::shared_ptr<CGE::Loader::TexturedModel>(texModel), position, rotation) {}

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

        }



        //Normalize the relative forces so that the speed is the same in all orientations
        if (!(glm::length(relativeForces) < 0.0001f))
            relativeForces = glm::normalize(relativeForces);

        relativeForces *= speed;

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

}