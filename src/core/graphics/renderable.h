//
// Created by magnias on 22/03/2021.
//

#ifndef VULKANENGINE_ENTITY_H
#define VULKANENGINE_ENTITY_H

#include <glm/glm.hpp>
#include "render_manager.h"

class IRenderable
{
public:
    virtual ~IRenderable(){};
    virtual void draw(IRenderProvider &renderProvider) = 0;
};

#endif //VULKANENGINE_ENTITY_H
