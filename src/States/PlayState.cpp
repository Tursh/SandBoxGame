//
// Created by tursh on 4/30/19.
//

#include "States/PlayState.h"

void PlayState::tick()
{
    world.tick();
}

void PlayState::draw()
{
    world.render();
}
