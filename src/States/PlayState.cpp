//
// Created by tursh on 4/30/19.
//

#include <IO/Input.h>
#include "States/PlayState.h"

static void keyCallback(GLFWwindow *window, int key, int action)
{
    if(key == GLFW_KEY_G && action == GLFW_PRESS)
        CGE::IO::input::toggleGrabMouse();
}

PlayState::PlayState()
{
    CGE::IO::input::setYourOwnKeyCallBack(keyCallback);
}

void PlayState::tick()
{
    world.tick();
}

void PlayState::draw()
{
    world.render();
}
