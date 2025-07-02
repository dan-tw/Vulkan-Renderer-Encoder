
# Vulkan Renderer to H.264 Encoder

A real-time graphics application using Vulkan that renders frames on the GPU and encodes them into an H.264 video using GPU-accelerated encoders such as Vulkan Video. This simulates the workflow of a lightweight game capture or visualization pipeline.

This project exists to deepen my understanding of modern GPU programming and video processing. The project bridges **real-time rendering** with **hardware-accelerated video encoding**. Rather than relying on platform-specific solutions like NVENC, this project explores **Vulkan Video**, a cross-platform and vendor-neutral API for encoding video directly from GPU-rendered content.

This project provides a practical way to learn how to:
- Efficiently manage GPU memory and synchronization
- Integrate Vulkan rendering and encoding pipelines
- Work with the Vulkan Video extensions to achieve hardware-accelerated encoding on any supported GPU (NVIDIA, AMD, Intel)
- Build a cross-platform application that avoids vendor lock-in

## Development Environment

This project was built and tested on Linux (Ubuntu/Debian-based) with the following dependencies installed:

```bash
sudo apt update

# Vulkan core and extensions
sudo apt install libvulkan-dev vulkan-tools vulkan-utility-libraries-dev spirv-tools

# Windowing, math, and input support
sudo apt install libglfw3-dev libglm-dev libxi-dev libxxf86vm-dev
```

> Note: Ensure your GPU and drivers support Vulkan Video extensions (e.g. NVIDIA driver 535+ or compatible AMD/Intel driver). Use `vulkaninfo` to inspect extension support

## Building the Project

Ensure `g++` and `make` are installed. Then build the project with:

```bash
make
```

This will:
- Compile all `.cpp` files in the `src/` directory
- Link with Vulkan, GLFW, and other necessary libraries
- Produce a binary executable named `VulkanTest`

To run the compiled app:

```bash
make test
```

To clean build artifacts:
```bash
make clean
```

## Debugging

To debug or inspect Vulkan behavior, ensure the Vulkan SDK is installed. See the official guide: [LunarG Vulkan SDK - Getting Started on Ubuntu](https://vulkan.lunarg.com/doc/view/latest/linux/getting_started_ubuntu.html)

## Cross-compiling for Windows (from Linux)

Ensure that cross-compiler `MinGW-w64` is installed:

```bash
sudo apt-get install mingw-w64 gcc-mingw-w64-x86-64 g++-mingw-w64-x86-64
```

Build Windows dependencies (GLFW, Vulkan Headers, Vulkan Loader):

```bash
./setup-windows-deps.sh
```

Compile the project for Windows:

```bash
make TARGET_OS=windows MODE=release
```

This will produce a `VulkanTest.exe`, ready to run on Windows.
