cmake_minimum_required(VERSION 3.10)

project(Spec-Hops)

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

if (NOT EMSCRIPTEN)
    add_definitions(-DIMGUI_IMPL_WEBGPU_BACKEND_DAWN)
else()
	add_library(glfw INTERFACE)
	target_link_options(glfw INTERFACE -sUSE_GLFW=3)
endif()

# Add resource files
file(GLOB_RECURSE res_files "res/*")

# Add WebGPU
# Also adds Add GLFW
FetchContent_Declare(
  webgpu
  GIT_REPOSITORY https://github.com/anchpop/WebGPU-distribution.git
  GIT_TAG        257f230bdad880339eef961b7ff5974a04e30dc5
)
FetchContent_MakeAvailable(webgpu)

add_subdirectory(${VENDOR_DIR}/glfw3webgpu)

# Add GLM
add_subdirectory(${VENDOR_DIR}/glm)

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
    glm::glm
    Clipper2
    webgpu
    glfw3webgpu
)

target_copy_webgpu_binaries(${PROJECT_NAME})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/res_path.hpp.in
               ${CMAKE_CURRENT_SOURCE_DIR}/src/res_path.hpp ESCAPE_QUOTES)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE GLEW_STATIC)
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY PUBLIC_HEADER ${header_files})

if (EMSCRIPTEN)
	set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
	target_link_options(${PROJECT_NAME} PRIVATE
		-sASYNCIFY
		--preload-file "${CMAKE_CURRENT_SOURCE_DIR}/res"
        --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/../web/template.html
	)
    # TODO: It would be cool to add  -fno-exceptions -fno-rtti
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -flto")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O3 -flto")


    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/../web/static
        ${CMAKE_CURRENT_BINARY_DIR}/static
    )
endif()
