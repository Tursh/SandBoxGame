//
// Created by tursh on 4/30/19.
//

#pragma once


#include <State/State.h>
#include <World/World.h>
#include <GUI/Panel.h>
#include <GLFW/glfw3.h>

namespace SBG
{

    class PlayState : public CGE::State::State
    {
        World world;
    public:
        PlayState();

        void tick() override;

        void draw() override;

        CGE::GUI::Panel *pausePanel;

        void keyCallback(GLFWwindow *window, int key, int action);
    };

}