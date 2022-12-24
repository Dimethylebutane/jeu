#include "../include/Model.hpp"

Model createModel(std::vector<Vertex> vertices, std::vector<uint16_t> indices,
    const DeviceHandler devh, const VkQueue trsfrtQueue, const VkCommandPool commandPool)
{
    Model r{ };

    //vertex
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer, stagingBufferMemory, devh);

    void* data;
    vkMapMemory(devh.device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(devh.device, stagingBufferMemory);

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, r.vertexBuffer, r.vertexBufferMemory, devh);

    copyBuffer(stagingBuffer, r.vertexBuffer, bufferSize, devh.device, trsfrtQueue, commandPool);

    vkDestroyBuffer(devh.device, stagingBuffer, nullptr);
    vkFreeMemory(devh.device, stagingBufferMemory, nullptr);

    //Indices

    bufferSize = sizeof(indices[0]) * indices.size();

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer, stagingBufferMemory, devh);

    vkMapMemory(devh.device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(devh.device, stagingBufferMemory);

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        r.indexBuffer, r.indexBufferMemory, devh);

    copyBuffer(stagingBuffer, r.indexBuffer, bufferSize, devh.device, trsfrtQueue, commandPool);

    vkDestroyBuffer(devh.device, stagingBuffer, nullptr);
    vkFreeMemory(devh.device, stagingBufferMemory, nullptr);

    return r;
}

void destroyModel(Model model, const VkDevice dev)
{
    vkDestroyBuffer(dev, model.indexBuffer, nullptr);
    vkDestroyBuffer(dev, model.vertexBuffer, nullptr);

    vkFreeMemory(dev, model.indexBufferMemory, nullptr);
    vkFreeMemory(dev, model.vertexBufferMemory, nullptr);
}