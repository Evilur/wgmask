CMAKE_MINIMUM_REQUIRED(VERSION 4.2)
SET(CMAKE_CXX_STANDARD 20)
PROJECT(wgmask)

# Set compile options
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
    SET(CMAKE_CXX_FLAGS "-g3 -O0 -fdiagnostics-color=always")
    SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
ELSE ()
    SET(CMAKE_CXX_FLAGS "-g0 -O2 -static")
ENDIF ()

# Search header files in the 'src' directory
INCLUDE_DIRECTORIES(src)

# Get all *.cpp files
FILE(GLOB_RECURSE SOURCES "src/*.cpp")

# Build all *.cpp files
ADD_EXECUTABLE(wgmask ${SOURCES})

# Link windows libraries
IF(MINGW)
    TARGET_LINK_LIBRARIES(wgmask ws2_32)
ENDIF()
