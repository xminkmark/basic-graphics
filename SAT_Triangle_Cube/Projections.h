#pragma once

#include <algorithm>

/**
* Structures and classes for computing projections of triangles
*	onto separating axes during scan conversion 
* (I rewritten algorithm Tao ju wrote [2004. SIGGRAPH. Robust Repair of Polygonal Models])
*
* @email xminkmark@outlook.com
*/

const int vertmap[8][3] = { { 0,0,0 },{ 0,0,1 },{ 0,1,0 },{ 0,1,1 },{ 1,0,0 },{ 1,0,1 },{ 1,1,0 },{ 1,1,1 } };

/*
* Class for projections of Cube and Triangle vertices on the separating axes
* Number of axes is 13, as follows:
*	1 triangle face normal
*	3 cube face normals
*	9 pair-wise cross-products of the 3 primal edges of the cube and 3 edges of the triangle
*/
class Projections {
	
// projections of the Cube or Triangle (min and max projection)
public:
	int cubeProj[13][2];
	int trigProj[13][2];

public:
	
	Projections() = default;
	Projections(int cube[2][3], int trig[3][3])
	{
		int axes[13][3] = {0};

		// 3 Cube face normals (same as axes of x, y, z)
		for (int i = 0; i < 3; i++) axes[i][i] = 1;

		// 1 triangle face normal
		int trigEdge[3][3];
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				trigEdge[i][j] = trig[(i + 1) % 3][j] - trig[i][j];
			}
		}
		crossProduct(trigEdge[0], trigEdge[1], axes[3]);

		// 9 pair-wise
		int cnt = 4;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				crossProduct(axes[i], trigEdge[j], axes[cnt++]);
			}
		}

		// Generate Projections
		int cubeEdge[3][3] = {0};
		for (int i = 0; i < 3; i++) {
			cubeEdge[i][i] = cube[1][i] - cube[0][i];
		}
		// generate Cube Projections
		for (int k = 0; k < 13; k++) 
		{
			// Origin of the cube
			int originProj = scalarProduct(cube[0], axes[k]);
			
			// 3 direction vectors
			int cubeDirProj[3];
			for (int i = 0; i < 3; i++) {
				cubeDirProj[i] = scalarProduct(cubeEdge[i], axes[k]);
			}

			// Offsets of the two end of Cube projection
			int mini = 0, maxi = 0;
			for (int i = 0; i < 8; i++)
			{
				int proj = 0;
				for (int j = 0; j < 3; j++) {
					proj += vertmap[i][j] * cubeDirProj[j];
				}
				maxi = std::max(maxi, proj);
				mini = std::min(mini, proj);
			}

			cubeProj[k][0] = originProj + mini;
			cubeProj[k][1] = originProj + maxi;
		}
		// generate Triangle Projections
		for (int k = 0; k < 13; k++)
		{
			// 3 edges
			int trigVertProj[3];
			for (int i = 0; i < 3; i++) {
				trigVertProj[i] = scalarProduct(trig[i], axes[k]);
			}

			// Offsets of the two end of Triangle projection
			trigProj[k][0] = trigProj[k][1] = trigVertProj[0];
			for (int i = 1; i < 3; i++) {
				trigProj[k][0] = std::min(trigProj[k][0], trigVertProj[i]);
				trigProj[k][1] = std::max(trigProj[k][1], trigVertProj[i]);
			}
		}
	}

public:
	//Method to test intersection of the triangle and the cube
	bool isIntersecting()
	{
		for (int k = 0; k < 13; k++) {
			if (cubeProj[k][0] > trigProj[k][1] || cubeProj[k][1] < trigProj[k][0])
				return false;
		}
		return true;
	}

private:
	// return Scalar(dot) Product of two vectors 
	template<class T>
	T scalarProduct(T a[3], T b[3]) {
		return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
	}

	// Method to perform Cross Product, res[] is resultant
	template<class T>
	void crossProduct(T a[3], T b[3], T res[3]) {
		res[0] = a[1] * b[2] - a[2] * b[1];
		res[1] = a[2] * b[0] - a[0] * b[2];
		res[2] = a[0] * b[1] - a[1] * b[0];
	}

};




