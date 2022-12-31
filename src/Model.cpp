#include "../include/GameStruct/Model.hpp"

void Model::destroy(const VkDevice dev)
{
    vkDestroyBuffer(dev, indexBuffer, nullptr);
    vkDestroyBuffer(dev, vertexBuffer, nullptr);

    vkFreeMemory(dev, indexBufferMemory, nullptr);
    vkFreeMemory(dev, vertexBufferMemory, nullptr);
}


struct RAWBUFFER
{
    uint8_t* pbuff;
    size_t sizeOfBuffer;
    size_t nElem;
};

//returned value < 0 if error
static uint64_t sizeOffAccessorComponant(tinygltf::Accessor access, uint16_t& dimension, uint16_t& size)
{
    uint16_t compType = (uint16_t)(access.componentType - 5120);

    uint64_t r = 0;

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
    size = r;

    switch (access.type)
    {
    case TINYGLTF_TYPE_SCALAR:
        break;
    case TINYGLTF_TYPE_VEC2:
        dimension = 2;
        break;
    case TINYGLTF_TYPE_VEC3:
        dimension = 3;
        break;
    case TINYGLTF_TYPE_VEC4:
        dimension = 4;
        break;
    case TINYGLTF_TYPE_MAT2:
        dimension = 4;
        break;
    case TINYGLTF_TYPE_MAT3:
        dimension = 9;
        break;
    case TINYGLTF_TYPE_MAT4:
        dimension = 16;
        break;
    default:
        dimension = -1;
        break;
    }

    return r * dimension;
}

RAWBUFFER getBufferData(const tinygltf::Accessor& access, const tinygltf::Model& model)
{
    RAWBUFFER r{};

    tinygltf::BufferView buffView = model.bufferViews[access.bufferView];
    tinygltf::Buffer buff = model.buffers[buffView.buffer];

    uint16_t dim; uint16_t si;

    uint64_t size = sizeOffAccessorComponant(access, dim, si);
    assert(size > 0);

    if (size < 0)
        return {nullptr, 0, 0};

    //total size of buffer
    r.sizeOfBuffer = size * access.count;
    r.nElem = access.count;

    //get data ptr
    r.pbuff = buff.data.data() + buffView.byteOffset + access.byteOffset; //get ptr to data

    return r;
}

static void copyBufferWithDec(uint8_t* dst, uint8_t* src, size_t nElem, size_t sizeOfElem, size_t dstOffset)
{
    for (int v = 0; v < nElem; v++)
    {
        for (int i = 0; i < sizeOfElem; i++) //r32g32b32 = 32*3 char*
            *(dst + v * sizeOfElem + i + dstOffset) = *(src + v * sizeOfElem + i);
    }
}

