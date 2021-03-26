//
// Created by magnias on 25/03/2021.
//
#ifndef VULKANENGINE_WORLD_H
#define VULKANENGINE_WORLD_H

#include "core/graphics/renderable.h"
#include "game_entity.h"
//#include "entity.h"

class World : public IRenderable
{
public:

    World(vks::VksDevice &device)
    :entity(Entity(device)){

    };

    virtual void draw(IRenderProvider &renderProvider)
    {
        entity.draw(renderProvider);
    }
private:

    Entity entity;


};

#endif //VULKANENGINE_WORLD_H
