# CMakeList.txt : CMake project for jeu, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.19)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
SET(CMAKE_VERBOSE_MAKEFILE ON)

project ("Engine" VERSION 0.0.1 DESCRIPTION "the best game engine i could make")
message("Started CMake for ${PROJECT_NAME} v${PROJECT_VERSION}...")

# Add source to this project's executable.
add_executable (${PROJECT_NAME} "jeu.cpp" "jeu.h" "src/SwapChain.cpp" "src/Model.cpp" "src/BuffersUtils.cpp" "src/QueueUtils.cpp" "src/Camera.cpp" "src/DescriptorSet.cpp" "src/SkyBox.cpp" "src/ShaderModulesUtils.cpp" "src/Input.cpp" "src/FileManagement.cpp")

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED On)

###################################### DEPENDENCIES AND LIB ######################################
#dependency to LibUtils
add_dependencies(${PROJECT_NAME} libUtils)
get_target_property(utilsTargetDir libUtils INCLUDE_DIRECTORIES)
target_include_directories(${PROJECT_NAME} PUBLIC ${utilsTargetDir})

#import vulkan
find_package(Vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})

#add glfw
if(NOT TARGET GLFW)
	add_library(GLFW SHARED IMPORTED) # or STATIC instead of SHARED
endif()
set_property(TARGET GLFW PROPERTY
			IMPORTED_LOCATION "${CMAKE_LIBRARY_PATH}/libglfw.so")

#add tinyOBJLoader
if(NOT TARGET TINYOBJLOADER)
	add_library(TINYOBJLOADER INTERFACE)
endif()
set_target_properties(TINYOBJLOADER PROPERTIES
INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_INCLUDE_PATH}/tinyobjloader"
)

#add tinyGLTF
if(NOT TARGET TINYGLTF)
	add_library(TINYGLTF INTERFACE)
endif()
set_target_properties(TINYGLTF PROPERTIES
INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_INCLUDE_PATH}/tinygltf"
)

#link target to lib
target_link_libraries(${PROJECT_NAME}
                      PRIVATE
                      Vulkan::Vulkan GLFW TINYGLTF libUtils TINYOBJLOADER)

#c++ flags (sse3, c++20, ...)
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
    target_compile_options(${PROJECT_NAME} INTERFACE /arch:SSE3)
else()
    target_compile_options(${PROJECT_NAME} INTERFACE -msse3)
endif()

#compile shader every time
add_custom_target(TDocument ALL
  DEPENDS ${RESOURCESPATH}/trick
)
add_custom_command(
    COMMAND ./compile.sh ${RESOURCESPATH}/shaders
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Resources/shaders
    COMMENT "compiling shader..."
    OUTPUT ${RESOURCESPATH} ${RESOURCESPATH}/trick
    DEPENDS always_rebuild
)
add_custom_command(
    OUTPUT always_rebuild
    COMMAND cmake -E echo
)

# TODO: Add tests and install targets if needed.
