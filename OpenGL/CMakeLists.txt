cmake_minimum_required(VERSION 3.10)

project(SpaceBoom)

set(CMAKE_CXX_STANDARD 23)

set(VENDOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor)

# Add application code
file(GLOB_RECURSE cpp_files 
    "src/*.cpp"
    "src/*.cxx"
    "src/*.cc"
    "${VENDOR_DIR}/imgui/*.cpp"
)

# Add header files
file(GLOB_RECURSE header_files 
    "src/*.h"
    "src/*.hpp"
    "src/*.hxx"
)

add_definitions(-DIMGUI_IMPL_WEBGPU_BACKEND_DAWN)

# Add resource files
file(GLOB_RECURSE res_files "res/*")

include(../cmake/CPM.cmake)
CPMAddPackage(
  NAME GroupSourcesByFolder.cmake
  GITHUB_REPOSITORY TheLartians/GroupSourcesByFolder.cmake
  VERSION 1.0
)

# Add WebGPU
# Also adds Add GLFW
FetchContent_Declare(
  webgpu
  GIT_REPOSITORY https://github.com/anchpop/WebGPU-distribution.git
  GIT_TAG        7d29923aff711ae179ffdf933cc9ca31d6e400f0
)
FetchContent_MakeAvailable(webgpu)
if (WIN32)
    # Ensure webgpu_SOURCE_DIR is available
    FetchContent_GetProperties(webgpu)
    if(NOT webgpu_SOURCE_DIR)
        message(FATAL_ERROR "webgpu source directory not found")
    endif()

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${webgpu_SOURCE_DIR}/dist/webgpu_dawn.dll"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()

add_subdirectory(${VENDOR_DIR}/glfw3webgpu)

# Add GLM
add_subdirectory(${VENDOR_DIR}/glm)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Add xxHash
add_subdirectory(${VENDOR_DIR}/xxHash/cmake_unofficial ${VENDOR_DIR}/xxHash/build/ EXCLUDE_FROM_ALL)

add_executable(${PROJECT_NAME} ${cpp_files} ${header_files} ${res_files})

# Add Clipper2
set(CLIPPER2_TESTS OFF CACHE BOOL "Disable Clipper2 tests" FORCE)
set(CLIPPER2_UTILS OFF CACHE BOOL "Disable Clipper2 utilities" FORCE)
set(CLIPPER2_EXAMPLES OFF CACHE BOOL "Disable Clipper2 examples" FORCE)
add_subdirectory(${VENDOR_DIR}/Clipper2/CPP)

# Set resource files as header-only (non-compiled)
foreach(res_file ${res_files})
    set_source_files_properties(${res_file} PROPERTIES HEADER_FILE_ONLY TRUE)
endforeach()

# Group resource files in Visual Studio
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${res_files})

# Group source files by folder
GroupSourcesByFolder(${PROJECT_NAME})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${VENDOR_DIR}/glm
    ${VENDOR_DIR}/miniaudio
    ${VENDOR_DIR}/stb_image
    ${VENDOR_DIR}/earcut
    ${VENDOR_DIR}/Clipper2/CPP/Clipper2Lib/include
    ${VENDOR_DIR}/imgui/
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    glm::glm
    xxHash::xxhash
    Clipper2
    webgpu
    glfw3webgpu
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/res_path.hpp.in
               ${CMAKE_CURRENT_SOURCE_DIR}/src/res_path.hpp ESCAPE_QUOTES)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE GLEW_STATIC)
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY PUBLIC_HEADER ${header_files})

if(WIN32)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:webgpu>
      $<TARGET_FILE_DIR:${PROJECT_NAME}>
  )
endif()
