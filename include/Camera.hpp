#ifndef CAMERA
#define CAMERA

#include "vulkan/vulkan.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "UBO.hpp"
#include "AppHandler.hpp"
#include "DescriptorSet.hpp"

#include "../Settings.hpp" //max_frame_in_flight

//STL
#include <array>

//hight abstraction level of a camera object
struct Camera
{
	struct CamUBO
	{
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
	};


	CamUBO m_data{};
	UBO_buffmem m_camBuffMem{};

	std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> m_descrptrSet;

	//static stuff
	static VkDescriptorPool CameraDescriptorPool;
	static DeviceHandler CameraDevh;
	static VkDescriptorSetLayout CameraDescriptorSetLayout;


	Camera()
	{}

	~Camera()
	{}


	void init()
	{
		m_camBuffMem = createUBO<CamUBO>(CameraDevh);
		m_descrptrSet = createDescriptorSets<CamUBO>(CameraDescriptorSetLayout, CameraDescriptorPool, CameraDevh.device, m_camBuffMem);
	}

	void clean()
	{
		//destroy buffer and free memory
		destroyUBO(m_camBuffMem, CameraDevh.device);
		//free descriptorSets
		vkFreeDescriptorSets(CameraDevh.device, CameraDescriptorPool, MAX_FRAMES_IN_FLIGHT, m_descrptrSet.data());
	}

	void updateBuffer(uint16_t imageIndex)
	{
		memcpy(m_camBuffMem.uniformBuffersMapped[imageIndex], &m_data, sizeof(m_data));
	}

	static void InitCamerasClass(DeviceHandler devh)
	{
		CameraDevh = devh;
		CameraDescriptorPool = createDescriptorPool(CameraDevh.device, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);
		CameraDescriptorSetLayout = createDescriptorSetLayout(CameraDevh.device);
	}

	static void CleanCameraClass()
	{
		vkDestroyDescriptorPool(CameraDevh.device, CameraDescriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(CameraDevh.device, CameraDescriptorSetLayout, nullptr);
	}
};
#endif