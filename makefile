# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++17

# Vulkan SDK path
VULKAN_SDK = /Users/hyunsoo/VulkanSDK/1.3.290.0

# Include paths
INCLUDES = -I"$(VULKAN_SDK)/macOS/include" \
           -I/opt/homebrew/Cellar/glfw/3.4/include \
           -I/opt/homebrew/Cellar/glm/1.0.1/include
# Library paths and libraries
LIBS = -L"$(VULKAN_SDK)/macOS/lib" \
       -L/opt/homebrew/Cellar/glfw/3.4/lib \
       -lglfw -lvulkan \
       -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo
# Source files
SRCS = main.cpp

# Output executable name
TARGET = app

# Shader files
VERT_SHADER = shaders/shader.vert
FRAG_SHADER = shaders/shader.frag
VERT_SPV = shaders/vert.spv
FRAG_SPV = shaders/frag.spv
GLSLC = $(VULKAN_SDK)/macOS/bin/glslc

# Debug flags
DEBUG_FLAGS = -g -DDEBUG

# Release flags
RELEASE_FLAGS = -O3 -DNDEBUG

# Default to release mode
MODE ?= release

ifeq ($(MODE),debug)
    CXXFLAGS += $(DEBUG_FLAGS)
    TARGET := $(TARGET)_debug
else
    CXXFLAGS += $(RELEASE_FLAGS)
endif

# Targets
all: compile_shaders $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@ $(LIBS)

compile_shaders: $(VERT_SPV) $(FRAG_SPV)

$(VERT_SPV): $(VERT_SHADER)
	$(GLSLC) $< -o $@

$(FRAG_SPV): $(FRAG_SHADER)
	$(GLSLC) $< -o $@

debug:
	$(MAKE) MODE=debug

release:
	$(MAKE) MODE=release

clean:
	rm -f $(TARGET) $(TARGET)_debug shaders/*.spv
    
re: clean all

.PHONY: all debug release compile_shaders clean re