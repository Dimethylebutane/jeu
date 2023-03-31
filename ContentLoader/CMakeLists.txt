cmake_minimum_required (VERSION 3.19)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project ("ContentLoader" VERSION 0.0.1 DESCRIPTION "unify all file format to ease loading process in the engine")
message("Started CMake for ${PROJECT_NAME} v${PROJECT_VERSION}...")

set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_SHARED_LIBRARY_PREFIX "")

add_executable(${PROJECT_NAME} "src/gltfLoader.cpp")

target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})

#import vulkan
find_package(Vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})

#link target to lib
target_link_libraries(${PROJECT_NAME}
                      PUBLIC
                      Vulkan::Vulkan GLFW TINYGLTF libUtils TINYOBJLOADER)