//
// Created by magnias on 26/03/2021.
//

#ifndef VULKANENGINE_GAME_ENTITY_H
#define VULKANENGINE_GAME_ENTITY_H

#include "../../vks/vks_model.h"
#include "../graphics/render_manager.h"

class Entity : public IRenderable
{
public:

    Entity(vks::VksDevice &device)
    {
        std::vector<vks::VksModel::Vertex> vertices{
                {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
                {{1.0f,  0.0f, 0.0f}, {1.0f,  0.0f, 0.0f}},
                {{1.0f,  0.0f, 1.0f},  {1.0f,  1.0f,  0.0f}},
                {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
                {{1.0f,  0.0f, 1.0f},  {1.0f,  1.0f,  0.0f}},
                {{0.0f, 0.0f, 1.0f},  {0.0f, 1.0f,  0.0f}},
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

    void setScale(glm::vec3 scale)
    {
        _scale = scale;
    }

    glm::mat4 calculateTransform()
    {
        glm::mat4 transform =  glm::translate(glm::mat4(1), position);
        glm::scale(transform, _scale);
        return transform;
    }


private:
    vks::VksModel *model;
    glm::vec3 position;
    glm::vec3 _scale = glm::vec3(1);
};

#endif //VULKANENGINE_GAME_ENTITY_H
