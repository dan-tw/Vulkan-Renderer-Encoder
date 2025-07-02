# === Configuration Options ===
TARGET_OS ?= linux
MODE ?= debug

TARGET := VulkanTest
SRC_DIR := src
BUILD_DIR := build

SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC))

# === Compiler and Flags ===
CXX := g++

CXXFLAGS := -std=c++17
ifeq ($(MODE), release)
    CXXFLAGS += -O2 -DNDEBUG
else
    CXXFLAGS += -Wall -Wextra -g
endif

INCLUDES :=
LIBDIRS :=
LDFLAGS :=

# === OS-specific Settings ===
ifeq ($(TARGET_OS), windows)
    CXX := x86_64-w64-mingw32-g++
    PROJECT_DIR := $(shell pwd)

    GLFW_DIR := $(PROJECT_DIR)/thirdparty/glfw-win64
    VULKAN_HEADERS_DIR := $(PROJECT_DIR)/thirdparty/Vulkan-Headers
    VULKAN_LIB_DIR := $(PROJECT_DIR)/thirdparty/Vulkan-Loader

    INCLUDES += -I$(GLFW_DIR)/include -I$(VULKAN_HEADERS_DIR)/include
    LIBDIRS += -L$(GLFW_DIR)/install/lib -L$(VULKAN_LIB_DIR)/install/lib
    LDFLAGS += $(LIBDIRS) -lglfw3 -lvulkan-1 -lgdi32 -luser32 -lkernel32 -static-libgcc -static-libstdc++
else
    LDFLAGS += -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
endif

# === Build Rules ===
$(info MODE=$(MODE), TARGET_OS=$(TARGET_OS), CXX=$(CXX), CXXFLAGS=$(CXXFLAGS))

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# === Utility Targets ===
.PHONY: test clean rebuild

test: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

rebuild: clean $(TARGET)
