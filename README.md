## xrRNG

This is an attempt to create a renderer which is compatible with [OpenXRay](https://github.com/OpenXRay/xray-16) engine but uses Vulkan in back end. The project is trying to mimic vanilla renderers interface, but implements modern features like GPU occlusion culling and tiled/clustered shading techniques. The task is quite challenging since even latest R4 renderer heavy relies on immediate DX11 mode so whole architecture re-design is required.

Some early attempts to implement a straight forward approach can be found [here](https://github.com/vTurbine/xrRNG/tree/master) and kept for history purposes.

### Build instructions

You need to add the `xrRNG` into your solution file. That's it. CMake support will be added later. 

### Third party

List of wonderful libraries used in the project:

* `LunarG Vulkan SDK` for Vulkan loader, headers, static libraries and validation layers
* AMD's ``VMA`` for low level memory allocation
