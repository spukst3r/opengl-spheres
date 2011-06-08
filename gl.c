#include <gtkgl/gtkglarea.h>
#include <GL/gl.h>
#include <math.h>

#include "gl.h"

#define X 0.525731112119133606 
#define Z 0.850650808352039932

static GLfloat vdata[12][3] = {    
	    {-X, 0.0, Z}, {X, 0.0, Z},  {-X, 0.0, -Z}, {X, 0.0, -Z},
		{0.0, Z, X},  {0.0, Z, -X}, {0.0, -Z, X},  {0.0, -Z, -X},    
		{Z, X, 0.0},  {-Z, X, 0.0}, {Z, -X, 0.0},  {-Z, -X, 0.0} 
};
static GLuint tindices[20][3] = { 
	    {0,4,1},  {0,9,4},  {9,5,4},  {4,5,8},  {4,8,1},    
		{8,10,1}, {8,3,10}, {5,3,8},  {5,2,3},  {2,7,3},    
		{7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6}, 
		{6,1,10}, {9,0,11}, {9,11,2}, {9,2,5},  {7,2,11}
};

void normalize(GLfloat *a)
{
	GLfloat d;

	d = sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);

	a[0] /= d;
	a[1] /= d;
	a[2] /= d;
}

void draw_triangle(GLfloat *a, GLfloat *b, GLfloat *c, int div, GLfloat r)
{
	if (div <= 0)
	{
		glNormal3fv(a); glVertex3f(a[0] * r, a[1] * r, a[2] * r);
		glNormal3fv(b); glVertex3f(b[0] * r, b[1] * r, b[2] * r);
		glNormal3fv(c); glVertex3f(c[0] * r, c[1] * r, c[2] * r);
	}
	else
	{
		GLfloat ab[3], ac[3], bc[3];
		int i;

		for (i=0; i<3; i++)
		{
			ab[i] = (a[i] + b[i]) / 2;
			ac[i] = (a[i] + c[i]) / 2;
			bc[i] = (b[i] + c[i]) / 2;
		}
		normalize(ab), normalize(ac), normalize(bc);

		draw_triangle(a, ab, ac, div - 1, r);
		draw_triangle(b, bc, ab, div - 1, r);
		draw_triangle(c, ac, bc, div - 1, r);
		draw_triangle(ab, bc, ac, div - 1, r);
	}
}

void draw_sphere(int ndiv, float radius)
{
	int i;

	glBegin(GL_TRIANGLES);

	for (i=0; i<20; i++)
		draw_triangle(vdata[tindices[i][0]],
				vdata[tindices[i][1]],
				vdata[tindices[i][2]],
				ndiv, radius);
	glEnd();
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

		glShadeModel(GL_SMOOTH);

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

		glRotatef(x+=0.1, 0, 1, 0);
		draw_sphere(2, 1.0);

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
		glLoadIdentity();
		gluPerspective(45.0, ((GLfloat)w)/((GLfloat)h), 0.0, 100.0);
	}
	return TRUE;
}

