#ifndef CAMERA
#define CAMERA

#include "vulkan/vulkan.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "UBO.hpp"
#include "AppHandler.hpp"
#include "DescriptorSet.hpp"

//STL
#include <array>
#include <assert.h>

//hight abstraction level of a camera object
struct Camera
{
	struct CamUBO
	{
		alignas(16) glm::vec3 pos = { 0.f, 0.f, 0.f };
		alignas(16) glm::vec3 up = {0.f, 0.f, 1.f};
		alignas(16) glm::vec3 front = { 1.f, 0.f, 0.f };
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
	};


	CamUBO m_data{};
	UBO_buffmem m_camBuffMem;

	std::vector<VkDescriptorSet> m_descrptrSet;

	void init(const unsigned char numberOfFrame);

	void clean();

	void updateBuffer(uint16_t imageIndex);

	void updateCamPosition(glm::vec3 newPos);

	void moveCameraOf(glm::vec3 deplacement);

	void lookAt(glm::vec3 Center);

	void rotateCamera(glm::vec3 axis, float angle);

	//static stuff
	static VkDescriptorPool CameraDescriptorPool;
	static DeviceHandler CameraDevh;
	static VkDescriptorSetLayout CameraDescriptorSetLayout;

	static void InitCamerasClass(uint32_t NumberOfImageToBind, DeviceHandler devh);

	static void CleanCameraClass();
};
#endif