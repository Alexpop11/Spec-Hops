set(PROJECT_NAME OpenGL)

file(GLOB cpp_files "src/*.cpp" "src/game_objects/*.cpp")
file(GLOB header_files "src/*.h" "src/game_objects/*.h")

## OpenGL
find_package(OpenGL REQUIRED)

## GLFW
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

## GLU
find_package(GLU REQUIRED)

find_package(GLEW REQUIRED)

add_executable(${PROJECT_NAME} ${cpp_files} ${header_files})

target_include_directories(${PROJECT_NAME} PRIVATE ${GlFW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} )

target_link_libraries(${PROJECT_NAME} glfw3 ${GLEW_LIBRARIES} GLEW::GLEW ${OPENGL_LIBRARIES})

set_property(TARGET ${PROJECT_NAME} PROPERTY PUBLIC_HEADER ${header_files})
