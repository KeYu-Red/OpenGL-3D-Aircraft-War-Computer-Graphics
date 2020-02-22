#include "SpaceShip.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

float SpaceShip::postureAngel = 0.0f;
SpaceShip::SpaceShip() : hJetOffset(0.15f), vJetOffset(0.14f), fbJetOffset(-0.3f),
BaseOBJ(ObjectionType::spaceship,
(glm::vec3)(0.0),
glm::vec3(0.0, 0.0, 1.0),
3.0,
SHIP_SPEED)
, Model("objects/Arc170.obj",
	"objects/Arc170_blinn1.png",
	"objects/Arc170.mtl"),
	DEFAULT_DIRECTION_FRONT(glm::normalize(glm::vec3(0.0, 0.0, 1.0)))

{
	subTexture = new Texture2D("objects/Arc170_blinn1_reverse.png", true);
	TextureSwitch = false;
	type = ObjectionType::spaceship;
	glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
	DEFAULT_DIRECTION_RIGHT = glm::normalize(glm::cross(DEFAULT_DIRECTION_FRONT, worldUp));
	DEFAULT_DIRECTION_UP = glm::normalize(glm::cross(DEFAULT_DIRECTION_RIGHT,DEFAULT_DIRECTION_FRONT));
	extents = glm::vec3(1.0, 1.0, 1.0)* glm::vec3(1.5,0.6,0.4)*glm::vec3(0.5, 0.5, 0.5);
	WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	Yaw = SHIP_YAW;
	Pitch = SHIP_PITCH;
	MouseSensitivity = SHIP_SENSITIVITY;
	update();
}
void SpaceShip::update()
{
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

	this->setDirection(glm::normalize(front));
	// Also re-calculate the Right and Up vector
	this->directionRight = glm::normalize(glm::cross(this->direction, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	this->directionUp = glm::normalize(glm::cross(this->directionRight, this->direction));
	this->DEFAULT_VIEW = (-3.5f*this->getDirection()) + 0.7f*this->directionUp;
	this->PILOT_VIEW = (0.16f*this->getDirection()) + 0.23f*this->directionUp;
	this->OVERLOOK_VIEW = 20.0f*this->directionUp + (-0.3f*this->getDirection());
	this->OUTSEAT_VIEW = 0.38f*this->directionUp+(-0.58f*this->getDirection());
}
void SpaceShip::update(glm::vec3 pos, glm::vec3 dir)
{
	glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
	this->directionRight = glm::normalize(glm::cross(this->getDirection(), worldUp));
	this->directionUp = glm::normalize(glm::cross(directionRight, this->getDirection()));
	this->setDirection(dir);
	this->setPosition(pos);
}
void SpaceShip::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	float velocity = SHIP_SPEED * deltaTime;
	if (direction == FORWARD)
		this->addPosition(this->direction * velocity);
	if (direction == BACKWARD)
		this->addPosition(this->direction * (-velocity));
	if (direction == LEFT)
		this->addPosition(this->directionRight * (-velocity));
	if (direction == RIGHT)
		this->addPosition(this->directionRight * velocity);
	if (direction == UP)
		this->addPosition(this->directionUp * velocity);
	if (direction == DOWN)
		this->addPosition(this->directionUp * (-velocity));
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void SpaceShip::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	Yaw += xoffset;
	Pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}
	// Update Front, Right and Up Vectors using the updated Euler angles
	update();
}
void SpaceShip::Draw(Shader shader, bool& posture, float dt)
{
	shader.use();
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 stepT = glm::mat4(1.0f);
	glm::mat4 stepR = glm::mat4(1.0f);

	glm::vec3 right(0.0f);
	model = glm::scale(model, glm::vec3(0.001f, 0.001f, 0.001f));

	// before drawing a object, you must tell the shader its material and texture
	shader.setMaterial(this->getMaterial());
	if (!TextureSwitch)
		shader.setTexture(this->getTexture());
	else
		shader.setTexture(subTexture->ID);
	float cosFangel = glm::dot(this->getDirection(), DEFAULT_DIRECTION_FRONT);
	glm::vec3 axisFroll = glm::normalize(glm::cross(DEFAULT_DIRECTION_FRONT, this->getDirection()));
	stepT = glm::rotate(model, glm::acos(cosFangel), axisFroll);
	right = glm::normalize(glm::vec3(stepT * glm::vec4(DEFAULT_DIRECTION_RIGHT, 1.0f)));
	float cosRangel = glm::dot(directionRight, right);
	glm::vec3 axisRroll = glm::normalize(glm::cross(right, directionRight));
	/*if (posture) {
		model = glm::rotate(model, glm::radians(postureAngel), this->getDirection());
		stepR = glm::rotate(model, glm::radians(postureAngel), this->getDirection());
		directionRight = glm::normalize(glm::vec3(stepR * glm::vec4(directionRight, 1.0f)));
		directionUp = glm::normalize(glm::vec3(stepR * glm::vec4(directionUp, 1.0f)));
		postureAngel += 200.0f*(2.0f + glm::sin(postureAngel / 2.0f))*dt;
		if (postureAngel >= 360.0f) {
			postureAngel = 0.0f;
			posture = false;
		}
	}*/
	model = glm::rotate(model, glm::acos(cosRangel), axisRroll);
	model = glm::rotate(model, glm::acos(cosFangel), axisFroll);
	//float cosUangel = glm::dot(camera.Up, DEFAULT_DIRECTION_UP);
	//glm::vec3 axisUroll = glm::normalize(glm::cross(camera.Up, DEFAULT_DIRECTION_UP));
	//model = glm::rotate(model, glm::acos(cosUangel), axisUroll);
	shader.setMat4("model", model);
	shader.setVec3("offSet", this->getPosition());
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, triFaces * 3);
}


