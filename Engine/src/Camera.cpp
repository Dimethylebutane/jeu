#include "../include/GameStruct/Camera.hpp"

#include <cstring>

void Camera::init(const unsigned char numberOfFrame)
{
	assert(numberOfFrame != 0);

	m_camBuffMem = createUBO<CamUBO>(numberOfFrame, CameraDevh);
	m_descrptrSet = createDescriptorSets<CamUBO>(numberOfFrame, CameraDescriptorSetLayout, CameraDescriptorPool, CameraDevh.device, m_camBuffMem);
}

void Camera::clean()
{
	//destroy buffer and free memory
	destroyUBO(m_camBuffMem, CameraDevh.device);
	//free descriptorSets
	vkFreeDescriptorSets(CameraDevh.device, CameraDescriptorPool, MAX_FRAMES_IN_FLIGHT, m_descrptrSet.data());
}

void Camera::updateBuffer(uint16_t imageIndex)
{
	m_data.view = glm::lookAt(m_data.pos, m_data.front + m_data.pos, m_data.up);

	memcpy(m_camBuffMem.uniformBuffersMapped()[imageIndex], &m_data, sizeof(m_data));
}

void Camera::updateCamPosition(glm::vec3 newPos)
{
	m_data.pos = newPos;
}

void Camera::moveCameraOf(glm::vec3 deplacement)
{
	m_data.pos += deplacement;
}

void Camera::lookAt(glm::vec3 Center)
{
	glm::vec3 old = m_data.front;

	m_data.front = Center - m_data.pos;

	m_data.up = glm::cross(m_data.front, m_data.up) == glm::vec3(0, 0, 0) ? -old : m_data.up;
}

void Camera::rotateCamera(glm::vec3 axis, float angle)
{
	m_data.front = glm::rotate(glm::mat4(1), angle, axis) * glm::vec4(m_data.front, 1);
	m_data.up = glm::rotate(glm::mat4(1), angle, axis) * glm::vec4(m_data.up, 1);
}

VkDescriptorPool Camera::CameraDescriptorPool{};
DeviceHandler Camera::CameraDevh{};
VkDescriptorSetLayout Camera::CameraDescriptorSetLayout{};

void Camera::InitCamerasClass(uint32_t NumberOfImageToBind, DeviceHandler devh)
{
	CameraDevh = devh;
	CameraDescriptorPool = createDescriptorPool(NumberOfImageToBind, CameraDevh.device, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);
	CameraDescriptorSetLayout = createDescriptorSetLayout(CameraDevh.device);
}

void Camera::CleanCameraClass()
{
	vkDestroyDescriptorPool(CameraDevh.device, CameraDescriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(CameraDevh.device, CameraDescriptorSetLayout, nullptr);
}
