#ifndef _GUI_H_
#define _GUI_H_

GtkWidget *create_menu(GtkWidget *window);
GdkColor choose_color();

gboolean key_press(GtkWidget *widget, GdkEventKey *key, gpointer userdata);

void show_about();
void timer_event(GtkWidget *widget);

#endif

