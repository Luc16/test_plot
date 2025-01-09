cmake_minimum_required(VERSION 3.20)
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
        external/imgui/
        external/implot/
        lib/
)
file(GLOB imgui CONFIGURE_DEPENDS "external/imgui/*.cpp")
file(GLOB implot CONFIGURE_DEPENDS "external/implot/*.cpp")

add_library(glad_lib external/glad/src/glad.c)
target_include_directories(glad_lib PRIVATE external/glad/include)

find_package(OpenGL REQUIRED)

add_executable(example_window src/example.cpp)

target_link_libraries(example_window glfw
        ${GLFW_LIBRARIES} ${GLAD_LIBRARIES} ${imgui} glad_lib OpenGL::GL)


add_executable(initialTests src/initialTests.cpp)

target_link_libraries(initialTests glfw
        ${GLFW_LIBRARIES} ${GLAD_LIBRARIES} ${imgui} ${implot} glad_lib OpenGL::GL)

add_executable(main src/main.cpp lib/plot.cpp lib/plot.h lib/screen_saver.cpp lib/screen_saver.h)

target_link_libraries(main glfw
        ${GLFW_LIBRARIES} ${GLAD_LIBRARIES} ${imgui} ${implot} glad_lib OpenGL::GL)
