#include <gtk/gtk.h>

#include "gui.h"

extern GtkWidget *main_window;
extern GdkColor colors[COLOR_BUTTON_COUNT];

static GtkActionEntry entries[] = {
	{ "FileMenuAction", NULL, "_File" },
	{ "HelpMenuAction", NULL, "_Help" },

	{ "QuitAction", GTK_STOCK_QUIT, "_Quit", "<control>q",
		"Quit", G_CALLBACK(gtk_main_quit) },

	{ "AboutAction", GTK_STOCK_ABOUT, "_About", NULL,
		"About", G_CALLBACK(show_about) }
};

void timer_event(GtkWidget *widget)
{
	if (widget->window == NULL)
		return;
	gtk_widget_queue_draw(widget);
}

void init_color_buttons(GtkWidget **buttons)
{
	int i;
	const gchar *str_colors[COLOR_BUTTON_COUNT] = {
		"#95FF00",
		"#EFFF00",
		"#00FF2D",
		"#FFFFFF",
		"#000000",
		"#000000",
		"#000000",
		"#000000",
		"#000000",
		"#000000",
		"#000000"
	};

	for (i=0; i<COLOR_BUTTON_COUNT; i++)
	{
		gdk_color_parse(str_colors[i], &colors[i]);
		gtk_color_button_set_color(GTK_COLOR_BUTTON(buttons[i]), &colors[i]);
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

