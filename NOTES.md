# Vulkan Notes

## Descriptors
Represents resources in shaders. Allow us to use uniform buffers, storage buffers and images in GLSL.

- Descriptor: special opaque shader variable to access buffer and image resources indirectly.
- Descriptor set: Array of homogenous resources with same layout binding
- Descriptor set layout: describes descriptor set
- Descriptor pool: a pool of useable descriptor sets 
- Pipeline layout: contain list of descriptor set layouts

## Queue
Commands to the gpu are submitted to a queue. These are then executed when the device finds the time for it.

There are different kinds of queues on the devices. Not all support graphics operations. Other might only support
compute operations.

Groups of queues that have the same capabilities ( for example when they both support graphics and compute operations),
are grouped into queue families.

## Buffers
Memory binding on the device.