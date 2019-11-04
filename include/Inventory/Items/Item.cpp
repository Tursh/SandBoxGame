//
// Created by musique88 on 11/4/19.
//

#include "Item.h"


Item::Item(unsigned int ID, float weight, float volume)
:ID_(ID), weight_(weight), volume_(volume)
{}

const char *Item::getName()
{
	return nullptr;
}

const char *Item::getDescription()
{
	return nullptr;
}