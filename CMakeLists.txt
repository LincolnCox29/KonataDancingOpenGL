cmake_minimum_required(VERSION 3.10)
project(KonataDancingOpenGL VERSION 1.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")

set(CMAKE_BUILD_TYPE Release)

set(FRAMES_DIRECTORY "${CMAKE_SOURCE_DIR}/frames")
set(OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Debug/frames")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/glew-2.1.0/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw-3.4.bin.WIN64/include)

add_executable(KonataDancingOpenGL KonataDancingOpenGL.c)

target_link_libraries(KonataDancingOpenGL
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/glew-2.1.0/lib/Release/x64/glew32s.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw-3.4.bin.WIN64/lib-vc2022/glfw3.lib
    opengl32.lib
)

add_custom_command(TARGET KonataDancingOpenGL POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${FRAMES_DIRECTORY} ${OUTPUT_DIRECTORY}
    COMMENT "Copying frames directory to Release output folder"
)

target_compile_definitions(KonataDancingOpenGL PRIVATE GLEW_STATIC)
