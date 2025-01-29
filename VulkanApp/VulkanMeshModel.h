#pragma once
#include <glm.hpp>
#include <assimp/scene.h>
#include <vector>
#include "VulkanMesh.h"
#include <string>
using std::string;
using std::vector;

class VulkanMeshModel
{
public:
	VulkanMeshModel();
	VulkanMeshModel(vector<VulkanMesh> meshesP);
	~VulkanMeshModel();
	size_t getMeshCount() const { return meshes.size(); };
	VulkanMesh* getMesh(size_t index);
	glm::mat4 getModel() const { return model; };
	void setModel(glm::mat4 modelP) { model = modelP; }
	void destroyMeshModel();
	static vector<string> loadMaterials(const aiScene* scene);

	static VulkanMesh loadMesh(vk::PhysicalDevice physicalDeviceP,
		vk::Device deviceP, vk::Queue transferQueue,
		vk::CommandPool transferCommandPool,
		aiMesh* mesh, const aiScene* scene, vector<int> matToTex);
	
	static vector<VulkanMesh> loadNode(vk::PhysicalDevice physicalDeviceP,
		vk::Device deviceP, vk::Queue transferQueue,
		vk::CommandPool transferCommandPool,	
		aiNode* node, const aiScene* scene, vector<int> matToTex);
	
private:
	vector<VulkanMesh> meshes;
	glm::mat4 model;
};

