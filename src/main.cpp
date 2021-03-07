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

void setup_logging()
{
    spdlog::set_level(spdlog::level::debug);
    spdlog::stdout_color_mt("vulkan");
    // create color multi threaded logger
//    auto console = spdlog::stdout_color_mt("console");
//    auto err_logger = spdlog::stderr_color_mt("stderr");
//    spdlog::get("console");
//    c->info("loggers can be retrieved from a global registry using the spdlog::get(logger_name)");
//    spdlog::get("console")->
//    console->
}
