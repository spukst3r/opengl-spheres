#include <gtkgl/gtkglarea.h>
#include <GL/gl.h>
#include <math.h>

#include "gl.h"
#include "gui.h"

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

extern GLfloat colors[COLOR_BUTTON_COUNT][4];
extern GLfloat shininess_sphere,
	   shininess_glass;

GLfloat aspect_ratio;

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
		normalize(ab); normalize(ac); normalize(bc);

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
		//		m_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };

		glClearColor(0, 0, 0, 1);
		glClearDepth(1.0);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glEnable(GL_CULL_FACE);
		glEnable(GL_NORMALIZE);
		glEnable(GL_BLEND);

		//glDepthFunc(GL_LEQUAL);
		//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

		//glLightfv(GL_LIGHT1, GL_DIFFUSE, m_ambient);
		//glLightfv(GL_LIGHT1, GL_AMBIENT, m_ambient);
		//glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.1);

		glShadeModel(GL_SMOOTH);

		glCullFace(GL_BACK);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glViewport(0, 0, widget->allocation.width, widget->allocation.height);
	}
	return TRUE;
}

gint draw(GtkWidget *widget, GdkEventExpose *event)
{
	if (event->count > 0)
		return TRUE;
	if (gtk_gl_area_make_current(GTK_GL_AREA(widget)))
	{
		static GLint x = 0, x2 = 0;
		static GLfloat movement_z = 0,
					   step = 0.05;
		static GLfloat light_pos[] = { 0.0, 0.0, 0.0, 1.0 },
					   light_dir[] = { 0.0, 0.0, 1.0, 0.0 };
		GLfloat l_ambient[]  = { colors[8][0],  colors[8][1],  colors[8][2],  colors[8][3], 1.0 },
				l_diffuse[]  = { colors[9][0],  colors[9][1],  colors[9][2],  colors[9][3], 1.0 },
				l_specular[] = { colors[10][0], colors[10][1], colors[10][2], colors[10][3], 1.0 };
		GLfloat m_sphere[4][4];
		int i, j;

		for (i=0; i<4; i++)
			for (j=0; j<4; j++)
				m_sphere[i][j] = colors[i][j];

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//glOrtho(-2*aspect_ratio, 2*aspect_ratio, -2.0, 2.0, -10.0, 10.0);
		gluPerspective(45.0, aspect_ratio, 2.0, 25.0);

		gluLookAt(0.0, -2.0, -6.0,
				0.0, 0.0, 0.0,
				0.0, 1.0, 0.0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, l_ambient);
		glLightfv(GL_LIGHT0, GL_AMBIENT, l_ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, l_diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, l_specular);

		glLightfv(GL_LIGHT1, GL_POSITION, light_dir);

		glMaterialfv(GL_FRONT, GL_AMBIENT,  m_sphere[0]);
		glMaterialfv(GL_FRONT, GL_DIFFUSE,  m_sphere[1]);
		glMaterialfv(GL_FRONT, GL_SPECULAR, m_sphere[2]);
		glMaterialfv(GL_FRONT, GL_EMISSION, m_sphere[3]);
		glMaterialf(GL_FRONT, GL_SHININESS, shininess_sphere);

		glPushMatrix();
			glTranslatef(0.0, 0.0, movement_z+=step);
			glPushMatrix();
				glRotatef((GLfloat)(x+=4 % 360), 1.0, 0.3, 0.0);
				glTranslatef(0.0, 0.0, -1.0);
				glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
				draw_sphere(3, 0.1);
			glPopMatrix();
	
			glPushMatrix();
				glRotatef((GLfloat)(x2++ % 360), 0.0, 1.0, 0.0);
				glTranslatef(0.0, 0.0, 1.0);
				draw_sphere(3, 0.4);
			glPopMatrix();
		glPopMatrix();

		if (movement_z > 5 || movement_z < 0)
			step = -step;

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

		aspect_ratio = ((GLfloat)w)/((GLfloat)h);

		glViewport(0, 0, w, h);
		//gluPerspective(90.0, ((GLfloat)w)/((GLfloat)h), 0.0, 150.0);
	}
	return TRUE;
}

