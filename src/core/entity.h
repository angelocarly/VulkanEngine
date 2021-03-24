//
// Created by magnias on 22/03/2021.
//

#ifndef VULKANENGINE_ENTITY_H
#define VULKANENGINE_ENTITY_H

#include <glm/glm.hpp>

class Entity
{
public:

    glm::mat4 calculateTransform()
    {
        return glm::translate(glm::mat4(1), position);
    }

    vks::VksModel *model;
    glm::vec3 position;

private:
};

#endif //VULKANENGINE_ENTITY_H
