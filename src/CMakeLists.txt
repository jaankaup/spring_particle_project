# Specify the minimum version for CMake # cmake -H. -Bbuild
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

# Debug.
if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    message("Debug Build")
    add_compile_definitions(DEBUG_PROJECT)
    #    add_compile_definitions(EMSCRIPTEN)
    #    add_compile_definitions(__cplusplus)

# Release. Tanne optimoinnit ym. 
elseif(${CMAKE_BUILD_TYPE} MATCHES Release)
    message("Release Build")
endif()

set (graphics_dir ${src_dir}/Graphics)
set (soil_dir ${src_dir}/soil)
set (util_dir ${src_dir}/Utils)

set(graphics_src 
      ${graphics_dir}/window.cpp
      ${graphics_dir}/shader.cpp
      ${graphics_dir}/model.cpp
      ${graphics_dir}/vertexbuffer.cpp
      ${graphics_dir}/vertexAttributes.cpp
      ${graphics_dir}/renderer.cpp
      ${graphics_dir}/texture.cpp
      ${graphics_dir}/camera.cpp
      #${graphics_dir}/textureManager.cpp
      ${graphics_dir}/programstate.cpp
      ${graphics_dir}/textureData.cpp
      # ${graphics_dir}/timer.cpp
      # ${graphics_dir}/vertexBufferManager.cpp
      ${graphics_dir}/modelManager.cpp
      )

set(util_src 
      ${util_dir}/log.cpp
      ${util_dir}/misc.cpp
      )

set (soil_src
	${soil_dir}/image_DXT.c
       	${soil_dir}/image_helper.c
	${soil_dir}/SOIL.c
	${soil_dir}/stb_image_aug.c
	)


set (headers ${CMAKE_SOURCE_DIR}/headers)
message("libraries : ${libraries}")
message("headers : ${headers}")

# Jos luodaan webassemlya.
if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    message("EMSCRITPEN build.")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s ALLOW_MEMORY_GROWTH=1 -s TOTAL_MEMORY=1000MB -O2 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='[\"png\"]'") 
#endif()

# Luodaan perinteinen ohjelma.

# Visual studio
#elseif ( MSVC )
#  message("NATIVE win32 build.")
#  include_directories("${CMAKE_SOURCE_DIR}/includeVisualStudio")
#  #  set(CMAKE_LIBRARY_PATH "${CMAKE_SOURCE_DIR}/lib")
## set(GLEW_LIBRARIES "${CMAKE_SOURCE_DIR}/lib/glew32.lib") 
##  set(OPENGL_LIBRARIES "${CMAKE_SOURCE_DIR}/lib/SDL2.lib ${CMAKE_SOURCE_DIR}/lib/SDL2main.lib") 
##  set(SDL2_LIBRARIES "${CMAKE_SOURCE_DIR}/lib/SDL2.lib ${CMAKE_SOURCE_DIR}/lib/SDL2main.lib") 
#	find_package(SDL2)
#	if (NOT SDL2_FOUND)
#		message("Couldn't find SDL2!!!")
#	else ()
#    	include_directories(${SDL2_INCLUDE_DIRS})
#	message("found SDL ${SDL2_MAJOR}.${SDL2_MINOR}.${SDL2_MICRO} (${SDL2_LIBRARIES}:${SDL2_INCLUDE_DIRS})")
#	endif ()
#
#	find_package(GLEW)
#	if (NOT GLEW_FOUND)
#		message("Couldn't find GLEW!!!")
#	else ()
#		include_directories(${GLEW_INCLUDE_DIRS})
#		message("found GLEW (${GLEW_LIBRARIES}:${GLEW_INCLUDE_DIRS})")
#	endif ()
#
#	find_package(OpenGL)
#	if (NOT OPENGL_FOUND)
#		message("Couldn't find openGL!!!")
#	else ()
#          include_directories(${OPENGL_INCLUDE_DIRS})
#	  message("found openGL (${OPENGL_LIBRARIES})")
#	endif ()
#endif()

