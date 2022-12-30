#include "../include/GameStruct/Model.hpp"

void Model::destroy(const VkDevice dev)
{
    vkDestroyBuffer(dev, indexBuffer, nullptr);
    vkDestroyBuffer(dev, vertexBuffer, nullptr);

    vkFreeMemory(dev, indexBufferMemory, nullptr);
    vkFreeMemory(dev, vertexBufferMemory, nullptr);
}