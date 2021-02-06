#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <cstdlib>
#include <optional>
#include "app.hpp"

int main()
{
    vks::App app;

    try
    {
        app.run();
    } catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}