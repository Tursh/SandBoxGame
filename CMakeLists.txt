cmake_minimum_required(VERSION 3.14)

set(BUILD_SHARED_LIBS OFF)

#Set the output directory
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")

add_subdirectory(CrossGameEngine)

include_directories(CrossGameEngine/include)

project(SandBoxGame)

set(CMAKE_CXX_STANDARD 17)

if(WIN32)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -Wl,-allow-multiple-definition")
endif()

include_directories(include)

link_libraries(CrossGameEngine)

if (NOT WIN32)
    link_libraries(GL harfbuzz)
else ()
    link_libraries(opengl32)
endif ()

if (NOT BUILD_SHARED_LIBS)
    if (NOT WIN32)
        link_libraries(freetype GLEW glfw tinyxml2 pthread)
    else ()
        link_libraries(freetype glew32 glfw3 tinyxml2)
    endif ()
endif ()

add_executable(SandBoxGame
        src/Main.cpp
        src/States/PlayState.cpp
        include/States/PlayState.h
        src/World/World.cpp
        include/World/World.h
        src/World/Terrain/Chunk.cpp
        include/World/Terrain/Chunk.h
        include/World/Terrain/Bloc.h
        src/World/Terrain/Bloc.cpp
        src/World/Player.cpp
        src/World/Terrain/ChunkManager.cpp
        src/World/Terrain/WorldGenerator.cpp)

add_custom_command(TARGET SandBoxGame PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:SandBoxGame>/res)