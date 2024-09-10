#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include <optional>
#include <set>
#include <vector>

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

// 디버그 메신저 생성을 위한 프록시 함수
VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger) {
  // vkGetInstanceProcAddr를 사용하여 함수 포인터를 가져옵니다.
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) {
    // 함수가 존재하면 호출하여 디버그 메신저를 생성합니다.
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    // 함수가 없으면 확장이 없음을 나타내는 에러를 반환합니다.
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

// 디버그 메신저 파괴를 위한 프록시 함수
void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks* pAllocator) {
  // vkGetInstanceProcAddr를 사용하여 함수 포인터를 가져옵니다.
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) {
    // 함수가 존재하면 호출하여 디버그 메신저를 파괴합니다.
    func(instance, debugMessenger, pAllocator);
  }
}

class HelloTriangleApplication {
 public:
  void run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
  }

 private:
  GLFWwindow* window;
  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
      return graphicsFamily.has_value() && presentFamily.has_value();
    }
  };
  VkDevice device;
  VkQueue graphicsQueue;
  VkQueue presentQueue;
  VkSurfaceKHR surface;

  void initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
  }

  void initVulkan() {
    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
  }
  void createSurface() {
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create window surface!");
    }
    // std::cout << "In FUNCTION : createSurface" << std::endl;
    // std::cout << "Surface created successfully." << std::endl;
  }
  void createLogicalDevice() {
    // 물리적 장치에서 큐 패밀리 인덱스 찾기
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
                                              indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
      VkDeviceQueueCreateInfo queueCreateInfo{};
      queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queueCreateInfo.queueFamilyIndex = queueFamily;
      queueCreateInfo.queueCount = 1;
      queueCreateInfo.pQueuePriorities = &queuePriority;
      queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount =
        static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = 0;

    if (enableValidationLayers) {
      createInfo.enabledLayerCount =
          static_cast<uint32_t>(validationLayers.size());
      createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
      createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create logical device!");
    }
    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);

    /*
    std::cout << "In FUNCTION : createLogicalDevice" << std::endl;
    std::cout << "Logical device created successfully." << std::endl;
    std::cout << "Graphics Queue: " << graphicsQueue << std::endl;
    std::cout << "Present Queue: " << presentQueue << std::endl;
    */
  }

  void pickPhysicalDevice() {
    // 추가: 물리적 장치 선택 로직 구현
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    std::cout << "device count: " << deviceCount << std::endl;
    if (deviceCount == 0) {
      throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const auto& device : devices) {
      std::cout << "Device: " << device << std::endl;
      if (isDeviceSuitable(device)) {
        std::cout << "Found suitable device!" << std::endl;
        physicalDevice = device;
        break;
      }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
      throw std::runtime_error("failed to find a suitable GPU!");
    }
  }
  // 추가: 장치의 적합성을 검사하는 함수
  bool isDeviceSuitable(VkPhysicalDevice device) {
    QueueFamilyIndices indices = findQueueFamilies(device);

    // std::cout << "IN FUNCTION : isDeviceSuitable" << std::endl;
    // std::cout << "Device suitability check:" << std::endl;
    // std::cout << "  Graphics Family: "
    //           << (indices.graphicsFamily.has_value() ? "Found" : "Not Found")
    //           << std::endl;
    // std::cout << "  Present Family: "
    //           << (indices.presentFamily.has_value() ? "Found" : "Not Found")
    //           << std::endl;

    return indices.isComplete();
    /*
    GPU 정보 출력
    std::cout << "Device Name : " << deviceProperties.deviceName << std::endl;
    std::cout << "Device Type :  " << deviceProperties.deviceType << std::endl;
    std::cout << "Device limits.maxImageDimension2D : "
              << deviceProperties.limits.maxImageDimension2D << std::endl;
    std::cout << "Device geometryShader : " << deviceFeatures.geometryShader
              << std::endl;
    std::cout << "Device tessellationShader : "
              << deviceFeatures.tessellationShader << std::endl;
    std::cout << "Device shaderFloat64 : " << deviceFeatures.shaderFloat64
              << std::endl;

    // 컴퓨트 셰이더 관련 정보 출력
    std::cout << "Max Compute Shared Memory Size: "
              << deviceProperties.limits.maxComputeSharedMemorySize
              << std::endl;
    std::cout << "Max Compute Work Group Count: "
              << deviceProperties.limits.maxComputeWorkGroupCount[0] << ", "
              << deviceProperties.limits.maxComputeWorkGroupCount[1] << ", "
              << deviceProperties.limits.maxComputeWorkGroupCount[2]
              << std::endl;
    std::cout << "Max Compute Work Group Invocations: "
              << deviceProperties.limits.maxComputeWorkGroupInvocations
              << std::endl;
    std::cout << "Max Compute Work Group Size: "
              << deviceProperties.limits.maxComputeWorkGroupSize[0] << ", "
              << deviceProperties.limits.maxComputeWorkGroupSize[1] << ", "
              << deviceProperties.limits.maxComputeWorkGroupSize[2]
              << std::endl;
    */
    /*
    Apple m1 을 위한 조건 완화, geometryShader가 없음
    return deviceProperties.deviceType ==
        VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
        deviceFeatures.geometryShader;
    */
    return true;
  }

  void mainLoop() {
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();
    }
  }

  // 리소스 정리 함수
  void cleanup() {
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);  // 추가: surface 정리

    if (enableValidationLayers) {
      DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();
  }

  void createInstance() {
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
      createInfo.enabledLayerCount =
          static_cast<uint32_t>(validationLayers.size());
      createInfo.ppEnabledLayerNames = validationLayers.data();

      populateDebugMessengerCreateInfo(debugCreateInfo);
      createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    } else {
      createInfo.enabledLayerCount = 0;
      createInfo.pNext = nullptr;
    }
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
      throw std::runtime_error("failed to create instance!");
    }
    /*
    // 가능한 확장 알아보기
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
    extensions.data()); std::cout << "available extensions:\n";

    for (const auto& extension : extensions) {
        std::cout << '\t' << extension.extensionName << '\n';
    }
    */
  }
  void setupDebugMessenger() {
    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr,
                                     &debugMessenger) != VK_SUCCESS) {
      throw std::runtime_error("failed to set up debug messenger!");
    }
  }
  // 디버그 메신저 생성 정보 설정 함수
  void populateDebugMessengerCreateInfo(
      VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
  }

  std::vector<const char*> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions,
                                        glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
      extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

    return extensions;
  }

  // 유효성 검사 계층 지원 확인 함수
  bool checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
      bool layerFound = false;

      for (const auto& layerProperties : availableLayers) {
        if (strcmp(layerName, layerProperties.layerName) == 0) {
          layerFound = true;
          break;
        }
      }

      if (!layerFound) {
        return false;
      }
    }

    return true;
  }
  // 디버그 콜백 함수
  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                void* pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
  }

  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                             nullptr);
    std::cout << "queueFamilyCount: " << queueFamilyCount << std::endl;

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                             queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
      /*
       std::cout << "Queue Family " << i << ":" << std::endl;
       std::cout << "  Queue Count: " << queueFamily.queueCount << std::endl;
       std::cout << "  Supported Operations:" << std::endl;
       if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
         std::cout << "    Graphics" << std::endl;
       if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
         std::cout << "    Compute" << std::endl;
       if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
         std::cout << "    Transfer" << std::endl;
       if (queueFamily.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
         std::cout << "    Sparse Binding" << std::endl;
       if (queueFamily.queueFlags & VK_QUEUE_PROTECTED_BIT)
         std::cout << "    Protected" << std::endl;
       std::cout << "  Timestamp Valid Bits: " << queueFamily.timestampValidBits
                 << std::endl;
       std::cout << "  Min Image Transfer Granularity: "
                 << queueFamily.minImageTransferGranularity.width << "x"
                 << queueFamily.minImageTransferGranularity.height << "x"
                 << queueFamily.minImageTransferGranularity.depth << std::endl;
      */

      if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        indices.graphicsFamily = i;
      }
      VkBool32 presentSupport = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
      if (presentSupport) {
        indices.presentFamily = i;
      }
      if (indices.isComplete()) {
        break;
      }

      i++;
    }

    return indices;
  }
};

int main() {
  HelloTriangleApplication app;

  try {
    app.run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}