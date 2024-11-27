cmake_minimum_required(VERSION 3.30)
project(test_plot)
set(CMAKE_CXX_STANDARD 20)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(external/glfw)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()


include_directories(
        external/glad/include/
        external/glfw/include/
        external/glm/
        external/stb/
        external/imgui/)
file(GLOB imgui CONFIGURE_DEPENDS "external/imgui/*.cpp")

find_package(OpenGL REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp external/glad/src/glad.c)

target_link_libraries(${PROJECT_NAME} glfw
        ${GLFW_LIBRARIES} ${GLAD_LIBRARIES} ${imgui} OpenGL::GL)

