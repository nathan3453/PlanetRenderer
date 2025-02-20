cmake_minimum_required(VERSION 3.16)

# Set the project name and version
project(PlanetRenderer VERSION 1.0)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Specify the path to GLM
set(GLM_DIR "${CMAKE_SOURCE_DIR}/dependencies/glm")  # Path to your cloned GLM directory
include_directories(${GLM_DIR})

# Specify the path to GLEW
set(GLEW_DIR "${CMAKE_SOURCE_DIR}/dependencies/glew")  # Path to GLEW
include_directories("${GLEW_DIR}/include")

add_definitions(-DGLEW_STATIC)

file(GLOB_RECURSE GLEW_SOURCE "${GLEW_DIR}/src/*.c" "${GLEW_DIR}/include/*.h")
add_library(glew ${GLEW_SOURCE})

# GLFW
add_subdirectory(dependencies/glfw)
include_directories("${CMAKE_SOURCE_DIR}/dependencies/glfw/include/")

# ImGui
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/dependencies/imgui")
# Specify the backend directory
set(IMGUI_BACKEND_DIR "${IMGUI_DIR}/backends")
include_directories(${IMGUI_DIR} ${IMGUI_BACKEND_DIR})


# Collect ImGui source files
file(GLOB IMGUI_SOURCE "${IMGUI_DIR}/*.cpp" "${IMGUI_DIR}/*.h")

# Collect backend include files (if needed, typically not necessary for linking)
file(GLOB IMGUI_BACKENDS_INCLUDE
    "${IMGUI_BACKEND_DIR}/imgui_impl_glfw.h"
    "${IMGUI_BACKEND_DIR}/imgui_impl_opengl3.h"
)

# Collect backend source files
file(GLOB IMGUI_BACKENDS_SOURCE
    "${IMGUI_BACKEND_DIR}/imgui_impl_glfw.cpp"
    "${IMGUI_BACKEND_DIR}/imgui_impl_opengl3.cpp"
)

# Create the ImGui library, including both ImGui and backend source files
add_library(imgui ${IMGUI_SOURCE} ${IMGUI_BACKENDS_SOURCE})

# ImGui File Dialog
set(IMGUI_FILE_DIALOG_DIR "${CMAKE_SOURCE_DIR}/dependencies/imgui-file-dialog")
include_directories(${IMGUI_FILE_DIALOG_DIR})
file(GLOB IMGUI_FILE_DIALOG_SOURCE "${IMGUI_FILE_DIALOG_DIR}/*.cpp" "${IMGUI_FILE_DIALOG_DIR}/*.h")

add_library(imguiFileDialog ${IMGUI_FILE_DIALOG_SOURCE})

# Stb Image
set(STB_DIR "${CMAKE_SOURCE_DIR}/dependencies/stb")
include_directories(${STB_DIR})

# Assimp
add_subdirectory(dependencies/assimp)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")

set(ICON_RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon.ico")

set(RESOURCE_FILE "${CMAKE_CURRENT_BINARY_DIR}/resources/resources.rc")

# Write the resource script for the icon
file(WRITE ${RESOURCE_FILE} 
    "IDI_ICON1 ICON \"${ICON_RESOURCE}\""
)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

if(WIN32)
    set_target_properties(PlanetRenderer PROPERTIES WIN32_EXECUTABLE TRUE)
elseif(APPLE)
    set_target_properties(PlanetRenderer PROPERTIES MACOSX_BUNDLE TRUE)
endif()

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    ${OPENGL_LIBRARIES} 
    glew
    glfw
    imgui
    imguiFileDialog
    assimp
)

# Gather shaders for copying
set(SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
file(GLOB SHADERS "${SHADER_DIR}/*.vert" "${SHADER_DIR}/*.frag" "${SHADER_DIR}/*.tesc" "${SHADER_DIR}/*.tese" "${SHADER_DIR}/*.comp")

# Custom command to copy shaders to the build output
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/shaders
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SHADERS}
        ${CMAKE_CURRENT_BINARY_DIR}/shaders/
    DEPENDS ${SHADERS}
    COMMENT "Copying shaders to build output"
)

# Ensure shaders are copied
add_custom_target(copy_shaders ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/shaders)
add_dependencies(PlanetRenderer copy_shaders)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resources" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")