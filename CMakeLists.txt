
cmake_minimum_required(VERSION 3.0)

project(Xerus)

# Set dependency options
macro(SET_OPTION option value)
  set(${option} ${value} CACHE INTERNAL "" FORCE)
endmacro()

# GLEW options
SET_OPTION(glew-cmake_BUILD_SHARED OFF)
SET_OPTION(glew-cmake_BUILD_MULTI_CONTEXT OFF)

# GLFW options
SET_OPTION(GLFW_BUILD_DOCS OFF)
SET_OPTION(GLFW_BUILD_EXAMPLES OFF)
SET_OPTION(GLFW_BUILD_TESTS OFF)
SET_OPTION(GLFW_INSTALL OFF)

# Add dependencies
add_subdirectory("deps/glew/build/cmake")
add_subdirectory("deps/glfw")
add_subdirectory("deps/picopng")

add_subdirectory("deps/glt")

# Add include directory for GLEW
include_directories("deps/glew/include")


# Options for Xerus
option(XERUS_BUILD_EXAMPLES "Build the examples" ON)



# Set source files
set(XERUS_SOURCE_FILES
        src/xerus.cpp
        src/BaseGame.cpp
        src/Buffer.cpp
        src/Camera.cpp
        src/Collision.cpp
        src/Image.cpp
        src/Mesh.cpp
        src/OpenGL.cpp
        src/RenderBatch.cpp
        src/Renderer.cpp
        src/Shader.cpp
        src/stdafx.cpp
        src/Texture.cpp
        src/Utility.cpp
        src/VectorMath.cpp
        src/Vertex.cpp
        src/Window.cpp
        src/BitmapFont.cpp)

# Set header files
set(XERUS_HEADER_FILES
        include/BaseGame.h
        include/Buffer.h
        include/Camera.h
        include/Collision.h
        include/Constants.h
        include/Image.h
        include/Interpolation.h
        include/Mesh.h
        include/OpenGL.h
        include/RenderBatch.h
        include/Renderer.h
        include/Shader.h
        include/stdafx.h
        include/Texture.h
        include/Utility.h
        include/VectorMath.h
        include/Vertex.h
        include/Window.h
        include/Xerus.h
        include/BitmapFont.h)

# Create the library
add_library(xerus ${XERUS_SOURCE_FILES} ${XERUS_HEADER_FILES})


target_link_libraries(xerus picopng glt glew_s glfw ${GLFW_LIBRARIES})
target_include_directories(xerus PUBLIC "include")



add_subdirectory("examples")

if (XERUS_BUILD_EXAMPLES)
endif()


