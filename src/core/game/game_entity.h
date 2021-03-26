//
// Created by magnias on 26/03/2021.
//

#ifndef VULKANENGINE_GAME_ENTITY_H
#define VULKANENGINE_GAME_ENTITY_H

#include <vks/vks_model.h>
#include <core/graphics/render_manager.h>

class Entity : public IRenderable
{
public:

    Entity(vks::VksDevice &device)
    {
        std::vector<vks::VksModel::Vertex> vertices{
                {{-1.0f, 0.0f, -1.0f}, {-1.0f, -1.0f, 0.0f}},
                {{1.0f,  0.0f, -1.0f}, {1.0f,  -1.0f, 0.0f}},
                {{1.0f,  0.0f, 1.0f},  {1.0f,  1.0f,  0.0f}},
                {{-1.0f, 0.0f, -1.0f}, {-1.0f, -1.0f, 0.0f}},
                {{1.0f,  0.0f, 1.0f},  {1.0f,  1.0f,  0.0f}},
                {{-1.0f, 0.0f, 1.0f},  {-1.0f, 1.0f,  0.0f}}
        };

//        vksModel = std::make_unique<VksModel>(device, vertices);
        model = new vks::VksModel(device, vertices);

        position = glm::vec3(
                std::rand() % 1000 / 100.0f - 5,
                std::rand() % 1000 / 100.0f - 5,
                std::rand() % 1000 / 100.0f - 5
        );
    }

    void draw(IRenderProvider &renderProvider) override
    {
        renderProvider.bindModel(*model);
        renderProvider.bindModelTransform(calculateTransform());
        renderProvider.drawModel();
    }

    void draw2(IRenderProvider &renderProvider)
    {
        renderProvider.bindModelTransform(calculateTransform());
        renderProvider.drawModel();
    }

    glm::mat4 calculateTransform()
    {
        return glm::translate(glm::mat4(1), position);
    }

    vks::VksModel *model;
    glm::vec3 position;

private:
};

#endif //VULKANENGINE_GAME_ENTITY_H
