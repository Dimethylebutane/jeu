#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <tiny_gltf.h>

#include <vulkan/vulkan.h>

#include <vector>

#include "Vertex.hpp"
#include "../EngineBuildingBlock/UBO.hpp"
#include "../EngineBuildingBlock/AppHandler.hpp"
#include "../Utils/BufferUtils.hpp"

#include "MacroGlobal.hpp"

struct Model
{
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    template<class Vert>
    void create(std::vector<Vert> vertices, std::vector<uint16_t> indices,
        const DeviceHandler devh, const VkQueue trsfrtQueue, const VkCommandPool commandPool)
    {
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
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory, devh);

        copyBuffer(stagingBuffer, vertexBuffer, bufferSize, devh.device, trsfrtQueue, commandPool);

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
            indexBuffer, indexBufferMemory, devh);

        copyBuffer(stagingBuffer, indexBuffer, bufferSize, devh.device, trsfrtQueue, commandPool);

        vkDestroyBuffer(devh.device, stagingBuffer, nullptr);
        vkFreeMemory(devh.device, stagingBufferMemory, nullptr);

    }

    void destroy(const VkDevice dev);
};

//returned value < 0 if error
static INT64 sizeOffAccessorComponant(tinygltf::Accessor access)
{
    uint16_t compType = (uint16_t)(access.componentType - 5120);

    INT64 r = 0;

    switch (compType)
    {
    case 0://byte
        r = 1;
        break;
    case 1://ubyte
        r = 1;
        break;
    case 2://short
        r = 2;
        break;
    case 3://ushort
        r = 2;
        break;
    case 4://int
        r = 4;
        break;
    case 5://uint
        r = 4;
        break;
    case 6://float
        r = 4;
        break;
    case 10://double
        r = 8;
        break;
    default:
        r = -1;
        break;
    }

    switch (access.type)
    {
    case TINYGLTF_TYPE_SCALAR:
        break;
    case TINYGLTF_TYPE_VEC2:
        r *= 2;
        break;
    case TINYGLTF_TYPE_VEC3:
        r *= 3;
        break;
    case TINYGLTF_TYPE_VEC4:
        r *= 4;
        break; 
    case TINYGLTF_TYPE_MAT2:
        r *= 4;
        break;
    case TINYGLTF_TYPE_MAT3:
        r *= 9;
        break;
    case TINYGLTF_TYPE_MAT4:
        r *= 16;
        break;
    default:
        r = -1;
        break;
    }

    return r;
}

//todo: VkVertexInputBindingDescription
//todo: VkVertexInputAttributeDescription
bool LoadModelGeometry(Model& toLoad, tinygltf::Model model, const char* meshName)
{
    tinygltf::Mesh& mesh = model.meshes[0];
    bool find = false;
    for (auto& m : model.meshes)
    {
        if (m.name.compare(meshName) == 0)
        {
            mesh = m;
            find = true;
            break;
        }
    }

    if (!find)   //mesh not exist
        return false;

    std::vector<VertexPositionColor> vert;

    uint8_t* indices;
    size_t sizeOfIndicesList;
    size_t nIndex;
    
    for (auto& prim : mesh.primitives)
    {
        {
            //primitive stuff:
            tinygltf::Accessor access = model.accessors[prim.indices];

            tinygltf::BufferView buffView = model.bufferViews[access.bufferView];
            tinygltf::Buffer buff = model.buffers[buffView.buffer];

            INT64 size = sizeOffAccessorComponant(access);
            assert(size > 0);

            if (size < 0 || size > 8)
            {//error or not scalar
                return false;
            }

            //number of indices
            nIndex = access.count;

            //total size of buffer
            sizeOfIndicesList = size * nIndex;

            //get data ptr
            indices = buff.data.data() + buffView.byteOffset + access.byteOffset; //get ptr to data
        }

        //mode
        switch (prim.mode)
        {
            case 0://pts
            case 1://line
            case 2://line loop
            case 3://line strip
                break;
            case 4://tri
            case 5://tri strip
            case 6://tri fan

                break;
        default:
            break;
        }
    }
}