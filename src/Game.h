//
// Created by magnias on 20/10/20.
//

#ifndef VULKANENGINE_GAME_H
#define VULKANENGINE_GAME_H


#include <graphics/Window.h>
#include <graphics/Vulkan.h>

class Game : vks::Renderable
{
public:
    Game();
    ~Game();
    void run();

private:
    Window window;
    Vulkan vulkan;
    void update();
    void render();
    void draw(VkCommandBuffer cmdbuffer, VkPipelineLayout pipelineLayout, int index) override;

    void createUniformBuffers();

    void destroy();

    void createDescriptorSets(int size);

    void createDescriptorSetLayout();

    void createDescriptorPool(int size);

    void updateUniformBuffer(uint32_t currentImage);

    void createUniformBuffers(int size);
};


#endif //VULKANENGINE_GAME_H
