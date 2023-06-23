#include "../include/gltfLoader.hpp"

struct RAWBUFFER
{
    uint8_t* pbuff;
    size_t sizeOfBuffer;
    size_t nElem;
};

//return size of one element of the buffer (size * dimension) value < 0 if error
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
        dimension = 1;
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
        return { nullptr, 0, 0 };

    //total size of buffer
    r.sizeOfBuffer = size * access.count; //total size of buffer
    r.nElem = access.count;               //number off element in buffer (how many vec3 or mat4 in the buffer)

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

bool LoadModelMeshGeometry(const tinygltf::Model& model, const tinygltf::Mesh& mesh, VkPipelineVertexInputStateCreateInfo& vertexInputInfo, VkPipelineInputAssemblyStateCreateInfo& inputAssembly)
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
    for (auto& primitive : mesh.primitives)
    {
        //index buffer
        {
            tinygltf::Accessor access = model.accessors[primitive.indices];

            assert(access.componentType == TINYGLTF_TYPE_SCALAR && "ERROR mesh indices type is not scalar");

            indexRawBuff = getBufferData(access, model);
        }

        //mode -> input assembly = primitive topology
        switch (primitive.mode)
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

        //TODO: material (texture and stuff)

        attributeDescriptions.reserve(primitive.attributes.size());
        int loc = 0;
        size_t offset = 0;

        RAWBUFFER POSRawBuff, NORMRawBuff, COLRawBuff;
        size_t posElemSize, normElemSize, colElemSize, vertexElemSize;

        assert(primitive.attributes.contains("POSITION") && "ERROR primitive does not contains position");
        {
            VkVertexInputAttributeDescription IAD{};
            IAD.binding = 0;
            IAD.location = loc;
            IAD.offset = offset;

            tinygltf::Accessor access = model.accessors[primitive.attributes.at("POSITION")];
            POSRawBuff = getBufferData(access, model);

            IAD.format = VK_FORMAT_R32G32B32_SFLOAT;
            posElemSize = 3 * 32;

            offset += sizeof(glm::vec3);
            loc++;
            attributeDescriptions.push_back(IAD);
        }

        if (primitive.attributes.contains("NORMAL"))
        {
            VkVertexInputAttributeDescription IAD{};
            IAD.binding = 0;
            IAD.location = 1;
            IAD.offset = offset;

            tinygltf::Accessor access = model.accessors[primitive.attributes.at("NORMAL")];
            NORMRawBuff = getBufferData(access, model);

            IAD.format = VK_FORMAT_R32G32B32_SFLOAT;
            normElemSize = 3 * 32;

            offset += sizeof(glm::vec3);
            loc++;
            attributeDescriptions.push_back(IAD);
        }
        if (primitive.attributes.contains("COLOR_0"))
        {
            VkVertexInputAttributeDescription IAD{};
            IAD.binding = 0;
            IAD.location = 0;
            IAD.offset = offset;

            tinygltf::Accessor access = model.accessors[primitive.attributes.at("NORMAL")];
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

        return true;
    }

    return false;
}

bool loadModelFromObj(const std::string& MODEL_PATH)
{
    //create objformat loader
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    //check for error
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
        throw std::runtime_error(warn + err);
    }

    //for each shape
    /*
    for (const auto& shape : shapes) {
        //loop in all indices
        for (const auto& index : shape.mesh.indices) {
            //VertexPositionColorTextCoord vertex{};

            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.texCoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            };

            vertex.color = { 1.0f, 1.0f, 1.0f };

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }

            indices.push_back(uniqueVertices[vertex]);
        }
    }*/

    return true;
}
