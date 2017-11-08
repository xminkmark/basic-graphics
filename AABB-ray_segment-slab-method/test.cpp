#include <CGAL/Simple_cartesian.h>
#include <algorithm>
#include <vector>
#include <windows.h>
#include "gl/glut.h"

typedef CGAL::Simple_cartesian<double>	Kernel;
typedef Kernel::Point_3			Point_3;
typedef Kernel::Vector_3		Vector_3;
typedef Kernel::Segment_3		Segment_3;
typedef Kernel::Triangle_3		Triangle_3;
typedef Kernel::Direction_3		Direction_3;
typedef Kernel::Ray_3			Ray_3;
typedef CGAL::Bbox_3			BBox;

using std::cout;
using std::endl;
using std::vector;

const double DOUBLE_EPSILON = 1e-16;
const double DOUBLE_INF = 1e10;

int width = 500, height = 500;
double rotationX = 0.0;
double rotationY = 0.0;
double scaling = 0.8;

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

	void test_display();
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

void paint_cube(const Point_3 &pmin, const Point_3 &pmax)
{
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINE_STRIP);
	glVertex3f(pmin.x(), pmin.y(), pmin.z());
	glVertex3f(pmax.x(), pmin.y(), pmin.z());
	glVertex3f(pmax.x(), pmax.y(), pmin.z());
	glVertex3f(pmin.x(), pmax.y(), pmin.z());
	glVertex3f(pmin.x(), pmax.y(), pmax.z());
	glVertex3f(pmin.x(), pmin.y(), pmax.z());
	glVertex3f(pmin.x(), pmin.y(), pmin.z());
	glVertex3f(pmin.x(), pmax.y(), pmin.z());
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex3f(pmax.x(), pmax.y(), pmax.z());
	glVertex3f(pmax.x(), pmax.y(), pmin.z());
	glVertex3f(pmax.x(), pmin.y(), pmin.z());
	glVertex3f(pmax.x(), pmin.y(), pmax.z());
	glVertex3f(pmin.x(), pmin.y(), pmax.z());
	glVertex3f(pmin.x(), pmax.y(), pmax.z());
	glVertex3f(pmax.x(), pmax.y(), pmax.z());
	glVertex3f(pmax.x(), pmin.y(), pmax.z());
	glEnd();
}

//void paint_triangle(const Triangle_3 &t) 
//{
//	glBegin(GL_LINE_LOOP);
//	for (int i = 0; i < 3; i++)
//		glVertex3f(t.vertex(i).x(), t.vertex(i).y(), t.vertex(i).z());
//	glEnd();
//}

void paint_ray(const Ray_3 &ray)
{
	// destination point
	Point_3 dest{ ray.source().x() + 100.0 * ray.direction().dx(), 
		ray.source().y() + 100.0 * ray.direction().dy(),
		ray.source().z() + 100.0 * ray.direction().dz()
	};
	glBegin(GL_LINES);
	glVertex3f(ray.source().x(), ray.source().y(), ray.source().z());
	glVertex3f(dest.x(), dest.y(), dest.z());
	glEnd();
	glColor3f(0.0, 0.0, 1.0);
	glPointSize(5.0);
	glBegin(GL_POINTS);
	glVertex3f(ray.source().x(), ray.source().y(), ray.source().z());
	glEnd();
}

void paint_segment(const Segment_3 &seg)
{
	glBegin(GL_LINES);
	glVertex3f(seg.vertex(0).x(), seg.vertex(0).y(), seg.vertex(0).z());
	glVertex3f(seg.vertex(1).x(), seg.vertex(1).y(), seg.vertex(1).z());
	glEnd();
}

// Ray-AABB Detection Algorithm (Slabs method)
// If this AABB hit by ray, return true
// pmin, pmax: minimum and maximum points of bounding box(AABB)
bool AABB_is_hit_ray(const Ray_3 &ray, const Point_3 &pmin, const Point_3 &pmax)
{
	double vmin[3] = { pmin.x(), pmin.y(), pmin.z() };
	double vmax[3] = { pmax.x(), pmax.y(), pmax.z() };
	double o[3] = { ray.source().x(), ray.source().y(), ray.source().z() };
	double d[3] = { ray.direction().dx(), ray.direction().dy(), ray.direction().dz() };
	double step_near[3], step_far[3];	// the time to enter and quit slabs

	// travel x-y-z
	for (int i = 0; i < 3; i++) {
		if (abs(d[i]) < DOUBLE_EPSILON) {
			// if source of ray is not in slabs
			if (o[i] < vmin[i] || o[i] > vmax[i]) return false;
			else {
				step_near[i] = 0;	step_far[i] = DOUBLE_INF;
			}
		}
		else {
			double inv_d = 1.0 / d[i];
			if (d[i] >= 0) {
				step_near[i] = (vmin[i] - o[i]) * inv_d;
				step_far[i] = (vmax[i] - o[i]) * inv_d;
			}
			else {
				step_near[i] = (vmax[i] - o[i]) * inv_d;
				step_far[i] = (vmin[i] - o[i]) * inv_d;
			}
		}
	}
	double t_max = max(step_near[0], max(step_near[1], step_near[2]));
	if (t_max < 0) return false;	// Not enter Slabs
	double t_min = min(step_far[0], min(step_far[1], step_far[2]));
	// time of maximum for entering < time of minimum for quiting == AABB-ray intersect
	return t_max < t_min;
}

