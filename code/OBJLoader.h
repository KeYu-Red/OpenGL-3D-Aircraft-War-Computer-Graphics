#ifndef OBJLOADER_H
#define OBJLOADER_H
#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
using namespace std;

typedef struct aPoint {
	float x;
	float y;
	float z;
}Point;
typedef struct aUV {
	float u;
	float v;
}UV;
typedef struct aNormal {
	float x;
	float y;
	float z;
}Normal;

typedef struct aPointData {
	int pointindex;
	int uvindex;
	int normalindex;
}PointData;
bool loadOBJ(
	const char * path,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals,
	int* out_tri_faces
);


#endif