//
// Created by magnias on 20/10/20.
//

#ifndef VULKANENGINE_GAME_H
#define VULKANENGINE_GAME_H


#include <graphics/Window.h>
#include <graphics/Vulkan.h>

class Game
{
public:
    Game();
    void run();

private:
    Window window;
    Vulkan vulkan;
    void update();
    void render();
};


#endif //VULKANENGINE_GAME_H
