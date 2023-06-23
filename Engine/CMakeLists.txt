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

#dependency to LibUtils
add_dependencies(${PROJECT_NAME} libUtils)
get_target_property(utilsTargetDir libUtils INCLUDE_DIRECTORIES)
target_include_directories(${PROJECT_NAME} PUBLIC ${utilsTargetDir})

#import vulkan
find_package(Vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})

#add glfw
add_library(GLFW SHARED IMPORTED) # or STATIC instead of SHARED

set_target_properties(GLFW PROPERTIES
  #IMPORTED_LOCATION "${CMAKE_LIBRARY_PATH}/GLFW/glfw-3.3.5.bin.WIN64/glfw-3.3.5.bin.WIN64/lib-vc2022/glfw3.lib"
  IMPORTED_LOCATION "${CMAKE_INCLUDE_PATH}/GLFW"
  IMPORTED_IMPLIB "${CMAKE_LIBRARY_PATH}/libglfw.so"
	#win only tag??
)

#add tinyOBJLoader
add_library(TINYOBJLOADER INTERFACE)
set_target_properties(TINYOBJLOADER PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_INCLUDE_PATH}/tinyobjloader"
)

#add tinyGLTF
add_library(TINYGLTF INTERFACE)
set_target_properties(TINYGLTF PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_INCLUDE_PATH}/tinygltf"
)

#link target to lib
target_link_libraries(${PROJECT_NAME}
                      PUBLIC
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

#compile shader
add_custom_target(build-time-make-directory ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${RESOURCESPATH}/shaders)
add_custom_command( TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND compile.bat ${RESOURCESPATH}/shaders ${Vulkan_INCLUDE_DIR}/../
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/shaders
    COMMENT "compiling shader..."
)

# TODO: Add tests and install targets if needed.
