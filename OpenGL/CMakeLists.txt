set(PROJECT_NAME OpenGL)

file(GLOB cpp_files "src/*.cpp" "src/game_objects/*.cpp")
file(GLOB header_files "src/*.h" "src/game_objects/*.h")

## OpenGL
find_package(OpenGL REQUIRED)

## GLFW
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

## OpenAL
find_package(OpenAL REQUIRED)

## GLEW
find_package(GLEW REQUIRED)

add_executable(${PROJECT_NAME} ${cpp_files} ${header_files})

target_include_directories(${PROJECT_NAME} PRIVATE ${OpenAL_INCLUDE_DIRS} ${GlFW_INCLUDE_DIRS} ${GlEW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} )

target_link_libraries(${PROJECT_NAME} openal glfw3 GLEW::GLEW ${OPENGL_LIBRARIES})

set_property(TARGET ${PROJECT_NAME} PROPERTY PUBLIC_HEADER ${header_files})
