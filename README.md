# VulkanEngine

This is my try at learning C++ and Vulkan at the same time. Do not expect much from this project.

## TODO
- Safely delete ImGUI objects on cleanup
- Remove unused code
- Implement resizing without crashing  
- Clean up cmakelists
- Use initializers

## Notes
**Descriptors**  
- Descriptor: special opaque shader variable to access buffer and image resources indirectly.
- Descriptor set: Array of homogenous resources with same layout binding
- Descriptor pool: a pool of useable descriptor sets 
- Descriptor set layout: describes descriptor set
- Pipeline layout: contain list of descriptor set layouts

## Installation

Required dependencies
- `yay -S vulkan-devel boost tinyobjloader glm glfw-x11`

Build:
1. `$ cd build`
2. `$ cmake ..`
3. `$ make`

## Running

Launch the `Vulkanengine` application in build/
