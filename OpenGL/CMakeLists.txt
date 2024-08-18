cmake_minimum_required(VERSION 3.10)

project(SpaceBoom)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add GLFW
add_subdirectory(vendor/glfw)

# Add GLEW
set(BUILD_UTILS OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/glew/build/cmake)

# Add GLM
add_subdirectory(vendor/glm)

# Add application code
file(GLOB cpp_files "src/*.cpp" "src/game_objects/*.cpp" "src/game_objects/enemies/*.cpp")
file(GLOB header_files "src/*.h" "src/game_objects/*.h" "src/game_objects/enemies/*.h")

# Find OpenGL
find_package(OpenGL REQUIRED)

add_executable(${PROJECT_NAME} ${cpp_files} ${header_files})

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
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/res_path.hpp.in
               ${CMAKE_CURRENT_SOURCE_DIR}/src/res_path.hpp ESCAPE_QUOTES)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE GLEW_STATIC)
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY PUBLIC_HEADER ${header_files})
