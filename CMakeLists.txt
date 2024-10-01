cmake_minimum_required(VERSION 3.24)

project(SpaceBoom)

include(cmake/CPM.cmake)
CPMAddPackage(
  NAME GroupSourcesByFolder.cmake
  GITHUB_REPOSITORY TheLartians/GroupSourcesByFolder.cmake
  VERSION 1.0
)

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_EXTENSIONS OFF )
set( CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(NOT MSVC)
   add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:-pedantic;-Wall;-Wextra;-Wcast-qual;-Wdisabled-optimization;-Winit-self;-Wmissing-include-dirs;;-Wno-unused;-Wno-cast-qual;-Wno-unused-parameter>")
   if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
      add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:-Wlogical-op;-Wstrict-null-sentinel>")
   endif()
else()
   add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:/W4;/Zc:__cplusplus>")
endif()

add_subdirectory(OpenGL)

