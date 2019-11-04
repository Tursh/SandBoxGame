//
// Created by musique88 on 11/4/19.
//
#pragma once


struct Item
{
	const unsigned int ID_;

	float weight_;

	float volume_;

	Item(unsigned int ID, float weight, float volume);

	virtual const char *getName();

	virtual const char *getDescription();
};