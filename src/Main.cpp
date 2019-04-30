#include <iostream>
#include <Engine.h>
#include <IO/Input.h>
#include <Utils/TimeUtils.h>
#include <thread>
#include <State/StateManager.h>
#include "PlayState.h"

void init()
{
    CGE::initEngine("Sand Box Game", 1280, 720, false);
    CGE::IO::input::grabMouse();
    CGE::Utils::initTPSClock();

    CGE::State::stateManager::setCurrentState(new PlayState());
}

void loopTick()
{
    auto display = CGE::IO::getDisplay();
    while (!display->shouldClose())
    {
        while (CGE::Utils::shouldTick())
            CGE::State::stateManager::getCurrentState()->tick();
    }
}

void loopRender()
{
    auto display = CGE::IO::getDisplay();

    while (!display->shouldClose())
    {
        CGE::State::stateManager::getCurrentState()->draw();
        display->update();
    }
}

void terminate()
{
    CGE::stopEngine();
}

int main()
{
    init();
    std::thread tickThread(loopTick);
    loopRender();
    terminate();
    return 0;
}