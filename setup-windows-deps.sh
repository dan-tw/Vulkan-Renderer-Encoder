#!/bin/bash

set -e

# Base relative directory for dependencies
PROJECT_ROOT="$(pwd)"
BASE_DIR="$PROJECT_ROOT/thirdparty"
GLFW_DIR="$BASE_DIR/glfw-win64"
GLFW_BUILD_DIR="$GLFW_DIR/build"
GLFW_INSTALL_DIR="$GLFW_DIR/install"
VULKAN_HEADERS_DIR="$BASE_DIR/Vulkan-Headers"
VULKAN_HEADERS_BUILD_DIR="$VULKAN_HEADERS_DIR/build"
VULKAN_HEADERS_INSTALL_DIR="$VULKAN_HEADERS_DIR/install"
VULKAN_LOADER_DIR="$BASE_DIR/Vulkan-Loader"
VULKAN_LOADER_BUILD_DIR="$VULKAN_LOADER_DIR/build"
VULKAN_LOADER_INSTALL_DIR="$VULKAN_LOADER_DIR/install"

VULKAN_BUILD_DIR="$BASE_DIR/vulkan-win64-build"

# Ensure required tools are installed
command -v x86_64-w64-mingw32-g++ >/dev/null || {
    echo "Error: mingw-w64 toolchain not found. Install with: sudo apt install mingw-w64"
    exit 1
}
command -v cmake >/dev/null || {
    echo "Error: cmake not found. Install with: sudo apt install cmake"
    exit 1
}
command -v git >/dev/null || {
    echo "Error: git not found. Install with: sudo apt install git"
    exit 1
}
command -v make >/dev/null || {
    echo "Error: make not found. Install with: sudo apt install build-essential"
    exit 1
}

mkdir -p "$BASE_DIR"
cd "$BASE_DIR"

# Clone and build GLFW for Windows
if [[ ! -d "$GLFW_DIR" ]]; then
    echo "Cloning GLFW..."
    git clone https://github.com/glfw/glfw.git "$GLFW_DIR"
fi

cd $GLFW_DIR
mkdir -p $GLFW_INSTALL_DIR
mkdir -p $GLFW_BUILD_DIR
cd $GLFW_BUILD_DIR

echo "Configuring GLFW with CMake..."
cmake .. -G "Unix Makefiles" \
    -DCMAKE_SYSTEM_NAME=Windows \
    -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
    -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
    -DBUILD_SHARED_LIBS=OFF \
    -DCMAKE_INSTALL_PREFIX="$GLFW_INSTALL_DIR"

echo "Building GLFW..."
make -j$(nproc)

echo "Installing GLFW to $GLFW_INSTALL_DIR"
make install

# Back to base directory
cd "$BASE_DIR"

# Clone Vulkan-Headers if not present
if [[ ! -d "$VULKAN_HEADERS_DIR" ]]; then
    echo "Cloning Vulkan-Headers..."
    git clone https://github.com/KhronosGroup/Vulkan-Headers.git "$VULKAN_HEADERS_DIR"
fi

# Build and install Vulkan-Headers for Windows
cd $VULKAN_HEADERS_DIR
mkdir -p $VULKAN_HEADERS_INSTALL_DIR
mkdir -p $VULKAN_HEADERS_BUILD_DIR
cd $VULKAN_HEADERS_BUILD_DIR

echo "Configuring Vulkan-Headers with CMake..."
cmake .. -G "Unix Makefiles" \
    -DCMAKE_SYSTEM_NAME=Windows \
    -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
    -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
    -DCMAKE_INSTALL_PREFIX="$VULKAN_HEADERS_INSTALL_DIR"

echo "Building Vulkan-Headers..."
make -j$(nproc)

echo "Installing Vulkan-Headers to $VULKAN_HEADERS_INSTALL_DIR"
make install

# Clone Vulkan-Loader if not present
if [[ ! -d "$VULKAN_LOADER_DIR" ]]; then
    echo "Cloning Vulkan-Loader..."
    git clone https://github.com/KhronosGroup/Vulkan-Loader.git "$VULKAN_LOADER_DIR"
fi

# Create toolchain file dynamically
TOOLCHAIN_FILE="$PROJECT_ROOT/thirdparty/mingw-w64-toolchain.cmake"
cat > "$TOOLCHAIN_FILE" <<EOF
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)

# SET CMAKE_FIND ROOT PATHS TO FIND BINARIES OR COMPILERS
set(CMAKE_FIND_ROOT_PATH # COMPILER
    /usr/x86_64-w64-mingw32
    # VENDOR LIBS
)

# adjust the default behavior of the FIND_XXX() commands: search programs in the
# host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# search headers and libraries in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
EOF

echo "Created CMake toolchain file at $TOOLCHAIN_FILE"

# Build Vulkan-Loader for Windows (generates import libs)
cd $VULKAN_LOADER_DIR
mkdir -p $VULKAN_LOADER_INSTALL_DIR
mkdir -p $VULKAN_LOADER_BUILD_DIR
cd $VULKAN_LOADER_BUILD_DIR

echo "Configuring Vulkan-Loader with CMake..."
cmake .. -G "Unix Makefiles" \
    -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE" \
    -DBUILD_SHARED_LIBS=OFF \
    -DCMAKE_INSTALL_PREFIX="$VULKAN_LOADER_INSTALL_DIR" \
    -DVulkanHeaders_DIR="$VULKAN_HEADERS_INSTALL_DIR/share/cmake/VulkanHeaders"

echo "Building Vulkan-Loader..."
make -j$(nproc)

echo "Installing Vulkan-Loader to $VULKAN_LOADER_INSTALL_DIR"
make install

echo
echo "All done."
echo "GLFW built and installed to: $GLFW_INSTALL_DIR"
echo "Vulkan-Headers built and installed to: $VULKAN_HEADERS_INSTALL_DIR"
echo "Vulkan-Loader built and installed to: $VULKAN_LOADER_INSTALL_DIR"