void SpaceShip::Draw(Shader shader, bool& posture, float dt, GLuint &depthMap)
{
	shader.use();
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 stepT = glm::mat4(1.0f);
	glm::mat4 stepR = glm::mat4(1.0f);
	
	glm::vec3 right(0.0f);
	model = glm::scale(model, glm::vec3(0.001f, 0.001f, 0.001f));

	// before drawing a object, you must tell the shader its material and texture
	shader.setMaterial(this->getMaterial());
	if (!TextureSwitch)
		shader.setTexture(this->getTexture());
	else
		shader.setTexture(subTexture->ID);
	// set the shadow texture
	shader.setInt("shadowMap", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	float cosFangel = glm::dot(this->getDirection(), DEFAULT_DIRECTION_FRONT);
	glm::vec3 axisFroll = glm::normalize(glm::cross(DEFAULT_DIRECTION_FRONT,this->getDirection()));
	stepT = glm::rotate(model, glm::acos(cosFangel), axisFroll);
	right = glm::normalize(glm::vec3(stepT * glm::vec4(DEFAULT_DIRECTION_RIGHT, 1.0f)));
	float cosRangel = glm::dot(directionRight, right);
	glm::vec3 axisRroll = glm::normalize(glm::cross(right, directionRight));
	if (posture) {
		model = glm::rotate(model, glm::radians(postureAngel), this->getDirection());
		stepR = glm::rotate(model, glm::radians(postureAngel), this->getDirection());
		directionRight = glm::normalize(glm::vec3(stepR * glm::vec4(directionRight, 1.0f)));
		directionUp = glm::normalize(glm::vec3(stepR * glm::vec4(directionUp, 1.0f)));
		postureAngel += 200.0f*(2.0f+glm::sin(postureAngel/2.0f))*dt;
		if (postureAngel >= 360.0f) {
			postureAngel = 0.0f;
			posture = false;
		}
	}
	model = glm::rotate(model, glm::acos(cosRangel), axisRroll);
	model = glm::rotate(model, glm::acos(cosFangel), axisFroll);
	//float cosUangel = glm::dot(camera.Up, DEFAULT_DIRECTION_UP);
	//glm::vec3 axisUroll = glm::normalize(glm::cross(camera.Up, DEFAULT_DIRECTION_UP));
	//model = glm::rotate(model, glm::acos(cosUangel), axisUroll);
	shader.setMat4("model", model);
	shader.setVec3("offSet",this->getPosition());
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, triFaces * 3);
}

SpaceShip::~SpaceShip()
{
}

ObjectionType SpaceShip::getType()
{
	return type;
}

