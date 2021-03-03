//
// Created by magnias on 28/02/2021.
//

#include "app.hpp"

namespace vks
{

    void App::loadModel() {
        std::vector<VksModel::Vertex> vertices {
                {{0.0f, -1.0f}, {1.0f, 0.0f, 0.0f}},
                {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
        };

        vksModel = std::make_unique<VksModel>(device, vertices);
    }
}
