#include <gtkgl/gtkglarea.h>
#include <GL/gl.h>
#include <math.h>

#include "gl.h"

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

