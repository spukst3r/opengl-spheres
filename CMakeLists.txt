cmake_minimum_required(VERSION 2.6)

set (PROJECT opengl-spheres)

project(${PROJECT})

set(SOURCES lab.c gui.c gl.c)

find_package(GTK2)
find_package(OpenGL)

set (GTK_GL_INCLUDE_DIR /usr/include/gtkgl-2.0
	/usr/include/gtk-2.0 /usr/lib64/gtk-2.0/include /usr/include/atk-1.0
	/usr/include/cairo /usr/include/gdk-pixbuf-2.0
	/usr/include/pango-1.0 /usr/include/glib-2.0
	/usr/lib64/glib-2.0/include /usr/include/pixman-1
	/usr/include/freetype2 /usr/include/libpng14 /usr/include/libdrm)
set (GTK_GL_LIBRARIES gtkgl-2.0 GLU GL gtk-x11-2.0 gdk-x11-2.0
	atk-1.0 gio-2.0 pangoft2-1.0 pangocairo-1.0 gdk_pixbuf-2.0 m
	cairo pango-1.0 freetype fontconfig gobject-2.0 gmodule-2.0
	gthread-2.0 rt glib-2.0)

include_directories(${GTK2_INCLUDE_DIRS} ${GTK_GL_INCLUDE_DIR}
	${OPENGL_INCLUDE_DIR})

add_executable(${PROJECT} ${SOURCES})

target_link_libraries(${PROJECT} ${GTK2_LIBRARIES} ${OPENGL_gl_LIBRARY}
	${OPENGL_glu_LIBRARY} ${GTK_GL_LIBRARIES})

