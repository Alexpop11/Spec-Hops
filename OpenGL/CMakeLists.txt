cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME OpenGL)

project(${PROJECT_NAME})

file(GLOB_RECURSE cpp_files "src/*.cpp")
file(GLOB_RECURSE header_files "src/*.h")

## OpenGL
find_package(OpenGL REQUIRED)

## GLFW
if(WIN32)
  set(GLFW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/OpenGL/Dependencies/GLFW/include")
  set(GLFW_LIBRARY "${CMAKE_SOURCE_DIR}/OpenGL/Dependencies/GLFW/lib-vc2022/glfw3_mt.lib")
else()
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(GLFW REQUIRED glfw3)
endif()

## GLEW
if(WIN32)
  set(GLEW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/OpenGL/Dependencies/GLEW/include")
  set(GLEW_LIBRARY "${CMAKE_SOURCE_DIR}/OpenGL/Dependencies/GLEW/lib/Release/x64/glew32s.lib")
  
  if(NOT EXISTS ${GLEW_LIBRARY})
    message(FATAL_ERROR "GLEW library not found at ${GLEW_LIBRARY}. Please check the path.")
  endif()
else()
  find_package(GLEW REQUIRED)
endif()

add_executable(${PROJECT_NAME} ${cpp_files} ${header_files})

target_include_directories(${PROJECT_NAME} PRIVATE 
  ${GLFW_INCLUDE_DIR} 
  ${GLEW_INCLUDE_DIR} 
  ${OPENGL_INCLUDE_DIRS} 
)

if(WIN32)
  target_link_libraries(${PROJECT_NAME} 
    ${GLFW_LIBRARY}
    ${GLEW_LIBRARY}
    ${OPENGL_LIBRARIES} 
  )
  target_compile_definitions(${PROJECT_NAME} PRIVATE GLEW_STATIC)
else()
  target_link_libraries(${PROJECT_NAME} 
    -L${GLFW_LIBDIR} ${GLFW_LIBRARIES} 
    GLEW::GLEW 
    ${OPENGL_LIBRARIES} 
  )
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY PUBLIC_HEADER ${header_files})
