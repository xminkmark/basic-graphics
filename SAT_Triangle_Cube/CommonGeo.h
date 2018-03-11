#pragma once

struct Point3i {
	int x, y, z;

	Point3i() = default;
	Point3i(const int x, const int y, const int z) : x(x), y(y), z(z) {}
	Point3i(const int a[3]) : x(a[0]), y(a[1]), z(a[2]) {}
};

struct Triangle3i {
	Point3i vertex[3];

	Triangle3i() = default;
	Triangle3i(const Point3i a, const Point3i b, const Point3i c){
		vertex[0] = a;	vertex[1] = b;	vertex[2] = c;
	}
};

struct Cube3i {	
	Point3i mini, maxi;

	Cube3i() = default;
	Cube3i(const Point3i _mini, const Point3i _maxi): mini(_mini), maxi(_maxi){}
};

