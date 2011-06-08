#include <math.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gtkgl/gtkglarea.h>
#include <GL/gl.h>

#include "gui.h"
#include "gl.h"

#define SHAPE_COUNT 10

GtkWidget *main_window;

int main(int argc, char **argv)
{
	GtkWidget *glarea,
	// Boxes
			  *main_vbox,
	// Menus
			  *menubar,
	// Expander to show settings
			  *expander,
	// Tables for tabs
			  *sphere,
			  *light,
			  *light_main_vbox,
	// Aligns for labels
			  *aligns[10],
	// Labels
			  *labels[10],
	// Settings widgets
			  *color_buttons[13],
			  *spins[2],
	// Notebook (tab pages)
			  *notebook;

	const gchar *labels_text[] = {
		"Ambient:",
		"Diffusion:",
		"Specular:",
		"Emission:",
		"Shininess:",
		"Sphere",
		"Glass",
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

	gtk_widget_set_size_request(GTK_WIDGET(glarea), 480, 320);

	menubar = create_menu(main_window);
	expander = gtk_expander_new("Settings");

	// aligns and labels
	for (i=0; i<10; i++)
	{
		if (i > 7)
		{
			labels[i] = gtk_label_new(labels_text[i - 3]);
			aligns[i] = gtk_alignment_new(0.1, 0.5, 0.0, 0.0);
			gtk_container_add(GTK_CONTAINER(aligns[i]), labels[i]);
			continue;
		}

		labels[i] = gtk_label_new(labels_text[i % 5]);
		aligns[i] = gtk_alignment_new(0.1, 0.5, 0.0, 0.0);
		gtk_container_add(GTK_CONTAINER(aligns[i]), labels[i]);
	}

	// Color buttons and spins
	for (i=0; i<13; i++)
	{
		color_buttons[i] = gtk_color_button_new();
		if (i < 2)
			spins[i] = gtk_spin_button_new_with_range(0.0, 100.0, 0.5);
	}

	// Boxes for tabs
	sphere = gtk_table_new(3, 6, FALSE);
	light = gtk_table_new(2, 3, FALSE);

	gtk_alignment_set(GTK_ALIGNMENT(aligns[8]), 0.5, 0.5, 0.0, 0.0);
	gtk_alignment_set(GTK_ALIGNMENT(aligns[9]), 0.5, 0.5, 0.0, 0.0);

	gtk_table_attach(GTK_TABLE(sphere), aligns[8], 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(sphere), aligns[9], 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);

	for (i=0; i<5; i++)
	{
		gtk_widget_set_size_request(aligns[i], 150, -1);
		gtk_widget_set_size_request(color_buttons[i],     120, -1);
		gtk_widget_set_size_request(color_buttons[i + 5], 120, -1);

		gtk_table_attach(GTK_TABLE(sphere), aligns[i], 0, 1, i+1, i+2, GTK_FILL, GTK_FILL, 0, 0);
		gtk_table_attach(GTK_TABLE(sphere), color_buttons[i], 1, 2, i+1, i+2, GTK_FILL, GTK_FILL, 0, 0);
		gtk_table_attach(GTK_TABLE(sphere), color_buttons[i + 5], 2, 3, i+1, i+2, GTK_FILL, GTK_FILL, 0, 0);

		if (i < 3)
		{
			gtk_widget_set_size_request(aligns[i + 5], 150, -1);
			gtk_widget_set_size_request(color_buttons[i + 10], 120, -1);

			gtk_table_attach(GTK_TABLE(light), aligns[i + 5], 0, 1, i+1, i+2, GTK_FILL, GTK_FILL, 0, 0);
			gtk_table_attach(GTK_TABLE(light), color_buttons[i + 10], 1, 2, i+1, i+2, GTK_FILL, GTK_FILL, 0, 0);
		}
	}

	// Notebook
	notebook = gtk_notebook_new();

	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_BOTTOM);

	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), sphere, gtk_label_new("Spheres"));
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), light, gtk_label_new("Light"));

	main_vbox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(main_vbox), menubar, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(main_vbox), glarea, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(main_vbox), notebook, FALSE, FALSE, 0);

	gtk_container_add(GTK_CONTAINER(main_window), main_vbox);

	gtk_widget_show_all(GTK_WIDGET(main_window));

	g_timeout_add(50, (GSourceFunc)timer_event, (gpointer)main_window);

	gtk_main();

	return 0;
}

