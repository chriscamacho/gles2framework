cmake_minimum_required(VERSION 2.6)
project( gles2framework )

if(MINGW OR UNIX)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-attributes")
  set_property(GLOBAL PROPERTY COMPILE_FLAGS "/ZI")
endif()

include_directories( include )
include_directories( kazmath/kazmath )
include_directories( glew/include )
include_directories( glfw/include )

link_directories( ${CMAKE_CURRENT_SOURCE_DIR}/glfw/lib )
link_directories( ${CMAKE_CURRENT_SOURCE_DIR}/glew/lib )

add_definitions( "-D__FOR_GLFW__" )

file( GLOB GLES2FRAMEWORK_HEADERS include/*.h )
file( GLOB GLES2FRAMEWORK_SOURCES src/*.c )
file( GLOB_RECURSE KAZMATH_HEADERS kazmath/kazmath/*.h )
file( GLOB_RECURSE KAZMATH_SOURCES kazmath/kazmath/*.c )

source_group( kazmath FILES ${KAZMATH_HEADERS} ${KAZMATH_SOURCES})
source_group( inc FILES ${GLES2FRAMEWORK_HEADERS} )
source_group( src FILES ${GLES2FRAMEWORK_SOURCES} )

add_library( gles2framework STATIC 
	${GLES2FRAMEWORK_HEADERS} 
	${GLES2FRAMEWORK_SOURCES} 
	${KAZMATH_HEADERS}
	${KAZMATH_SOURCES} )
	
file( GLOB EXAMPLE_SIMPLE examples/simple.c )
file( GLOB EXAMPLE_SPRITES examples/sprites.c )

file(GLOB C_FILES 
	${GLES2FRAMEWORK_SOURCES}
	${KAZMATH_SOURCES}
	${EXAMPLE_SIMPLE}
	${EXAMPLE_SPRITES})
	
set_source_files_properties(${C_FILES} PROPERTIES LANGUAGE CXX)

# Executables for only Standalone project
if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})

add_executable( simple ${EXAMPLE_SIMPLE} )
add_executable( sprites ${EXAMPLE_SPRITES} )

target_link_libraries( simple gles2framework GLFW glew32 opengl32 )
target_link_libraries( sprites gles2framework GLFW glew32 opengl32 )

endif()
