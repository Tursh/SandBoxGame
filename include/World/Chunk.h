//
// Created by tursh on 4/30/19.
//

#pragma once


#include <glm/vec3.hpp>
#include <Loader/Models/TexturedModel.h>


struct Bloc
{
    short ID;
    //first 4 digits are the Block shape and 4 last are rotation
    char state;
};

extern const unsigned int CHUNK_SIZE;

class Chunk
{
    Bloc *blocs_;

    CGE::Loader::TexturedModel *texModel_;

    void loadToTexModel();

public:
    Chunk(Bloc *blocs);

    void update();

    void render();

    void setBloc(glm::ivec3 &position, Bloc &newBloc);

    Bloc getBloc(glm::ivec3 &position);
};


