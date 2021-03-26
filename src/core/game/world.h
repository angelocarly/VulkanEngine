//
// Created by magnias on 25/03/2021.
//
#ifndef VULKANENGINE_WORLD_H
#define VULKANENGINE_WORLD_H

#include "core/graphics/renderable.h"
#include "game_entity.h"
#include <list>
//#include "entity.h"

class World : public IRenderable
{
public:

    World(vks::VksDevice &device)
    {
//        _entities.resize(10);
        for(int i = 0; i < 500; i++)
        {
//            _entities[i] = Entity(device);
            _entities.emplace_back(device);
        }
    };

    virtual void draw(IRenderProvider &renderProvider)
    {
        _entities.front().draw(renderProvider);
        for( auto entity : _entities)
        {
            entity.draw2(renderProvider);
        }
    }
private:

//    std::vector<Entity> _entities;
    std::list<Entity> _entities;

};

#endif //VULKANENGINE_WORLD_H
