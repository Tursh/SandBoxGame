//
// Created by tursh on 4/30/19.
//

#pragma once


#include <State/State.h>

class PlayState : public CGE::State::State
{
public:
    void tick() override;

    void draw() override;

};


