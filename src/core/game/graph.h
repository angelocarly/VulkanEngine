//
// Created by magnias on 25/03/2021.
//
#ifndef VULKANENGINE_GRAPH_H
#define VULKANENGINE_GRAPH_H

#include "../graphics/renderable.h"
#include "game_entity.h"
#include <list>
#include <vector>
#include <glm/glm.hpp>
#include <boost/ptr_container/ptr_list.hpp>


class Graph: public IRenderable
{
public:

    struct Node
    {
        std::string text;
        glm::vec3 position;
        boost::ptr_list<Node> effects;
    };

    Graph(vks::VksDevice &device)
    {
        // Generate random nodes
        nodes.resize(10);
        for(int i=0; i<nodes.size(); i++)
        {
            nodes[i] = Node();
            nodes[i].text = &"test" [ i];
//            nodes[i].position = glm::vec3(0);
            nodes[i].position = glm::vec3(
                    std::rand() % 1000 / 100.0f - 5,
//                    std::rand() % 1000 / 100.0f - 5,
0,
                    std::rand() % 1000 / 100.0f - 5
            );
        }

        // Generate random links
        for(int i=0; i<nodes.size(); i++)
        {
            int randomNode = std::rand() % 10;
            if (randomNode == i) randomNode++;
            if (randomNode >= nodes.size()) randomNode = 0;

            switch (std::rand() % 2)
            {
                case 2:
                    nodes[i].effects.push_back(&nodes[randomNode]);
                    break;
                case 1:
                    break;
                default:
                    break;
            }
        }

        std::vector<vks::VksModel::Vertex> vertices
        {
                {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}, // Bottom
                {{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
                {{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f}},
                {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
                {{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f}},
                {{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
                {{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}, // Top
                {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
                {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}},
                {{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
                {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}},
                {{0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
                {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}, // Front
                {{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
                {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}},
                {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
                {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}},
                {{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
                {{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}, // Back
                {{1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
                {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}},
                {{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
                {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}},
                {{0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
                {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}, // Left
                {{0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
                {{0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}},
                {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
                {{0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}},
                {{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
                {{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}, // Right
                {{1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
                {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}},
                {{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
                {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}},
                {{1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}
        };
        model = new vks::VksModel(device, vertices);

    }

    virtual void draw(IRenderProvider &renderProvider)
    {
        glm::mat4 transform;
        renderProvider.bindModel(*model);
        for(int i=0; i<nodes.size(); i++)
        {
            transform = glm::translate(glm::mat4(1), nodes[i].position);
            renderProvider.bindModelTransform(transform);
            renderProvider.drawModel();
        }
    }
private:
    std::vector<Node> nodes;
    vks::VksModel *model;
};

#endif //VULKANENGINE_GRAPH_H
