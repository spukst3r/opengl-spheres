#ifndef _GUI_H_
#define _GUI_H_

#define COLOR_BUTTON_COUNT 11

GtkWidget *create_menu(GtkWidget *window);
GdkColor choose_color();

gboolean key_press(GtkWidget *widget, GdkEventKey *key, gpointer userdata);

void spin_button_value_changed(GtkSpinButton *widget,
		gpointer data);
void color_button_value_changed(GtkColorButton *widget,
		gpointer data);

void show_about();
void create_widgets(int *argc, char ***argv);
void init_color_buttons();
void timer_event(GtkWidget *widget);

#endif

