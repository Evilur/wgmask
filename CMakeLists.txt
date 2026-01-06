CMAKE_MINIMUM_REQUIRED(VERSION 4.2)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
PROJECT(wgmask)

# Set compile options
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
    SET(CMAKE_CXX_FLAGS "-g3 -O0 -fdiagnostics-color=always")
ELSE ()
    SET(CMAKE_CXX_FLAGS "-g0 -O2")
ENDIF ()

# Search header files in the 'src' directory
INCLUDE_DIRECTORIES(src)

# Get all *.cpp files
FILE(GLOB_RECURSE SOURCES "src/*.cpp")

# Build all *.cpp files
ADD_EXECUTABLE(wgmask ${SOURCES})
