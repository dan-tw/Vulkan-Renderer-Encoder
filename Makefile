CXX := g++
CFLAGS := -std=c++17 -O2
LDFLAGS := -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

SRC := $(wildcard src/*.cpp)
# Translates src/main.cpp → build/main.o
OBJ := $(patsubst src/%.cpp, build/%.o, $(SRC))
TARGET := VulkanTest

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

build/%.o: src/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: test clean

test: VulkanTest
	./VulkanTest

clean:
	rm -f $(OBJ) $(TARGET)
