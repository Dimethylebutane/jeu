cmake_minimum_required (VERSION 3.19)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project ("ContentLoader" VERSION 0.0.1 DESCRIPTION "unify all file format to ease loading process in the engine")
message("Started CMake for ${PROJECT_NAME} v${PROJECT_VERSION}...")

add_executable(${PROJECT_NAME} "src/gltfLoader.cpp")

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED On)

###################################### DEPENDENCIES AND LIB ######################################
#import vulkan
find_package(Vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})

#dependency to LibUtils
add_dependencies(${PROJECT_NAME} libUtils)
get_target_property(utilsTargetDir libUtils INCLUDE_DIRECTORIES)
target_include_directories(${PROJECT_NAME} PUBLIC ${utilsTargetDir})


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
                      PUBLIC
                      Vulkan::Vulkan TINYGLTF libUtils TINYOBJLOADER)
