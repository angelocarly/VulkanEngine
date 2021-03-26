# VulkanEngine

This is my try at learning C++ and Vulkan at the same time. Do not expect much from this project.

## status
- [ ] Core
  - [ ] Make sure all pointers are deleted after usage
- [ ]  Refactoring
  - [x] ~~Empty out app.hpp~~
  - [x] ~~Clean up cmakelists~~
  - [ ] Move rendering specific code out of game.hpp
  - [ ] Remove unused code
- [ ] Implement Vulkan
  - [x] ~~Basic rendering setup~~
  - [x] ~~Vulkan code splitted up (_device, swapchain, pipeline)~~
  - [x] ~~Implement resizing without crashing~~  
  - [x] ~~Add descriptor sets/pools + move imgui pool uit of main => swapchain?~~
  - [x] ~~Add uniform buffers~~
  - [x] ~~Add push constants~~
  - [ ] Add index buffers
  - [ ] Add model loading
  - [ ] Add texture mapping
  - [ ] Add framebuffer utilities
  - [ ] Use initializers to clean up vulkan code
  - [ ] Load shaders dynamically
- [ ] Add logging library (spdlog)
  - [x] ~~Log basic vulkan setup~~
  - [ ] Log advanced vulkan status
- [ ] Game logic
  - [x] ~~Add camera utilities~~

## Notes
[notes](./NOTES.md)

## Installation

Required dependencies
- `yay -S vulkan-devel boost tinyobjloader glm glfw-x11`

Build:
1. `$ mkdir build`
2. `$ cd build`
3. `$ cmake ..`
4. `$ make`

## Running

Launch the `Vulkanengine` application in build/
