cmake_minimum_required(VERSION 2.6)

set (PROJECT opengl-spheres)

project(${PROJECT})

if (UNIX)

	set(SOURCES lab.c gui.c gl.c)

	find_package(GTK2)
	find_package(OpenGL)
	find_package(PkgConfig)

	pkg_check_modules(GTKGL REQUIRED gtkgl-2.0)

	include_directories(${GTK2_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR}
		${GTKGL_INCLUDE_DIRS})
	
	add_executable(${PROJECT} ${SOURCES})
	
	target_link_libraries(${PROJECT} ${GTK2_LIBRARIES} ${OPENGL_gl_LIBRARY}
		${OPENGL_glu_LIBRARY} ${GTKGL_LIBRARIES})
endif (UNIX)

