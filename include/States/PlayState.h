//
// Created by tursh on 4/30/19.
//

#pragma once


#include <State/State.h>
#include <World/World.h>

class PlayState : public CGE::State::State
{
    World world;
public:
    void tick() override;

    void draw() override;

};


