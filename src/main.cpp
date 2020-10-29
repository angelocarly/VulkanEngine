#include "Game.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

int main()
{
//    HelloTriangleApplication app;
    Game game;

    try
    {
        game.run();
    } catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}