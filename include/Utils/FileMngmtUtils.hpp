#pragma once
#include "MacroGlobal.hpp"

#include <tiny_gltf.h>

#include <fstream>

std::vector<char> readFile(const std::string& filename);

tinygltf::Model LoadGLTFModel(const char* filename);