# VulkanEngine

This is my try at learning C++ and Vulkan at the same time. Do not expect much from this project.

## status
- [ ] Implement Vulkan
  - [x] ~~Basic rendering setup~~
  - [x] ~~Vulkan code splitted up (_device, swapchain, pipeline)~~
  - [ ] Implement resizing without crashing  
  - [ ] Add descriptor sets/pools + move imgui pool uit of main => swapchain?
  - [ ] Use initializers to clean up vulkan code
- [ ]  Refactoring
  - [ ] Remove unused code
  - [ ] Clean up cmakelists
- [ ] Add logging library (spdlog)

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