# Joku muu
#else () #if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
#	message("NATIVE linux build.")
#
#        include_directories("${CMAKE_SOURCE_DIR}/includeLinux")
##        include_directories("${CMAKE_SOURCE_DIR}/include")
#	#find_package(SDL2 COMPONENTS image)
#	find_package(SDL2)
#	if (NOT SDL2_FOUND)
#		message("Couldn't find SDL2!!!")
#	else ()
#    	include_directories(${SDL2_INCLUDE_DIRS})
#	message("found SDL ${SDL2_MAJOR}.${SDL2_MINOR}.${SDL2_MICRO} (${SDL2_LIBRARIES}:${SDL2_INCLUDE_DIRS})")
#	endif ()
#
#	set (OpenGL_GL_PREFERENCE LEGACY)
#	find_package(OpenGL)
#	if (NOT OPENGL_FOUND)
#		message("Couldn't find openGL!!!")
#	else ()
#          include_directories(${OPENGL_INCLUDE_DIRS})
#	  message("found openGL (${OPENGL_LIBRARIES})")
#	endif ()
#
#	find_package(GLEW)
#	if (NOT GLEW_FOUND)
#		message("Couldn't find GLEW!!!")
#	else ()
#		include_directories(${GLEW_INCLUDE_DIRS})
#		message("found GLEW (${GLEW_LIBRARIES}:${GLEW_INCLUDE_DIRS})")
#	endif ()
#
	# Copy the native shaders to the build directory.
	#add_custom_command(TARGET hello POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/shadersNative $<TARGET_FILE_DIR:hello>/shaders)

	add_custom_command(TARGET hello 
		           POST_BUILD
			   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/shadersNative ${CMAKE_BINARY_DIR}/bin/shaders
			   COMMENT "Copying shaders."
			   VERBATIM)

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}") 
endif()
#
	#        set_target_properties(Engine PROPERTIES LINK_FLAGS "-o dist/engine.js -s USE_FREETYPE=1 -s DISABLE_EXCEPTION_CATCHING=0 -s DEMANGLE_SUPPORT=1 -s SAFE_HEAP=1 --bind -s WASM=1 -O2 -s LEGACY_GL_EMULATION=0  -s GL_UNSAFE_OPTS=0 --pre-js pre-module.js --post-js post-module.js -s ASSERTIONS=1 -s GL_ASSERTIONS=1 -s INVOKE_RUN=0  -std=c++11 -s USE_WEBGL2=1 -s FULL_ES3=1 -s USE_GLFW=3 -s OFFSCREENCANVAS_SUPPORT=1 --preload-file textures --preload-file shaders --preload-file fonts")

# MAKE_RUNTIME_OUTPUT_DIRECTORY or EXECUTABLE_OUTPUT_PATH. Where to create the executable. 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Library output path.
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

# Tanne alki-kirjastot
#add_subdirectory(lib/math)

# Alikirjaston nimi, tiedostot ja tyyppi.
#add_library(math SHARED lib/math/operations.cpp)
#add_library(math STATIC lib/math/operations.cpp)


add_executable(hello ${src_dir}/hello.cpp ${util_src} ${graphics_src} ${soil_src})

#Tanne linkitykset
if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")

   set_target_properties(hello PROPERTIES LINK_FLAGS "-s DISABLE_EXCEPTION_CATCHING=2 -s DEMANGLE_SUPPORT=1 -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s GL_ASSERTIONS=1 -s USE_WEBGL2=1 -s FULL_ES3=1 -s FULL_ES2=1 --bind --preload-file ${CMAKE_SOURCE_DIR}/shaders@shaders/ --preload-file ${CMAKE_SOURCE_DIR}/assets/rock.jpg@assets/ ")

else ()
	target_link_libraries(hello ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
endif()
