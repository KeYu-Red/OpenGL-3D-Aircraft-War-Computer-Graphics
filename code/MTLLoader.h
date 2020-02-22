#ifndef MTLLOADER_H
#define MTLLOADER_H
#define _CRT_SECURE_NO_WARNINGS

#include <vector>
#include <iostream>
#include <glm/glm.hpp>
using namespace std;

typedef struct aMaterial {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
}Material;

bool loadMTL(const char * path,Material & out_material);


#endif