// Based on Ray-AABB Detection Algorithm (Slabs method)
// If this bbox hit by segment, return true
// pmin, pmax: minimum and maximum points of bounding box(AABB)
bool AABB_is_hit_segment(const Segment_3 &s, const Point_3 &pmin, const Point_3 &pmax)
{
	Vector_3 v3 = s.vertex(1) - s.vertex(0);
	double vmin[3] = { pmin.x(), pmin.y(), pmin.z() };
	double vmax[3] = { pmax.x(), pmax.y(), pmax.z() };
	// converting ray
	double o[3] = { s.vertex(0).x(), s.vertex(0).y(), s.vertex(0).z() };
	double d[3] = { v3.x(), v3.y(), v3.z() };
	double step_near[3], step_far[3];	// the time to enter and quit slabs

	// travel x-y-z 
	for (int i = 0; i < 3; i++) {
		if (abs(d[i]) < DOUBLE_EPSILON) {
			// if source of ray is not in slabs
			if (o[i] < vmin[i] || o[i] > vmax[i]) return false;
			else {
				step_near[i] = 0;	step_far[i] = DOUBLE_INF;
			}
		}
		else {
			double inv_d = 1.0 / d[i];
			if (d[i] >= 0) {
				step_near[i] = (vmin[i] - o[i]) * inv_d;
				step_far[i] = (vmax[i] - o[i]) * inv_d;
			}
			else {
				step_near[i] = (vmax[i] - o[i]) * inv_d;
				step_far[i] = (vmin[i] - o[i]) * inv_d;
			}
		}
	}
	double t_max = max(step_near[0], max(step_near[1], step_near[2]));
	if (t_max < 0 || t_max > 1) return false;	// Not enter Slabs (Modified!)
	double t_min = min(step_far[0], min(step_far[1], step_far[2]));
	return t_max < t_min;
}

void test_display()
{
	Point_3 pmin1{ 1, 1, 1 }, pmax1{ 2,2,2 };
	Point_3 pmin2{ -2, -2, -2 }, pmax2{ -1, -1, -1 };

	vector<Ray_3> rays;
	rays.push_back(Ray_3{ Point_3{ 0, 0, 0}, Direction_3{ 0.5, 0.5, 0.5} });
	rays.push_back(Ray_3{ Point_3{ 0, 0.2, 0 }, Direction_3{ 0.5, 0.2, 0.2 } });
	rays.push_back(Ray_3{ Point_3{ 1.7, 1.2, 1.2 }, Direction_3{ 2, 0, 1 } });
	rays.push_back(Ray_3{ Point_3{ 1.5, 3, 2 }, Direction_3{ 0, 1, 1 } });
	paint_cube(pmin1, pmax1);
	for (Ray_3 ray : rays) {
		if (AABB_is_hit_ray(ray, pmin1, pmax1)) {
			glColor3f(0.8, 0.5, 0.5);
			paint_ray(ray);
		}
		else {
			glColor3f(1.0, 1.0, 1.0);
			paint_ray(ray);
		}
	}

	vector<Segment_3> segs;
	segs.push_back(Segment_3{ Point_3{ -0.2, -0.2, -0.2 }, Point_3{ -1, -1, -1} });
	segs.push_back(Segment_3{ Point_3{ -1.5, -1.5, 0 }, Point_3{ -1.5, -1.5, -3 } });
	segs.push_back(Segment_3{ Point_3{ -1.8, -1.5, 0 }, Point_3{ 0, 0, 1 } });
	paint_cube(pmin2, pmax2);
	for (Segment_3 seg : segs) {
		if (AABB_is_hit_segment(seg, pmin2, pmax2)) {
			glColor3f(0.8, 0.5, 0.5);
			paint_segment(seg);
		}
		else {
			glColor3f(1.0, 1.0, 1.0);
			paint_segment(seg);
		}
	}

}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Test Pro");
	init();
	glutDisplayFunc(display);
	glutMotionFunc(mouseMove);
	glutMouseFunc(mouse);
	glutReshapeFunc(reshape);
	glutMainLoop();
	
	return 0;
}