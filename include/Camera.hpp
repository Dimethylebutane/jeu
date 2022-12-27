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

	void init(const unsigned char numberOfFrame)
	{
		assert(numberOfFrame != 0);

		m_camBuffMem = createUBO<CamUBO>(numberOfFrame, CameraDevh);
		m_descrptrSet = createDescriptorSets<CamUBO>(numberOfFrame, CameraDescriptorSetLayout, CameraDescriptorPool, CameraDevh.device, m_camBuffMem);
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
		m_data.view = glm::lookAt(m_data.pos, m_data.front + m_data.pos, m_data.up);
		
		memcpy(m_camBuffMem.uniformBuffersMapped()[imageIndex], &m_data, sizeof(m_data));
	}

	void updateCamPosition(glm::vec3 newPos)
	{
		m_data.pos = newPos;
	}

	void moveCameraOf(glm::vec3 deplacement)
	{
		m_data.pos += deplacement;
	}

	void lookAt(glm::vec3 Center)
	{
		glm::vec3 old = m_data.front;

		m_data.front = Center - m_data.pos;

		m_data.up = glm::cross(m_data.front, m_data.up) == glm::vec3(0, 0, 0) ? -old : m_data.up;
	}

	void rotateCamera(glm::vec3 axis, float angle)
	{
		m_data.front = glm::rotate(glm::mat4(1), angle, axis) * glm::vec4(m_data.front, 1);
		m_data.up = glm::rotate(glm::mat4(1), angle, axis) * glm::vec4(m_data.up, 1);
	}

	//static stuff
	static VkDescriptorPool CameraDescriptorPool;
	static DeviceHandler CameraDevh;
	static VkDescriptorSetLayout CameraDescriptorSetLayout;

	static void InitCamerasClass(uint32_t NumberOfImageToBind, DeviceHandler devh)
	{
		CameraDevh = devh;
		CameraDescriptorPool = createDescriptorPool(NumberOfImageToBind, CameraDevh.device, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);
		CameraDescriptorSetLayout = createDescriptorSetLayout(CameraDevh.device);
	}

	static void CleanCameraClass()
	{
		vkDestroyDescriptorPool(CameraDevh.device, CameraDescriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(CameraDevh.device, CameraDescriptorSetLayout, nullptr);
	}
};
#endif