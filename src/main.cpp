#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <cstdlib>
#include <optional>
#include "legacy.hpp"

int main()
{
    Legacy app;

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