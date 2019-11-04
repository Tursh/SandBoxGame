/*
 * ItemManager.h
 *
 * Created by tursh on 11/4/19.
*/

#pragma once


#include <memory>
#include <map>
#include "Item.h"

namespace ItemManager
{
    void init();

    Item getItem(unsigned int ID);

    void terminate();
};