bool LoadModelGeometry(Model& toLoad, VkPipelineVertexInputStateCreateInfo& vertexInputInfo, VkPipelineInputAssemblyStateCreateInfo& inputAssembly,
    const tinygltf::Model& model, const tinygltf::Mesh& mesh, 
    DeviceHandler devh, VkQueue trsfrtQueue, VkCommandPool commandPool)
{
    RAWBUFFER vertexBuff{};

    RAWBUFFER indexRawBuff{};

    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = 0;
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

    //TODO: data alignement
    for (auto& prim : mesh.primitives)
    {
        //index buffer
        {
            tinygltf::Accessor access = model.accessors[prim.indices];
            indexRawBuff = getBufferData(access, model);
        }

        //mode -> input assembly
        switch (prim.mode)
        {
        case 0://pts
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
            break;
        case 1://line
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            break;
        case 2://line loop
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            break;
        case 3://line strip
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
            break;
        case 4://tri
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            break;
        case 5://tri strip
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            break;
        case 6://tri fan
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
            break;
        default:
            break;
        }

        attributeDescriptions.reserve(prim.attributes.size());
        int loc = 0;
        size_t offset = 0;

        RAWBUFFER POSRawBuff, NORMRawBuff, COLRawBuff;
        size_t posElemSize, normElemSize, colElemSize, vertexElemSize;

        if (prim.attributes.contains("POSITION"))
        {
            VkVertexInputAttributeDescription IAD{};
            IAD.binding = 0;
            IAD.location = loc;
            IAD.offset = offset;

            tinygltf::Accessor access = model.accessors[prim.attributes.at("POSITION")];
            POSRawBuff = getBufferData(access, model);

            IAD.format = VK_FORMAT_R32G32B32_SFLOAT;
            posElemSize = 3 * 32;

            offset += sizeof(glm::vec3);
            loc++;
            attributeDescriptions.push_back(IAD);
        }
        else return false; //POSITION is require
        if (prim.attributes.contains("NORMAL"))
        {
            VkVertexInputAttributeDescription IAD{};
            IAD.binding = 0;
            IAD.location = 1;
            IAD.offset = offset;

            tinygltf::Accessor access = model.accessors[prim.attributes.at("NORMAL")];
            NORMRawBuff = getBufferData(access, model);

            IAD.format = VK_FORMAT_R32G32B32_SFLOAT;
            normElemSize = 3 * 32;

            offset += sizeof(glm::vec3);
            loc++;
            attributeDescriptions.push_back(IAD);
        }
        if (prim.attributes.contains("COLOR_0"))
        {
            VkVertexInputAttributeDescription IAD{};
            IAD.binding = 0;
            IAD.location = 0;
            IAD.offset = offset;

            tinygltf::Accessor access = model.accessors[prim.attributes.at("NORMAL")];
            COLRawBuff = getBufferData(access, model);

            uint16_t dim, siz;
            uint64_t sizeTot = sizeOffAccessorComponant(access, dim, siz);
            assert(sizeTot > 0);

            if (sizeTot < 0)
                return false;

            //dim = 3 or 4
            //siz = 4(float) or 1 (norm ubyte) or 2 (norm ushort)

            //format decoding
            if (dim == 3) //vec3 rgb
            {
                switch (siz)
                {
                case 4://float
                    IAD.format = VK_FORMAT_R32G32B32_SFLOAT;
                    break;
                case 2://norm ushort
                    IAD.format = VK_FORMAT_R16G16B16_UNORM;
                    break;
                case 1://norm ubyte
                    IAD.format = VK_FORMAT_R8G8B8_UNORM;
                    break;
                default:
                    return false;
                    break;
                }
            }
            else if (dim == 4) //vec4 argb
            {
                switch (siz)
                {
                case 4://float
                    IAD.format = VK_FORMAT_R32G32B32A32_SFLOAT;
                    break;
                case 2://norm ushort
                    IAD.format = VK_FORMAT_R16G16B16A16_UNORM;
                    break;
                case 1://norm ubyte
                    IAD.format = VK_FORMAT_R8G8B8A8_UNORM;
                    break;
                default:
                    return false;
                    break;
                }

                IAD.format = VK_FORMAT_R32G32B32_SFLOAT;
            }

            colElemSize = dim * siz;

            offset += sizeTot;
            loc++;
            attributeDescriptions.push_back(IAD);

            //todo: create buffer
        }

        vertexElemSize = posElemSize + normElemSize + colElemSize;
        
        vertexBuff.nElem = POSRawBuff.nElem;
        vertexBuff.sizeOfBuffer = POSRawBuff.sizeOfBuffer + NORMRawBuff.sizeOfBuffer + COLRawBuff.sizeOfBuffer;
        vertexBuff.pbuff = reinterpret_cast<uint8_t*>(malloc(vertexBuff.sizeOfBuffer));

        copyBufferWithDec(vertexBuff.pbuff, POSRawBuff.pbuff, vertexBuff.nElem, posElemSize, 0);

        if (NORMRawBuff.nElem != 0)
        {
            assert(NORMRawBuff.nElem == vertexBuff.nElem);
            if (NORMRawBuff.nElem == vertexBuff.nElem)
                return false;

            copyBufferWithDec(vertexBuff.pbuff, NORMRawBuff.pbuff, vertexBuff.nElem, normElemSize, posElemSize);
        }
        if (COLRawBuff.nElem != 0)
        {
            assert(COLRawBuff.nElem == vertexBuff.nElem);
            if (COLRawBuff.nElem == vertexBuff.nElem)
                return false;

            copyBufferWithDec(vertexBuff.pbuff, COLRawBuff.pbuff, vertexBuff.nElem, colElemSize, posElemSize + normElemSize * (NORMRawBuff.nElem != 0));
        }
        //indexBuffer and vertexBuffer are done
        
        //VkPipelineVertexInputStateCreateInfo
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        //make model
        
        //vertex
        VkDeviceSize bufferSize = vertexBuff.sizeOfBuffer;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory, devh);

        void* data;
        vkMapMemory(devh.device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertexBuff.pbuff, (size_t)bufferSize);
        vkUnmapMemory(devh.device, stagingBufferMemory);

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, toLoad.vertexBuffer, toLoad.vertexBufferMemory, devh);

        copyBuffer(stagingBuffer, toLoad.vertexBuffer, bufferSize, devh.device, trsfrtQueue, commandPool);

        vkDestroyBuffer(devh.device, stagingBuffer, nullptr);
        vkFreeMemory(devh.device, stagingBufferMemory, nullptr);

        //Indices
        bufferSize = indexRawBuff.sizeOfBuffer;

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory, devh);

        vkMapMemory(devh.device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indexRawBuff.pbuff, (size_t)bufferSize);
        vkUnmapMemory(devh.device, stagingBufferMemory);

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            toLoad.indexBuffer, toLoad.indexBufferMemory, devh);

        copyBuffer(stagingBuffer, toLoad.indexBuffer, bufferSize, devh.device, trsfrtQueue, commandPool);

        vkDestroyBuffer(devh.device, stagingBuffer, nullptr);
        vkFreeMemory(devh.device, stagingBufferMemory, nullptr);

        return true;
    }

    return false;
}