#include <gtk/gtk.h>
#include <gtkgl/gtkglarea.h>

#include "gui.h"
#include "gl.h"

extern GtkWidget *main_window;
extern GLfloat colors[COLOR_BUTTON_COUNT][4];

static GtkActionEntry entries[] = {
	{ "FileMenuAction", NULL, "_File" },
	{ "HelpMenuAction", NULL, "_Help" },

	{ "QuitAction", GTK_STOCK_QUIT, "_Quit", "<control>q",
		"Quit", G_CALLBACK(gtk_main_quit) },

	{ "AboutAction", GTK_STOCK_ABOUT, "_About", NULL,
		"About", G_CALLBACK(show_about) }
};

GLfloat shininess_sphere,
	 shininess_glass;

void spin_button_value_changed(GtkSpinButton *widget,
		gpointer data)
{
	GtkSpinButton *spin = GTK_SPIN_BUTTON(widget);
	gintptr n_spin = (gintptr)data;

	switch (n_spin)
	{
		case 0:
			shininess_sphere = gtk_spin_button_get_value(spin);
			break;
		case 1:
			shininess_glass = gtk_spin_button_get_value(spin);
			break;
	}
}

void color_button_value_changed(GtkColorButton *widget,
		gpointer data)
{
	GtkColorButton *cbutton = GTK_COLOR_BUTTON(widget);
	gintptr n_button = (gintptr)data;
	GdkColor col;

	gtk_color_button_get_color(cbutton, &col);

	colors[n_button][0] = col.red   / 65536.0;
	colors[n_button][1] = col.green / 65536.0;
	colors[n_button][2] = col.blue  / 65536.0;
}

void timer_event(GtkWidget *widget)
{
	if (widget->window == NULL)
		return;
	gtk_widget_queue_draw(widget);
}

void create_widgets(int *argc, char ***argv)
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
			  *color_buttons[COLOR_BUTTON_COUNT],
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

	gtk_init(argc, argv);

	if (gdk_gl_query() == FALSE)
	{
		g_print("OpenGL not supported\n");
		gtk_exit(-1);
	}

	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
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
	for (i=0; i<COLOR_BUTTON_COUNT; i++)
	{
		color_buttons[i] = gtk_color_button_new();
		gtk_signal_connect(GTK_OBJECT(color_buttons[i]), "color-set",
				GTK_SIGNAL_FUNC(color_button_value_changed), (gpointer)(gintptr)i);

		if (i < 2)
			spins[i] = gtk_spin_button_new_with_range(0.0, 128.0, 0.5);
	}
	gtk_signal_connect(GTK_OBJECT(spins[0]), "value-changed",
			GTK_SIGNAL_FUNC(spin_button_value_changed), (gpointer)0);
	gtk_signal_connect(GTK_OBJECT(spins[1]), "value-changed",
			GTK_SIGNAL_FUNC(spin_button_value_changed), (gpointer)1);

	init_color_buttons(color_buttons);

	// Boxes for tabs
	sphere = gtk_table_new(3, 6, FALSE);
	light = gtk_table_new(2, 3, FALSE);

	gtk_alignment_set(GTK_ALIGNMENT(aligns[8]), 0.5, 0.5, 0.0, 0.0);
	gtk_alignment_set(GTK_ALIGNMENT(aligns[9]), 0.5, 0.5, 0.0, 0.0);

	gtk_table_attach(GTK_TABLE(sphere), aligns[8], 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(sphere), aligns[9], 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);

	gtk_table_attach(GTK_TABLE(sphere), spins[0], 1, 2, 5, 6, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(sphere), spins[1], 2, 3, 5, 6, GTK_FILL, GTK_FILL, 0, 0);

	for (i=0; i<5; i++)
	{
		gtk_widget_set_size_request(aligns[i], 150, -1);
		gtk_table_attach(GTK_TABLE(sphere), aligns[i], 0, 1, i+1, i+2, GTK_FILL, GTK_FILL, 0, 0);

		if (i < 4)
		{
			gtk_widget_set_size_request(color_buttons[i],     120, -1);
			gtk_widget_set_size_request(color_buttons[i + 4], 120, -1);

			gtk_table_attach(GTK_TABLE(sphere), color_buttons[i], 1, 2, i+1, i+2, GTK_FILL, GTK_FILL, 0, 0);
			gtk_table_attach(GTK_TABLE(sphere), color_buttons[i + 4], 2, 3, i+1, i+2, GTK_FILL, GTK_FILL, 0, 0);
		}

		if (i < 3)
		{
			gtk_widget_set_size_request(aligns[i + 5], 150, -1);
			gtk_widget_set_size_request(color_buttons[i + 8], 120, -1);

			gtk_table_attach(GTK_TABLE(light), aligns[i + 5], 0, 1, i+1, i+2, GTK_FILL, GTK_FILL, 0, 0);
			gtk_table_attach(GTK_TABLE(light), color_buttons[i + 8], 1, 2, i+1, i+2, GTK_FILL, GTK_FILL, 0, 0);
		}
	}

	// Notebook
	notebook = gtk_notebook_new();

	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_BOTTOM);

	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), sphere, gtk_label_new("Spheres"));
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), light, gtk_label_new("Light"));

	gtk_container_add(GTK_CONTAINER(expander), notebook);

	main_vbox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(main_vbox), menubar, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(main_vbox), glarea, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(main_vbox), expander, FALSE, FALSE, 0);

	gtk_container_add(GTK_CONTAINER(main_window), main_vbox);

	gtk_widget_show_all(GTK_WIDGET(main_window));

	g_timeout_add(50, (GSourceFunc)timer_event, (gpointer)main_window);
}

