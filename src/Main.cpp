#include <IO/Input.h>
#include <iostream>
#include <Engine.h>
#include <Utils/TimeUtils.h>
#include <thread>
#include <State/StateManager.h>
#include <Text/TextRenderer.h>
#include "States/PlayState.h"

void init()
{
    CGE::initEngine("Sand Box Game", 1280, 720, false);
    CGE::Text::textRenderer::init("res/graphics/fonts/Archivo-Regular.ttf");
    CGE::Utils::TPSClock::init();
    //CGE::IO::input::grabMouse();
    CGE::State::stateManager::createCurrentState<PlayState>();
}

void loopTick()
{
    auto display = CGE::IO::getWindow();
    while (!display->shouldClose())
    {
        while (!display->shouldClose() && CGE::Utils::TPSClock::shouldTick())
        {
            CGE::State::stateManager::getCurrentState()->tick();
        }
    }
}

void loopRender()
{
    auto display = CGE::IO::getWindow();

    while (!display->shouldClose())
    {
        CGE::State::stateManager::getCurrentState()->draw();
        display->update();
    }
}

void terminate()
{
    CGE::State::stateManager::deleteCurrentState();
    CGE::stopEngine();
}

int main()
{
    init();
    //Start asynchronously the tick and render loop
    std::thread tickThread(loopTick);
    loopRender();
    //Wait for the two loops to end
    tickThread.join();

    terminate();
    return EXIT_SUCCESS;
}