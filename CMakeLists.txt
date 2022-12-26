# CMakeList.txt : CMake project for jeu, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.19)

SET(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER clang)

project ("JEU" VERSION 0.0.1 DESCRIPTION "ksdjhksdjh")
message("Started CMake for ${PROJECT_NAME} v${PROJECT_VERSION}...")

add_subdirectory ("libUtils")

    #define stuff
#set where to look for lib
set(CMAKE_LIBRARY_PATH "C:/lib")
#resources path
set(RESOURCESPATH "${PROJECT_BINARY_DIR}/Resources")
#macro for program
add_compile_definitions(RESOURCES_PATH="${RESOURCESPATH}")

# Add source to this project's executable.
add_executable (${PROJECT_NAME} "jeu.cpp" "jeu.h" "src/SwapChain.cpp" "src/Model.cpp" "src/BuffersUtils.cpp" "src/QueueUtils.cpp" "src/Camera.cpp" "src/DescriptorSet.cpp" "src/SkyBox.cpp" "src/ShaderModulesUtils.cpp")

#import vulkan
find_package(Vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})

#add glfw
add_library(GLFW SHARED IMPORTED) # or STATIC instead of SHARED

set_target_properties(GLFW PROPERTIES
  #IMPORTED_LOCATION "${CMAKE_LIBRARY_PATH}/GLFW/glfw-3.3.5.bin.WIN64/glfw-3.3.5.bin.WIN64/lib-vc2022/glfw3.lib"
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_LIBRARY_PATH}/GLFW/include"
  IMPORTED_IMPLIB "${CMAKE_LIBRARY_PATH}/GLFW/lib-vc2022/glfw3.lib"
	#win only tag??
)

#add tinyGLTF
add_library(TINYGLTF INTERFACE)
set_target_properties(TINYGLTF PROPERTIES
  #IMPORTED_LOCATION "${CMAKE_LIBRARY_PATH}/GLFW/glfw-3.3.5.bin.WIN64/glfw-3.3.5.bin.WIN64/lib-vc2022/glfw3.lib"
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_LIBRARY_PATH}/TinyGLTF/include"
)

#link target to lib
target_link_libraries(${PROJECT_NAME}
                      PUBLIC
                      Vulkan::Vulkan GLFW TINYGLTF libUtils)

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
