# VulkanEngine

This is my try at learning C++ and Vulkan at the same time. Do not expect much from this project.

## status
- [ ] Implement Vulkan
  - [x] ~~Basic rendering setup~~
  - [x] ~~Vulkan code splitted up (_device, swapchain, pipeline)~~
  - [x] ~~Implement resizing without crashing~~  
  - [x] ~~Add descriptor sets/pools + move imgui pool uit of main => swapchain?~~
  - [x] ~~Add uniform buffers~~
  - [ ] Add index buffers
  - [ ] Add model loading
  - [ ] Add push constants
  - [ ] Add texture mapping
  - [ ] Add framebuffer utilities
  - [ ] Use initializers to clean up vulkan code
  - [ ] Load shaders dynamically
- [ ] Add logging library (spdlog)
  - [x] ~~Log basic vulkan setup~~
  - [ ] Log advanced vulkan status
- [ ] Game logic
  - [ ] Add camera utilities
- [ ]  Refactoring
  - [ ] Empty out app.hpp
  - [ ] Remove unused code
  - [ ] Clean up cmakelists

## Notes
[notes](./NOTES.md)

## Installation

Required dependencies
- `yay -S vulkan-devel boost tinyobjloader glm glfw-x11`

Build:
1. `$ cd build`
2. `$ cmake ..`
3. `$ make`

## Running

Launch the `Vulkanengine` application in build/
