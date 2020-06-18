#include "States/PlayState.h"
#include <Engine.h>
#include <Utils/TimeUtils.h>
#include <thread>
#include <State/StateManager.h>
#include <GUI/Text/TextRenderer.h>

void init()
{
    CGE::initEngine("Sand Box Game", 1280, 720, false);
    CGE::GUI::Text::TextRenderer::init("res/graphics/fonts/Archivo-Regular.ttf");
    CGE::Utils::TPSClock::init();
    CGE::State::StateManager::constructAndSetCurrentState<PlayState>();
    glClearColor(0.2, 0.3, 0.5,1);
}

void loopTick()
{
    auto display = CGE::IO::getWindow();
    while (!display->shouldClose())
    {
        while (!display->shouldClose() && CGE::Utils::TPSClock::shouldTick())
        {
            CGE::State::StateManager::getCurrentState()->tick();
        }
    }
}

void loopRender()
{
    auto display = CGE::IO::getWindow();

    while (!display->shouldClose())
    {
        CGE::State::StateManager::getCurrentState()->draw();
        display->update();
    }
}

void terminate()
{
    CGE::State::StateManager::deleteCurrentState();
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