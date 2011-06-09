#ifndef _GL_H_
#define _GL_H_

#include <gtk/gtk.h>
#include <GL/gl.h>

void normalize(GLfloat *a);
void draw_triangle(GLfloat *a, GLfloat *b, GLfloat *c, int div, GLfloat f);
void draw_sphere(int ndiv, float radius);

gint init(GtkWidget *widget);
gint draw(GtkWidget *widget, GdkEventExpose *event);
gint reshape(GtkWidget *widget, GdkEventConfigure *event);

#endif

