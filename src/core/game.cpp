//
// Created by magnias on 28/02/2021.
//

#include "game.hpp"

void Game::loadModel()
{
    std::vector<VksModel::Vertex> vertices{
            {{-1.0f, 0.0f, -1.0f}, {1.0f, 0.0f, 0.0f}},
            {{1.0f,  0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},
            {{1.0f,  0.0f, 1.0f},  {0.0f, 0.0f, 1.0f}},
            {{-1.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 1.0f}},
            {{1.0f,  0.0f, 1.0f},  {0.0f, 0.0f, 1.0f}},
            {{-1.0f, 0.0f, 0.9f},  {0.0f, 1.0f, 0.0f}}
    };

    vksModel = std::make_unique<VksModel>(device, vertices);
}