void init_color_buttons(GtkWidget **buttons)
{
	int i;
	GdkColor col;
	const gchar *str_colors[COLOR_BUTTON_COUNT] = {
		"#95FF00",
		"#EFFF00",
		"#00FF2D",
		"#000000",
		"#000000",
		"#000000",
		"#000000",
		"#000000",
		"#6F6F6F",
		"#FFFFFF",
		"#000000"
	};

	for (i=0; i<COLOR_BUTTON_COUNT; i++)
	{
		gdk_color_parse(str_colors[i], &col);

		colors[i][0] = col.red   / 65536.0;
		colors[i][1] = col.green / 65536.0;
		colors[i][2] = col.blue  / 65536.0;

		gtk_color_button_set_color(GTK_COLOR_BUTTON(buttons[i]), &col);
	}
}

void show_about()
{
	GtkWidget *dialog;

	dialog = gtk_message_dialog_new(GTK_WINDOW(main_window), GTK_DIALOG_MODAL,
			GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
			"OpenGL 'Spheres' lab.\nMade by Dima Fontanov (055)");

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK);
		gtk_widget_destroy(dialog);
}

GdkColor choose_color()
{
	GtkWidget *color_picker;
	GtkColorSelection *colorsel;
	GtkColorSelectionDialog color_dialog;
	GdkColor color;

	color_picker = gtk_color_selection_dialog_new("Choose point color");
	colorsel = GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(color_picker)->colorsel);

	gtk_color_selection_set_previous_color(colorsel, &color);
	gtk_color_selection_set_current_color(colorsel, &color);

	if (gtk_dialog_run(GTK_DIALOG(color_picker)) == GTK_RESPONSE_OK)
		gtk_color_selection_get_current_color(colorsel, &color);

	gtk_widget_hide(color_picker);

	return color;
}

gboolean key_press(GtkWidget *widget, GdkEventKey *key, gpointer userdata)
{
	if (key->type == GDK_KEY_PRESS)
	{
		switch (key->keyval)
		{
		}
		gtk_widget_queue_draw(widget);
	}
	return FALSE;
}

GtkWidget *create_menu(GtkWidget *window)
{
	guint n_entries = G_N_ELEMENTS(entries);
	GError *err = NULL;

	GtkActionGroup *action_group;
	GtkUIManager *menu_manager;

	action_group = gtk_action_group_new("Actions");
	gtk_action_group_add_actions(action_group, entries, n_entries, NULL);

	menu_manager = gtk_ui_manager_new();
	gtk_ui_manager_insert_action_group(menu_manager, action_group, 0);

	gtk_ui_manager_add_ui_from_file(menu_manager, "menu.xml", &err);

	if (err)
	{
		g_message("Building menus failed: %s", err->message);
		g_error_free(err);
		gtk_exit(-1);
	}

	gtk_window_add_accel_group(GTK_WINDOW(window),
			gtk_ui_manager_get_accel_group(menu_manager));

	return gtk_ui_manager_get_widget(menu_manager, "/MainMenu");
}

