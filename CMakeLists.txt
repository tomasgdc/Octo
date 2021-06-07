#-------------------------------------------------------------------
#		Copyright 2021 Tomas Mikalauskas
# This file is part of the CMake build system for Octo
#     (Object-oriented Graphics Rendering Engine)
#
# The contents of this file are placed in the public domain. Feel
# free to make use of it in any way you like.
#-----------------------------------------------------------------


######################################################################
# This is the main file where we prepare the general build environment
# and provide build configuration options.
######################################################################


CMAKE_MINIMUM_REQUIRED(VERSION 3.4)
PROJECT(Octo)


# Reduce the config types to only Debug and Release
SET(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
IF(DEFINED CMAKE_BUILD_TYPE AND CMAKE_VERSION VERSION_GREATER "3.4")
  SET_PROPERTY(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS  ${CMAKE_CONFIGURATION_TYPES})
ENDIF()
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} --my-debug-flags")
SET(CMAKE_DEBUG_POSTFIX "_d")

#Add External Include Dir that for GLI
INCLUDE_DIRECTORIES(
	"${CMAKE_SOURCE_DIR}/ThirdParty/glm"
	"${CMAKE_SOURCE_DIR}/ThirdParty/gli"
	"${CMAKE_SOURCE_DIR}/ThirdParty/assimp"
	"${CMAKE_SOURCE_DIR}/ThirdParty/freetype/include"
)

# Set preprocessor defines
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOMINMAX -D_USE_MATH_DEFINES")
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-std=c++14)
add_definitions(-std=c++0x)
add_definitions(-std=gnu++14)


SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMakeModules")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

find_package(ASSIMP REQUIRED)
include_directories(${ASSIMP_INCLUDE_DIR})

set(FREETPYE_PATH "${CMAKE_SOURCE_DIR}/ThirdParty/freetype/include")
set(FREETYPE_LIB_SEARCH_PATH "${CMAKE_SOURCE_DIR}/ThirdParty/freetype/objs/vc2017/x64")

find_package(Freetype_custom QUIET)
include_directories(${FREETYPE_INCLUDE_DIR})

#As we are building Assimp as static library. We need to get zlib. Luckily assimp contains zlib and builds it
set(ZLIB_LIB_SEARCH_PATH ${CMAKE_SOURCE_DIR}/ThirdParty/assimp/build/contrib/zlib)
find_package(Zlib REQUIRED)


message(STATUS "Vulkan lib path:   " 		${VULKAN_LIB})
message(STATUS "SDL2 lib path: 	   "   		${SDL2_LIBRARY})
message(STATUS "Assimp lib path:   " 		${ASSIMP_LIBRARIES})
message(STATUS "Freetype lib path: " 		${FREETYPE_LIBRARIES})
message(STATUS "Freetype include path: " 	${FREETYP_INCLUDE_DIR})
message(STATUS "Zlib lib path: "        	${ZLIB_LIBRARIES}) 

ADD_SUBDIRECTORY(Octo)
ADD_SUBDIRECTORY(OctoCore)
ADD_SUBDIRECTORY(OctoEditor)
ADD_SUBDIRECTORY(OctoRenderer)

MESSAGE("Generated with config types: ${CMAKE_CONFIGURATION_TYPES}")
if(CMAKE_CONFIGURATION_TYPES)
    MESSAGE("Multi-configuration generator")
else()
    MESSAGE("Single-configuration generator")
endif()