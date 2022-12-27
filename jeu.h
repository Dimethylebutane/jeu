// jeu.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include "vulkan/vulkan.h"
#include "glm/glm.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <tiny_gltf.h>

#include <stdexcept>
#include <set>

#include "include/SwapChain.hpp"
#include "include/AppHandler.hpp"
#include "include/ShaderModuleUtils.hpp"
#include "include/FileMngmtUtils.hpp"
#include "Settings.hpp"
#include "include/Model.hpp"
#include "include/CommandPool.hpp"
#include "include/DescriptorSet.hpp"
#include "include/Camera.hpp"
#include "include/SkyBox.hpp"

#include "libUtils/include/MacroGlobal.hpp"


#ifndef NDEBUG
#define ENABLEVALIDATIONLAYERS
#endif

//include validation layer stuff if enable
#ifdef ENABLEVALIDATIONLAYERS
#include "include/ValidationLayer.hpp"
#endif 