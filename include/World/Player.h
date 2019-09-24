//
// Created by tursh on 1/17/19.
//

#pragma once

#include <Entities/Entity.h>

//To dodge some stupid errors
class World;

namespace Entities
{

    extern const float PLAYER_HEIGHT, PLAYER_WIDTH;

    class Player : public CGE::Entities::Entity
    {
        float orientation{};
    public:
        explicit Player(unsigned int texModelID);

        explicit Player(CGE::Loader::TexturedModel *texModel, glm::vec3 position = glm::vec3(0),
                        glm::vec3 rotation = glm::vec3(0));

        void move(float speed, World *terrain);

        virtual const glm::vec3 &getSize() override;
    };

}

