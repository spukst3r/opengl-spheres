#include <math.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gtkgl/gtkglarea.h>
#include <GL/gl.h>

#include "gui.h"
#include "gl.h"

GtkWidget *main_window;

GLfloat colors[COLOR_BUTTON_COUNT][4];

int main(int argc, char **argv)
{
	create_widgets(&argc, &argv);

	gtk_main();

	return 0;
}

