cmake_minimum_required(VERSION 3.10)

project(SpaceBoom)

set(CMAKE_CXX_STANDARD 20)

# Add application code
file(GLOB cpp_files "src/*.cpp" "src/game_objects/*.cpp" "src/game_objects/enemies/*.cpp")
file(GLOB header_files "src/*.h" "src/game_objects/*.h" "src/game_objects/enemies/*.h")

# Add resource files
file(GLOB_RECURSE res_files "res/*")

include(../cmake/CPM.cmake)
CPMAddPackage(
  NAME GroupSourcesByFolder.cmake
  GITHUB_REPOSITORY TheLartians/GroupSourcesByFolder.cmake
  VERSION 1.0
)

# Add GLFW
add_subdirectory(vendor/glfw)

# Add GLEW
set(BUILD_UTILS OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/glew/build/cmake)

# Add GLM
add_subdirectory(vendor/glm)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Add xxHash
add_subdirectory(vendor/xxHash/cmake_unofficial vendor/xxHash/build/ EXCLUDE_FROM_ALL)

add_executable(${PROJECT_NAME} ${cpp_files} ${header_files} ${res_files})

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
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glew/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm
)


target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    glew_s
    OpenGL::GL
    glm::glm
    xxHash::xxhash
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/res_path.hpp.in
               ${CMAKE_CURRENT_SOURCE_DIR}/src/res_path.hpp ESCAPE_QUOTES)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE GLEW_STATIC)
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY PUBLIC_HEADER ${header_files})
