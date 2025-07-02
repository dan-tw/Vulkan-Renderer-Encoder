MODE ?= debug
ifeq ($(MODE), release)
    CXXFLAGS := -std=c++17 -O2 -DNDEBUG
else
    CXXFLAGS := -std=c++17 -g -Wall -Wextra
endif

CXX := g++
LDFLAGS := -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

$(info MODE=$(MODE), CXXFLAGS=$(CXXFLAGS))

SRC := $(wildcard src/*.cpp)
# Translates src/main.cpp → build/main.o
OBJ := $(patsubst src/%.cpp, build/%.o, $(SRC))
TARGET := VulkanTest

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

build/%.o: src/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: test clean rebuild

test: VulkanTest
	./VulkanTest

clean:
	rm -f $(OBJ) $(TARGET)

format:
	clang-format -i $(wildcard src/*.cpp src/*.hpp)

rebuild: clean VulkanTest
