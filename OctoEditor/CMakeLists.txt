CMAKE_MINIMUM_REQUIRED(VERSION 3.4)

PROJECT(OctoEditor)

SET(HEADERS
	"Public/OctoEd.h"
)

SET(SOURCES
	"Private/OctoEd.cpp"
)
SOURCE_GROUP("Public" FILES ${HEADERS})
SOURCE_GROUP("Private" FILES ${SOURCES})

ADD_LIBRARY(${PROJECT_NAME} STATIC
	${HEADERS}
	${SOURCES}
)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} PUBLIC
	debug ${ASSIMP_LIBRARY_DEBUG}
	debug ${ZLIB_LIBRARY_DEBUG}
	optimized ${ASSIMP_LIBRARY_RELEASE}
	optimized ${ZLIB_LIBRARY_RELEASE}
	)



TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}
	)