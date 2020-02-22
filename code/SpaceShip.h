#ifndef _SPACESHIP_H
#define _SPACESHIP_H

#include "partical_life.h"
#include "Model.h"
#include "texture.h"
class SpaceShip :
	public BaseOBJ,public Model
{
private:
	static float postureAngel;
	Texture2D *subTexture;
public:
	const float hJetOffset;
	const float vJetOffset;
	const float fbJetOffset;
	const glm::vec3 DEFAULT_DIRECTION_FRONT;
	glm::vec3 DEFAULT_DIRECTION_RIGHT;
	glm::vec3 DEFAULT_DIRECTION_UP;
	const float SHIP_YAW = -90.0f;
	const float SHIP_PITCH = 0.0f;
	const float SHIP_SENSITIVITY = 0.1f;
	const float SHIP_SPEED = 1.5f;
	glm::vec3 DEFAULT_VIEW;
	glm::vec3 PILOT_VIEW;
	glm::vec3 OVERLOOK_VIEW;
	glm::vec3 OUTSEAT_VIEW;

	glm::vec3 WorldUp;
	float Yaw;
	float Pitch;
	float MouseSensitivity;
	bool TextureSwitch;
public:
	SpaceShip();
	~SpaceShip();
	void Draw(Shader shader, bool& posture, float dt);
	void Draw(Shader shader, bool& posture, float dt,GLuint &depthMap);
	ObjectionType getType();
	void update();
	void update(glm::vec3 pos, glm::vec3 dir);
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
	void SwitchTexture() {
		TextureSwitch = !TextureSwitch;
	}

};

#endif