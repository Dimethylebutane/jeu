#pragma once
#include "MacroGlobal.hpp"

#include <tiny_gltf.h>

#include <fstream>

static std::vector<char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

tinygltf::Model LoadGLTFModel(const char* filename)
{
    tinygltf::TinyGLTF loader;

    tinygltf::Model model;

    std::string err;
    std::string warn;

    bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
    if (!warn.empty())
        LOG("WARN in load model " << warn);
    if (!err.empty())
        LOGERR("ERR in load model" << err);

    return model;
}