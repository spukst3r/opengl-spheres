#include <math.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gtkgl/gtkglarea.h>
#include <GL/gl.h>

#include "gui.h"

#define SHAPE_COUNT 10

GtkWidget *main_window;

void draw_sphere(double r, int lats, int longs)
{
	int i, j;

	for (i=0; i<=lats; i++)
	{
		double lat0 = M_PI * (-0.5 + (double)(i-1) / lats);
		double z0 = sin(lat0);
		double zr0 = cos(lat0);

		double lat1 = M_PI * (-0.5 + (double)i / lats);
		double z1 = sin(lat1);
		double zr1 = cos(lat1);

		glBegin(GL_QUAD_STRIP);

		for (j=0; j<=longs; j++)
		{
			double lng = 2 * M_PI * (double)(j - 1) / longs,
				   x = cos(lng),
				   y = sin(lng);

			glNormal3f(x * zr0, y * zr0, z0);
			glVertex3f(x * zr0, y * zr0, z0);
			glNormal3f(x * zr1, y * zr1, z1);
			glVertex3f(x * zr1, y * zr1, z1);
		}

		glEnd();
	}
}

gint init(GtkWidget *widget)
{
	if (gtk_gl_area_make_current(GTK_GL_AREA(widget)))
	{
		glViewport(0, 0, widget->allocation.width, widget->allocation.height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
	return TRUE;
}

gint draw(GtkWidget *widget, GdkEventExpose *event)
{
	if (event->count > 0)
		return TRUE;
	if (gtk_gl_area_make_current(GTK_GL_AREA(widget)))
	{
		static GLfloat x = 0;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0, 0, 0, 1);

		glLoadIdentity();

		//glRotatef(x+=0.1, 0, 1, 0);
		glShadeModel(GL_SMOOTH);
		draw_sphere(0.5, 15, 15);

		gtk_gl_area_swapbuffers(GTK_GL_AREA(widget));
	}
	return TRUE;
}

gint reshape(GtkWidget *widget, GdkEventConfigure *event)
{
	if (gtk_gl_area_make_current(GTK_GL_AREA(widget)))
	{
		int w = widget->allocation.width,
			h = widget->allocation.height;

		glViewport(0, 0, w, h);
		gluPerspective(45.0f, (GLfloat)(w)/(GLfloat)h, 0.1f, 100.0f);
	}
	return TRUE;
}

int main(int argc, char **argv)
{
	GtkWidget *glarea,
	// Boxes
			  *vbox,
	// Menus
			  *menubar,
	// Expander to show settings
			  *expander,
	// Tables for tabs
			  *sphere1,
			  *sphere2,
			  *light,
			  *light_vbox,
	// Aligns for labels
			  *aligns[13],
	// Labels
			  *labels[13],
	// Settings widgets
			  *color_buttons[11],
			  *spins[2],
	// Notebook (tab pages)
			  *notebook;

	const gchar *labels_text[] = {
		"Ambient:",
		"Diffusion:",
		"Specular:",
		"Emission:",
		"Shininess:",
	};

	gboolean t = TRUE;

	int i, j, c = 0;
	int attrlist[] = {
		GDK_GL_RGBA,
		GDK_GL_RED_SIZE,1,
		GDK_GL_GREEN_SIZE,1,
		GDK_GL_BLUE_SIZE,1,
		GDK_GL_DOUBLEBUFFER,
		GDK_GL_NONE
	};

	gtk_init(&argc, &argv);

	if (gdk_gl_query() == FALSE)
	{
		g_print("OpenGL not supported\n");
		return 0;
	}

	main_window = gtk_window_new( GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(main_window), "OpenGL");
	gtk_container_set_border_width(GTK_CONTAINER(main_window), 2);
	gtk_signal_connect(GTK_OBJECT(main_window), "delete_event",
			GTK_SIGNAL_FUNC(gtk_main_quit), NULL);

	gtk_quit_add_destroy(1, GTK_OBJECT(main_window));

	glarea = GTK_WIDGET(gtk_gl_area_new(attrlist));
	gtk_widget_set_events(GTK_WIDGET(glarea),
			GDK_EXPOSURE_MASK|
			GDK_BUTTON_PRESS_MASK);

	gtk_signal_connect(GTK_OBJECT(glarea), "expose_event",
			GTK_SIGNAL_FUNC(draw), NULL);
	gtk_signal_connect(GTK_OBJECT(glarea), "configure_event",
			GTK_SIGNAL_FUNC(reshape), NULL);
	gtk_signal_connect(GTK_OBJECT(glarea), "realize",
			GTK_SIGNAL_FUNC(init), NULL);

	gtk_signal_connect(GTK_OBJECT(main_window), "key-press-event",
			G_CALLBACK(key_press), NULL);

	gtk_widget_set_size_request(GTK_WIDGET(glarea), 480, 480);

	menubar = create_menu(main_window);
	expander = gtk_expander_new("Settings");

	// aligns and labels
	for (i=0; i<13; i++)
	{
		labels[i] = gtk_label_new(labels_text[i % 5]);
		aligns[i] = gtk_alignment_new(0.3, 0.5, 0.0, 0.0);
		gtk_container_add(GTK_CONTAINER(aligns[i]), labels[i]);
	}

	// Color buttons and spins
	for (i=0; i<11; i++)
	{
		color_buttons[i] = gtk_color_button_new();
		if (i < 2)
			spins[i] = gtk_spin_button_new_with_range(0.0, 100.0, 0.5);
	}

	// Boxes for tabs
	sphere1 = gtk_table_new(2, 5, TRUE);
	sphere2 = gtk_table_new(2, 5, TRUE);
	light = gtk_table_new(2, 3, TRUE);

	for (i=0; i<5; i++)
	{
		// Sphere tab:
		gtk_table_attach(GTK_TABLE(sphere1), aligns[i],
				0, 1, i, i+1, GTK_FILL, GTK_FILL, 0, 0);
		if (i < 4)
				gtk_table_attach(GTK_TABLE(sphere1), color_buttons[i],
					1, 2, i, i+1, GTK_FILL, GTK_FILL, 0, 0);

		// Glass sphere tab:
		gtk_table_attach(GTK_TABLE(sphere2), aligns[i + 5],
				0, 1, i, i+1, GTK_FILL, GTK_FILL, 0, 0);
		if (i < 4)
				gtk_table_attach(GTK_TABLE(sphere2), color_buttons[i+4],
					1, 2, i, i+1, GTK_FILL, GTK_FILL, 0, 0);

		// Light tab:
		if (i < 3)
		{
			gtk_table_attach(GTK_TABLE(light), aligns[i + 10], 0, 1, i, i+1,
					GTK_FILL, GTK_FILL, 0, 0);
			gtk_table_attach(GTK_TABLE(light), color_buttons[i+8], 1, 2, i, i+1,
					GTK_FILL, GTK_FILL, 0, 0);
		}
	}

	gtk_table_attach(GTK_TABLE(sphere1), spins[0], 1, 2, 4, 5, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(sphere2), spins[1], 1, 2, 4, 5, GTK_FILL, GTK_FILL, 0, 0);

	gtk_table_set_col_spacing(GTK_TABLE(sphere1), 0, 100);
	gtk_table_set_col_spacing(GTK_TABLE(sphere2), 0, 100);
	gtk_table_set_col_spacing(GTK_TABLE(light), 0, 100);

	// Notebook
	notebook = gtk_notebook_new();

	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_BOTTOM);

	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), sphere1, gtk_label_new("Sphere"));
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), sphere2, gtk_label_new("Glass sphere"));
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), light, gtk_label_new("Light"));

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), glarea, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), notebook, FALSE, FALSE, 0);

	gtk_container_add(GTK_CONTAINER(main_window), vbox);

	gtk_widget_show_all(GTK_WIDGET(main_window));

	g_timeout_add(10, (GSourceFunc)timer_event, (gpointer)main_window);

	gtk_main();

	return 0;
}

