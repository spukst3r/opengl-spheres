#ifndef _GUI_H_
#define _GUI_H_

#define COLOR_BUTTON_COUNT 11

GtkWidget *create_menu(GtkWidget *window);
GdkColor choose_color();

gboolean key_press(GtkWidget *widget, GdkEventKey *key, gpointer userdata);

void show_about();
void init_color_buttons();
void timer_event(GtkWidget *widget);

#endif

