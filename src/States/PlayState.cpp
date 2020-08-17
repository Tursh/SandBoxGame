//
// Created by tursh on 4/30/19.
//

#include <IO/Input.h>
#include <GUI/GUIManager.h>
#include "States/PlayState.h"

namespace SBG
{

    PlayState::PlayState()
    {
        CGE::IO::input::setYourOwnKeyCallBack(
                std::bind(&PlayState::keyCallback, this, std::placeholders::_1, std::placeholders::_2,
                          std::placeholders::_3)
        );

        pausePanel = new CGE::GUI::Panel({0, 0}, {0, 0}, CGE::GUI::PANEL,
                                         std::bind(&PlayState::keyCallback, this, nullptr,
                                                   std::placeholders::_1,
                                                   std::placeholders::_2), false);
        pausePanel->setVisibility(false);
        CGE::GUI::GUIManager::addComponent(pausePanel);
    }

    void PlayState::tick()
    {
        world.tick();
    }

    void PlayState::draw()
    {
        world.render();
    }

    void PlayState::keyCallback(GLFWwindow *window, int key, int action)
    {
        if (key == GLFW_KEY_G && action == GLFW_PRESS)
            CGE::IO::input::toggleGrabMouse();
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            pausePanel->setVisibility(!pausePanel->getVisibility());
            CGE::IO::input::ungrabMouse();
        }
    }

}