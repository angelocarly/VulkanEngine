#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <cstdlib>
#include <optional>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "app.hpp"

void setup_logging();

int main()
{

    setup_logging();
    App app;

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

void setup_logging()
{
    spdlog::set_level(spdlog::level::debug);
    spdlog::stdout_color_mt("vulkan");
}
