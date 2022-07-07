 # CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 3.14)

#set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_DEBUG_POSTFIX "_d")

project (EODPlayer)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})
LINK_DIRECTORIES( ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
LINK_DIRECTORIES( ${ARCHIVE_OUTPUT_DIRECTORY} )
set(PROJECTS_CONFIGURATION_FILES ${CMAKE_BINARY_DIR}/config)

message("Using compiler: ${CMAKE_CXX_COMPILER_ID}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # Disable C++ exceptions.
  string(REGEX REPLACE "/EH[a-z]+" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHs-c-")
  add_definitions(-D_HAS_EXCEPTIONS=0)

  # Disable RTTI.
  string(REGEX REPLACE "/GR" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR-")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # Enable strict prototype warnings for C code in clang and gcc.
  if(NOT CMAKE_C_FLAGS MATCHES "-Wstrict-prototypes")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wstrict-prototypes")
  endif(NOT CMAKE_C_FLAGS MATCHES "-Wstrict-prototypes")

  # Disable C++ exceptions.
  string(REGEX REPLACE "-fexceptions" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")

  # Disable RTTI.
  string(REGEX REPLACE "-frtti" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

##
# Include module pkg-config for CMake.
##
INCLUDE(FindPkgConfig)

##
# CMAKE_MODULE_PATH:FILEPATH=./FFmpeg-Video-Player/Modules
##
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/Modules)

##
# Find FFmpeg include directories and link libraries.
##
find_package(FFMPEG REQUIRED)

# Set new policy when searching for external libs
cmake_policy(SET CMP0074 NEW)

# Include GLFW
find_package(glfw3 REQUIRED)

# Include vulkan
#find_package(Vulkan REQUIRED)

# Include GLM
#include_directories(${PROJECT_SOURCE_DIR}/external/vulkan-framework/external/glm/glm)

# Include Vulkan-Framework
#include_directories(${PROJECT_SOURCE_DIR}/external/vulkan-framework/include)
#add_subdirectory(${PROJECT_SOURCE_DIR}/external/vulkan-framework)

#include_directories(${CMAKE_CURRENT_LIST_DIR}/external/vulkan-framework/external/memory/include)

include_directories(${PROJECT_SOURCE_DIR}/include)

#include_directories(${Vulkan_INCLUDE_DIRS})

add_subdirectory (source)


