#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <vulkan/vulkan.h>

#include <array>

enum class VertexComponant
{
    Position = 1,
    Color = 2,
    TextCoord = 4
};

inline VertexComponant operator|(VertexComponant a, VertexComponant b)
{
    return static_cast<VertexComponant>(static_cast<int>(a) | static_cast<int>(b));
}

struct VertexPositionColor {
    glm::vec3 pos;
    glm::vec3 color;

    static VertexComponant Type()
    {
        return (VertexComponant::Position | VertexComponant::Color);
    }

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(VertexPositionColor);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(VertexPositionColor, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(VertexPositionColor, color);

        return attributeDescriptions;
    }

    bool operator==(const VertexPositionColor& other) const {
        return pos == other.pos && color == other.color;
    }
};

struct VertexPositionColorTextCoord {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static VertexComponant Type()
    {
        return VertexComponant::Position | VertexComponant::Color | VertexComponant::TextCoord;
    }

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(VertexPositionColorTextCoord);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(VertexPositionColorTextCoord, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(VertexPositionColorTextCoord, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(VertexPositionColorTextCoord, texCoord);

        return attributeDescriptions;
    }

    bool operator==(const VertexPositionColorTextCoord& other) const {
        return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }
};

struct VertexPositionTextCoord {
    glm::vec3 pos;
    glm::vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(VertexPositionTextCoord);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(VertexPositionTextCoord, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(VertexPositionTextCoord, texCoord);

        return attributeDescriptions;
    }

    bool operator==(const VertexPositionTextCoord& other) const {
        return pos == other.pos && texCoord == other.texCoord;
    }
};

namespace std {
    template<> struct hash<VertexPositionColorTextCoord> {
        size_t operator()(VertexPositionColorTextCoord const& vertex) const {
            return ((hash<float>()(vertex.pos.y) << 1 ^ hash<float>()(vertex.pos.x))) ^ hash<float>()(vertex.pos.z) ^
            (hash<float>()(vertex.color.y) << 1) ^ hash<float>()(vertex.color.x) ^ hash<float>()(vertex.color.z) ^
                (hash<float>()(vertex.texCoord.y) ^ hash<float>()(vertex.texCoord.x)) >> 1;
        }
    };
}