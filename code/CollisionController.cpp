#include "CollisionController.h"
#include <iostream>
using namespace std;


CollisionController::CollisionController()
{
	//collisioners = new Collisioner[255];
}


CollisionController::~CollisionController()
{
}

GLboolean CollisionController::checkCollision(BaseOBJ &box0, BaseOBJ &box1)
{
	double distance = glm::distance(box0.getPosition(), box1.getPosition());
	if (distance > 5.0f)
		return false;
	
	glm::vec3 distanceV = box1.getPosition() - box0.getPosition();
	
	//Compute A's basis
	glm::vec3 axis_Ax = box0.directionRight *  glm::vec3(1, 0, 0);
	glm::vec3 axis_Ay = box0.directionUp *  glm::vec3(0, 1, 0);
	glm::vec3 axis_Az = box0.getDirection() *  glm::vec3(0, 0, 1);

	glm::vec3 axis_A[3];
	axis_A[0] = axis_Ax;
	axis_A[1] = axis_Ay;
	axis_A[2] = axis_Az;

	//Compute B's basis
	glm::vec3 axis_Bx = box1.directionRight *  glm::vec3(1, 0, 0);
	glm::vec3 axis_By = box1.directionUp *  glm::vec3(0, 1, 0);
	glm::vec3 axis_Bz = box1.getDirection()  *  glm::vec3(0, 0, 1);

	glm::vec3 axis_B[3];
	axis_B[0] = axis_Bx;
	axis_B[1] = axis_By;
	axis_B[2] = axis_Bz;

	glm::vec3 T = glm::vec3(glm::dot(distanceV, axis_Ax), glm::dot(distanceV, axis_Ay), glm::dot(distanceV, axis_Az));

	float R[3][3];
	float FR[3][3];
	float ra, rb, t;

	for (int i = 0; i < 3; i++)
	{
		for (int k = 0; k < 3; k++)
		{
			R[i][k] = glm::dot(axis_A[i], axis_B[k]);
			FR[i][k] = 1e-6f + abs(R[i][k]);
		}
	}

	// A's basis vectors
	for (int i = 0; i < 3; i++)
	{
		ra = box0.extents[i];
		rb = box1.extents[0] * FR[i][0] + box1.extents[1] * FR[i][1] + box1.extents[2] * FR[i][2];
		t = abs(T[i]);
		if (t > ra + rb) return false;
	}

	// B's basis vectors
	for (int k = 0; k < 3; k++)
	{
		ra = box0.extents[0] * FR[0][k] + box0.extents[1] * FR[1][k] + box0.extents[2] * FR[2][k];
		rb = box1.extents[k];
		t = abs(T[0] * R[0][k] + T[1] * R[1][k] + T[2] * R[2][k]);
		if (t > ra + rb) return false;
	}

	//9 cross products

	//L = A0 x B0
	ra = box0.extents[1] * FR[2][0] + box0.extents[2] * FR[1][0];
	rb = box1.extents[1] * FR[0][2] + box1.extents[2] * FR[0][1];
	t = abs(T[2] * R[1][0] - T[1] * R[2][0]);
	if (t > ra + rb) return false;

	//L = A0 x B1
	ra = box0.extents[1] * FR[2][1] + box0.extents[2] * FR[1][1];
	rb = box1.extents[0] * FR[0][2] + box1.extents[2] * FR[0][0];
	t = abs(T[2] * R[1][1] - T[1] * R[2][1]);
	if (t > ra + rb) return false;

	//L = A0 x B2
	ra = box0.extents[1] * FR[2][2] + box0.extents[2] * FR[1][2];
	rb = box1.extents[0] * FR[0][1] + box1.extents[1] * FR[0][0];
	t = abs(T[2] * R[1][2] - T[1] * R[2][2]);
	if (t > ra + rb) return false;

	//L = A1 x B0
	ra = box0.extents[0] * FR[2][0] + box0.extents[2] * FR[0][0];
	rb = box1.extents[1] * FR[1][2] + box1.extents[2] * FR[1][1];
	t = abs(T[0] * R[2][0] - T[2] * R[0][0]);
	if (t > ra + rb) return false;

	//L = A1 x B1
	ra = box0.extents[0] * FR[2][1] + box0.extents[2] * FR[0][1];
	rb = box1.extents[0] * FR[1][2] + box1.extents[2] * FR[1][0];
	t = abs(T[0] * R[2][1] - T[2] * R[0][1]);
	if (t > ra + rb) return false;

	//L = A1 x B2
	ra = box0.extents[0] * FR[2][2] + box0.extents[2] * FR[0][2];
	rb = box1.extents[0] * FR[1][1] + box1.extents[1] * FR[1][0];
	t = abs(T[0] * R[2][2] - T[2] * R[0][2]);
	if (t > ra + rb) return false;

	//L = A2 x B0
	ra = box0.extents[0] * FR[1][0] + box0.extents[1] * FR[0][0];
	rb = box1.extents[1] * FR[2][2] + box1.extents[2] * FR[2][1];
	t = abs(T[1] * R[0][0] - T[0] * R[1][0]);
	if (t > ra + rb) return false;

	//L = A2 x B1
	ra = box0.extents[0] * FR[1][1] + box0.extents[1] * FR[0][1];
	rb = box1.extents[0] * FR[2][2] + box1.extents[2] * FR[2][0];
	t = abs(T[1] * R[0][1] - T[0] * R[1][1]);
	if (t > ra + rb) return false;

	//L = A2 x B2
	ra = box0.extents[0] * FR[1][2] + box0.extents[1] * FR[0][2];
	rb = box1.extents[0] * FR[2][1] + box1.extents[1] * FR[2][0];
	t = abs(T[1] * R[0][2] - T[0] * R[1][2]);
	if (t > ra + rb) return false;

	return true;
}

