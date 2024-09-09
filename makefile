# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++17

# Include paths
INCLUDES = -I"/Users/hyunsoo/VulkanSDK/1.3.290.0/macOS/include" \
           -I/opt/homebrew/Cellar/glfw/3.4/include \
           -I/opt/homebrew/Cellar/glm/1.0.1/include

# Library paths and libraries
LIBS = -L"/Users/hyunsoo/VulkanSDK/1.3.290.0/macOS/lib" \
       -L/opt/homebrew/Cellar/glfw/3.4/lib \
       -lglfw -lvulkan \
       -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo

# Source files
SRCS = main.cpp

# Output executable name
TARGET = app

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
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@ $(LIBS)

debug:
	$(MAKE) MODE=debug

release:
	$(MAKE) MODE=release

clean:
	rm -f $(TARGET) $(TARGET)_debug

re: clean all

.PHONY: all debug release clean re