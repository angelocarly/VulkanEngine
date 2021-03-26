#ifndef VULKANENGINE_APP_H
#define VULKANENGINE_APP_H

#pragma once

#include "core/game.hpp"

const uint32_t WIDTH = 1600;
const uint32_t HEIGHT = 900;

using namespace vks;

class App
{
public:

    void run()
    {
        mainLoop();
    }

private:

    VksWindow window = VksWindow(WIDTH, HEIGHT, "VulkanEngine");

    // Delta milliseconds
    float deltaTime;
    std::chrono::high_resolution_clock::time_point lastFrameTime = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point currentFrameTime = std::chrono::high_resolution_clock::now();

    // fps
    int fps;
    int fpsCounter;
    std::chrono::milliseconds lastFpsUpdate =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()
            );

    /*
     * Main game loop, polls and updates the window and executes the game logic
     */
    void mainLoop()
    {
        Game game = Game(window);
        game.init();
        spdlog::info("Game initialized");

        while (!window.shouldClose())
        {
            glfwPollEvents();

            updateDeltaTime();
            updateFps();

            game.update(deltaTime);
        }

        spdlog::info("Shutting down");
        game.destroy();
    }

    /**
     * Record the duration of the last frame in seconds
     */
    void updateDeltaTime()
    {
        currentFrameTime = std::chrono::high_resolution_clock::now();
        deltaTime =
                std::chrono::duration_cast<std::chrono::microseconds>(currentFrameTime - lastFrameTime).count() /
                1000000.0f;
        lastFrameTime = currentFrameTime;
    }

    /**
     * Record the amount of frames that were rendered per second
     */
    void updateFps()
    {
        fpsCounter++;
        std::chrono::milliseconds newFpsUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
        );
        if (newFpsUpdate - lastFpsUpdate >= std::chrono::milliseconds(1000))
        {
            fps = fpsCounter;
            spdlog::info("Fps: {}", fps);
            lastFpsUpdate = newFpsUpdate;
            fpsCounter = 0;
        }
    }
};

#endif //VULKANENGINE_APP_H
