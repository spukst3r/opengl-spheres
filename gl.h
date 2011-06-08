#ifndef _GL_H_
#define _GL_H_

#include <gtk/gtk.h>

void draw_sphere(double r, int lats, int longs);

gint init(GtkWidget *widget);
gint draw(GtkWidget *widget, GdkEventExpose *event);
gint reshape(GtkWidget *widget, GdkEventConfigure *event);

#endif

