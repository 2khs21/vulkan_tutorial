# all : 
# 	g++ main.cpp -o vulkan_app -std=c++17 -I"/Users/hyunsoo/VulkanSDK/1.3.290.0/macOS/include" -I/opt/homebrew/Cellar/glfw/3.4/include -I/opt/homebrew/Cellar/glm/1.0.1/include -L"/Users/hyunsoo/VulkanSDK/1.3.290.0/macOS/lib" -L/opt/homebrew/Cellar/glfw/3.4/lib -lglfw -lvulkan -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo

CFLAGS = -std=c++17
FLAGS = -I"/Users/hyunsoo/VulkanSDK/1.3.290.0/macOS/include" -I/opt/homebrew/Cellar/glfw/3.4/include -I/opt/homebrew/Cellar/glm/1.0.1/include -L"/Users/hyunsoo/VulkanSDK/1.3.290.0/macOS/lib" -L/opt/homebrew/Cellar/glfw/3.4/lib -lglfw -lvulkan -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo

SRCS = main.cpp

all: $(SRCS)
	g++ main.cpp -o app $(CFLAGS) $(FLAGS)

clean:
	rm -f app

re:
	make clean
	make all

.PHONY: all clean re