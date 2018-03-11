#include <stdio.h>

#include "Projections.h"
#include "CommonGeo.h"

#include <windows.h>
#include "gl/glut.h"
/**********************************************
*				Main Methods				  *
**********************************************/
void paint_cube(const Point3i &pmin, const Point3i &pmax);
void paint_triangle(const Triangle3i &t, const GLfloat color[3]);

bool output = false;

void test_display()
{
	// read when display
	freopen("data.in", "r", stdin);
	
	int cube[2][3];
	// Input and paint Cube
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 3; j++) {
			scanf("%d", &cube[i][j]);
		}
	}
	paint_cube(Point3i(cube[0]), Point3i(cube[1]));
	
	// Input Triangle
	int T;
	scanf("%d", &T);
	for (int t = 0; t < T; t++) {
		int trig[3][3];
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				scanf("%d", &trig[i][j]);
			}
		}

		// processing intersecting and paint Triangle
		Projections proj(cube, trig);
		if (proj.isIntersecting()) {
			GLfloat color[3] = {0.0, 0.0, 1.0};		// Intersected triangle is blue
			paint_triangle(Triangle3i(Point3i(trig[0]), Point3i(trig[1]), Point3i(trig[2])), color);
			
			if(output == false) printf("%d: Yes\n", t);
		}
		else {
			GLfloat color[3] = { 1.0, 1.0, 1.0 };		// Not intersected triangle is white
			paint_triangle(Triangle3i(Point3i(trig[0]), Point3i(trig[1]), Point3i(trig[2])), color);
			
			if(output == false) printf("%d: No\n", t);
		}
	}

	output = true;
}

/**********************************************
*			Object Painting Methods			  *
**********************************************/

void paint_cube(const Point3i &pmin, const Point3i &pmax)
{
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINE_STRIP);
	glVertex3i(pmin.x, pmin.y, pmin.z);
	glVertex3i(pmax.x, pmin.y, pmin.z);
	glVertex3i(pmax.x, pmax.y, pmin.z);
	glVertex3i(pmin.x, pmax.y, pmin.z);
	glVertex3i(pmin.x, pmax.y, pmax.z);
	glVertex3i(pmin.x, pmin.y, pmax.z);
	glVertex3i(pmin.x, pmin.y, pmin.z);
	glVertex3i(pmin.x, pmax.y, pmin.z);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex3i(pmax.x, pmax.y, pmax.z);
	glVertex3i(pmax.x, pmax.y, pmin.z);
	glVertex3i(pmax.x, pmin.y, pmin.z);
	glVertex3i(pmax.x, pmin.y, pmax.z);
	glVertex3i(pmin.x, pmin.y, pmax.z);
	glVertex3i(pmin.x, pmax.y, pmax.z);
	glVertex3i(pmax.x, pmax.y, pmax.z);
	glVertex3i(pmax.x, pmin.y, pmax.z);
	glEnd();
}

void paint_triangle(const Triangle3i &t, const GLfloat color[3]) 
{
	glColor3f(color[0], color[1], color[2]);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 3; i++)
		glVertex3i(t.vertex[i].x, t.vertex[i].y, t.vertex[i].z);
	glEnd();
}


/**********************************************
*				Common Methods				  *
**********************************************/

int width = 500, height = 500;
double rotationX = 0.0;
double rotationY = 0.0;
double scaling = 0.15;

int lastX, lastY;

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -10.0);
	glScalef(scaling, scaling, scaling);

	glRotatef(rotationX, 1.0, 0.0, 0.0);
	glRotatef(rotationY, 0.0, 1.0, 0.0);
	//glRotatef(rotationZ, 0.0, 0.0, 1.0);

	test_display();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat x = 3.0 * GLfloat(width) / height;
	glOrtho(-x, +x, -3.0, +3.0, 4.0, 15.0);
	glMatrixMode(GL_MODELVIEW);
}

void init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
}

void mouse(int button, int state, int x, int y)
{
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) {
			lastX = x;
			lastY = y;
		}
		break;
	default:
		break;
	}
}

void mouseMove(int x, int y)
{
	// control rotation
	GLfloat dx = GLfloat(x - lastX) / width;
	GLfloat dy = GLfloat(y - lastY) / height;

	rotationX += 180 * dy;
	rotationY += 180 * dx;
	display();

	lastX = x;
	lastY = y;
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("SAT of Triangle and Cube Pro");
	init();
	glutDisplayFunc(display);
	glutMotionFunc(mouseMove);
	glutMouseFunc(mouse);
	glutReshapeFunc(reshape);
	glutMainLoop();

	return 0;
}


