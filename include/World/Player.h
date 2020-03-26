//
// Created by tursh on 1/17/19.
//

#pragma once

#include <Entities/Entity.h>
#include <View/Camera.h>

//To dodge some stupid errors
class World;

namespace Entities
{

    extern const float PLAYER_HEIGHT, PLAYER_WIDTH;

    class Player : public CGE::Entities::Entity
    {
        CGE::View::Camera &camera_;
    public:
        explicit Player(unsigned int texMeshID, CGE::View::Camera &camera);

        explicit Player(CGE::Loader::TexturedMesh *texMesh, CGE::View::Camera &camera,
                        glm::vec3 position = glm::vec3(0),
                        glm::vec3 rotation = glm::vec3(0));

        void move(float speed, World *terrain);

        /**
         * Activate all player actions (ex: hit block, open inventory, etc...
         */
        void checkAction(World *world);

        void hit(World *world);

        const glm::vec3 &getSize() override;
    };

}

