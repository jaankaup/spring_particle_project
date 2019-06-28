# Specify the minimum version for CMake
cmake_minimum_required(VERSION 2.8)

# Project's name
project(MainProject)
set (src_dir ${CMAKE_SOURCE_DIR}/src)
set (asset_dir ${CMAKE_SOURCE_DIR}/assets)
message(${src_dir}!!!!!)

set ( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" )
set ( MSVC_WARNING_LEVEL 3 CACHE STRING "Visual Studio warning levels" )

# Disable in-source builds to prevent source tree corruption.
if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "
FATAL: In-source builds are not allowed.
       You should create a separate directory for build files.
")
endif()

# The following folder will be included
include_directories("${CMAKE_SOURCE_DIR}/includes")
include_directories("${CMAKE_SOURCE_DIR}/includes/soil")

if ( WIN32 )
  set ( default_deps_dir "${CMAKE_SOURCE_DIR}/deps" )
  set ( application_type WIN32 )
endif ()

set ( DEPS_DIR "${default_deps_dir}" CACHE STRING "Dependencies directory" )

if ( IS_DIRECTORY ${DEPS_DIR} )
	#include_directories ( ${DEPS_DIR}/include/SDL ${DEPS_DIR}/include/GL)
   message("DEPS DIRECTORY")
   set(SDL2_INCLUDE_DIRS "${DEPS_DIR}/include")
   
   message("SDL2_INCLUDE_DIRS = ${SDL2_INCLUDE_DIRS}")

   # Support both 32 and 64 bit builds
   if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
	   set(SDL2_LIBRARIES "${DEPS_DIR}/lib/x64/SDL2.lib;${DEPS_DIR}/lib/x64/SDL2main.lib")
   else ()
	   set(SDL2_LIBRARIES "${DEPS_DIR}/lib/x86/SDL2.lib;${DEPS_DIR}/lib/x86/SDL2main.lib")
   endif ()
   string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)

   message("SDL2_LIBRARIES = ${SDL2_LIBRARIES}")


   set(GLEW_INCLUDE_DIRS "${DEPS_DIR}/include")
   
   message("GLEW_INCLUDE_DIRS = ${GLEW_INCLUDE_DIRS}")

   # Support both 32 and 64 bit builds
   if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
	   set(GLEWLIBRARIES "${DEPS_DIR}/lib/x64/glew32.lib")
   else ()
	   set(GLEW_LIBRARIES "${DEPS_DIR}/lib/x86/glew32.lib")
   endif ()

   string(STRIP "${GLEW_LIBRARIES}" GLEW_LIBRARIES)

   message("GLEW_LIBRARIES = ${GLEW_LIBRARIES}")

    	include_directories(${SDL2_INCLUDE_DIRS})
	include_directories(${GLEW_INCLUDE_DIRS})

else ()
	find_package(SDL2)
	if (NOT SDL2_FOUND)
		message("Couldn't find SDL2!!!")
	else ()
    	include_directories(${SDL2_INCLUDE_DIRS})
	message("found SDL ${SDL2_MAJOR}.${SDL2_MINOR}.${SDL2_MICRO} (${SDL2_LIBRARIES}:${SDL2_INCLUDE_DIRS})")
	endif ()

	find_package(GLEW)
	if (NOT GLEW_FOUND)
		message("Couldn't find GLEW!!!")
	else ()
		include_directories(${GLEW_INCLUDE_DIRS})
		message("found GLEW (${GLEW_LIBRARIES}:${GLEW_INCLUDE_DIRS})")
	endif ()

	endif()

find_package(OpenGL)
if (NOT OPENGL_FOUND)
	message("Couldn't find openGL!!!")
else ()
  include_directories(${OPENGL_INCLUDE_DIRS})
  message("found openGL (${OPENGL_LIBRARIES})")
endif ()

add_subdirectory("${src_dir}")
