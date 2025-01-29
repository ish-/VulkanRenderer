#pragma once
#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <stdexcept>
#include <vector>
using std::vector;
#include <set>
using std::set;
#include <array>
#include "stb_image.h"
using std::array;

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "VulkanMeshModel.h"

#include "VulkanUtilities.h"
#include "VulkanMesh.h"

struct ViewProjection
{
	glm::mat4 projection;
	glm::mat4 view;
};

class VulkanRenderer
{
public:
#ifdef NDEBUG
	static const bool enableValidationLayers = false;
#else
	static const bool enableValidationLayers = true;
#endif
	static const vector<const char*> validationLayers;

	VulkanRenderer();
	~VulkanRenderer();

	int init(GLFWwindow* windowP);
	void draw();
	void clean();

	void updateModel(int modelId, glm::mat4 modelP);
	int createMeshModel(const std::string& filename);
	void createColorBufferImage();

private:
	GLFWwindow* window;
	vk::Instance instance;
	vk::Queue graphicsQueue;			// Handles to queue (no value stored)
	VkDebugUtilsMessengerEXT debugMessenger;
	
	struct {
		vk::PhysicalDevice physicalDevice;
		vk::Device logicalDevice;
	} mainDevice;

	vk::SurfaceKHR surface;
	vk::Queue presentationQueue;
	vk::SwapchainKHR swapchain;
	vk::Format swapchainImageFormat;
	vk::Extent2D swapchainExtent;
	vector<SwapchainImage> swapchainImages;

	vk::PipelineLayout pipelineLayout;
	vk::RenderPass renderPass;
	vk::Pipeline graphicsPipeline;

	vector<vk::Framebuffer> swapchainFramebuffers;
	vk::CommandPool graphicsCommandPool;
	vector<vk::CommandBuffer> commandBuffers;

	vector<vk::Semaphore> imageAvailable;
	vector<vk::Semaphore> renderFinished;
	const int MAX_FRAME_DRAWS = 2;			// Should be less than the number of swapchain images, here 3 (could cause bugs)
	int currentFrame = 0;
	vector<vk::Fence> drawFences;

	vector<VulkanMesh> meshes;

	vk::DescriptorSetLayout descriptorSetLayout;
	vector<vk::Buffer> vpUniformBuffer;
	vector<vk::DeviceMemory> vpUniformBufferMemory;
	vk::DescriptorPool descriptorPool;
	vector<vk::DescriptorSet> descriptorSets;
	
	
	ViewProjection viewProjection;
	vk::DeviceSize minUniformBufferOffet;
	size_t modelUniformAlignement;
	Model* modelTransferSpace;
	const int MAX_OBJECTS = 20;
	vector<vk::Buffer> modelUniformBufferDynamic;
	vector<vk::DeviceMemory> modelUniformBufferMemoryDynamic;

	vk::PushConstantRange pushConstantRange;

	vk::Image depthBufferImage;
	vk::DeviceMemory depthBufferImageMemory;
	vk::ImageView depthBufferImageView;
	vector<VkImage> textureImages;
	vector<vk::ImageView> textureImageViews;
	vector<VkDeviceMemory> textureImageMemory;
	vk::Sampler textureSampler;
	vk::DescriptorPool samplerDescriptorPool;
	vk::DescriptorSetLayout samplerDescriptorSetLayout;
	vector<vk::DescriptorSet> samplerDescriptorSets;
	vector<VulkanMeshModel> meshModels;
	vk::SampleCountFlagBits msaaSamples{ vk::SampleCountFlagBits::e1 };

	vk::Image colorImage;
	vk::DeviceMemory colorImageMemory;
	vk::ImageView colorImageView;
	
	// Instance
	void createInstance();
	bool checkInstanceExtensionSupport(const vector<const char*>& checkExtensions);
	bool checkValidationLayerSupport();
	vector<const char*> getRequiredExtensions();

	// Debug
	void setupDebugMessenger();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

	// Devices
	void getPhysicalDevice();
	bool checkDeviceSuitable(vk::PhysicalDevice device);
	QueueFamilyIndices getQueueFamilies(vk::PhysicalDevice device);
	void createLogicalDevice();

	// Surface and swapchain
	vk::SurfaceKHR createSurface();
	bool checkDeviceExtensionSupport(vk::PhysicalDevice device);
	SwapchainDetails getSwapchainDetails(vk::PhysicalDevice device);
	void createSwapchain();
	vk::SurfaceFormatKHR chooseBestSurfaceFormat(const vector<vk::SurfaceFormatKHR>& formats);
	vk::PresentModeKHR chooseBestPresentationMode(const vector<vk::PresentModeKHR>& presentationModes);
	vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& surfaceCapabilities);
	vk::ImageView createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlagBits aspectFlags, uint32_t mipLevels);

	// Graphics pipeline
	void createGraphicsPipeline();
	vk::ShaderModule createShaderModule(const vector<char>& code);
	void createRenderPass();

	// Buffers
	void createFramebuffers();
	void createGraphicsCommandPool();
	void createGraphicsCommandBuffers();
	void recordCommands(uint32_t currentImage);

	// Descriptor sets
	void createDescriptorSetLayout();
	void createUniformBuffers();
	void createDescriptorPool();
	void createDescriptorSets();
	void updateUniformBuffers(uint32_t imageIndex);

	// Data alignment and dynamic buffers
	void allocateDynamicBufferTransferSpace();

	// Push constants
	void createPushConstantRange();

	// Depth
	void createDepthBufferImage();
	stbi_uc* loadTextureFile(const std::string& filename, int* width, int* height, vk::DeviceSize* imageSize);
	int createTextureImage(const std::string& filename, uint32_t& mipLevels);
	int createTexture(const std::string& filename);
	
	vk::Image createImage(uint32_t width, uint32_t height, uint32_t mipLevels,vk::SampleCountFlagBits numSamples, vk::Format format,
		vk::ImageTiling tiling,vk::ImageUsageFlags useFlags, vk::MemoryPropertyFlags propFlags,vk::DeviceMemory* imageMemory);
	
	vk::Format chooseSupportedFormat(const vector<vk::Format>& formats, vk::ImageTiling tiling, vk::FormatFeatureFlags featureFlags);
	void createTextureSampler();
	int createTextureDescriptor(vk::ImageView textureImageView);

	// Draw
	void createSynchronisation();

};
