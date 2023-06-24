// jeu.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include "vulkan/vulkan.h"
#include "glm/glm.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <set>

#include "Settings.hpp"

#include "include/EngineBuildingBlock/AppHandler.hpp"
#include "include/EngineBuildingBlock/GLFWCallback.hpp"

#include "include/GameStruct/SwapChain.hpp"
#include "include/GameStruct/Model.hpp"
#include "include/GameStruct/Camera.hpp"
#include "include/GameStruct/SkyBox.hpp"
#include "include/GameStruct/Input.hpp"

#include "include/Utils/ShaderModuleUtils.hpp"
#include "include/Utils/FileMngmtUtils.hpp"
#include "include/Utils/CommandPoolUtils.hpp"
#include "include/Utils/DescriptorSetUtils.hpp"

#include "MacroGlobal.hpp"


#ifndef NDEBUG
#define ENABLEVALIDATIONLAYERS
#endif

//include validation layer stuff if enable
#ifdef ENABLEVALIDATIONLAYERS
#include "include/EngineBuildingBlock/ValidationLayer.hpp"
#endif 

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#include <tiny_gltf.h>
