//
// Created by magnias on 25/03/2021.
//
#ifndef VULKANENGINE_WORLD_H
#define VULKANENGINE_WORLD_H

#include "../../core/graphics/renderable.h"
#include "game_entity.h"
#include "octree.h"
#include <list>
#include "graph.h"
//#include "entity.h"

class World : public IRenderable
{
public:

    World(vks::VksDevice &device)
    :octree(Octree(device)), graph(Graph(device))
    {
//        for(int i = 0; i < 5; i++)
//        {
//            _entities.emplace_back(device);
//        }
    };

    virtual void draw(IRenderProvider &renderProvider)
    {
//        _entities.front().draw(renderProvider);
//        for( auto entity : _entities)
//        {
//            entity.draw2(renderProvider);
//        }

//        octree.draw(renderProvider);
        graph.draw(renderProvider);
    }
private:

//    std::vector<Entity> _entities;
    std::list<Entity> _entities;
    Octree octree;
    Graph graph;

};

#endif //VULKANENGINE_WORLD_H
