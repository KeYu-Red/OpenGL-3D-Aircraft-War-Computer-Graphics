#ifndef _PARTICAL_LIFE_
#define _PARTICAL_LIFE_
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "texture.h"
#include "shader.h"
#include "Camera.h"

enum class ObjectionType {
	stone,
	spaceship,
	enemy,
	bullet,
	fireParticle
};
/* Bas abstract class which needs extending*/
class BaseOBJ
{
protected:
	ObjectionType type;/* type of this obj*/
	glm::vec3 position;/* the coordinate of this obj*/
	glm::vec3 direction;/* the vector of this obj, which will be normalized in this class*/
public:
	glm::vec3 extents;
	glm::vec3 directionRight;
	glm::vec3 directionUp;
	float life;/* how long will this be*/
	float speed;/* adjust obj speed individually, other wise it would be tedious*/
	bool isDead;
	
public:
	void update(glm::vec3 pos, glm::vec3 dir)
	{
		glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
		this->directionRight = glm::normalize(glm::cross(this->getDirection(), worldUp));
		this->directionUp = glm::normalize(glm::cross(directionRight, this->getDirection()));
		this->setDirection(dir);
		this->setPosition(pos);
	}


public:
	BaseOBJ(ObjectionType type, glm::vec3 pos, glm::vec3 dir, float life, float speed) {
		this->type = type;
		this->position = pos;
		this->direction = glm::normalize(dir);
		directionRight = glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)));
		directionUp = glm::normalize(glm::cross(directionRight, direction));
		this->life = life;
		this->speed = speed;
		this->isDead = false;
	}
	virtual ~BaseOBJ(){}
	virtual ObjectionType getType() = 0;
	/* operation on postion */
	glm::vec3 getPosition() const {
		return position;
	}
	void setPosition(glm::vec3 pos) {
		this->position = pos;
	}
	void setPosition(float px, float py, float pz) {
		this->position.x = px;
		this->position.y = py;
		this->position.z = pz;
	}
	void addPosition(float dx, float dy, float dz) {
		this->position.x += dx;
		this->position.y += dy;
		this->position.z += dz;
	}
	void addPosition(glm::vec3 increment) {
		this->position = this->position + increment;
	}
	/* operation on direction */
	glm::vec3 getDirection() const {
		return direction;
	}
	void setDirection(glm::vec3 dir) {
		this->direction = glm::normalize(dir);
	}
	void setDirection(float px, float py, float pz) {
		this->direction.x = px;
		this->direction.y = py;
		this->direction.z = pz;
		this->direction = glm::normalize(this->direction);
	}
	// add direction by rectangular
	void addDirection(float dx, float dy, float dz) {
		this->direction.x += dx;
		this->direction.y += dy;
		this->direction.z += dz;
		this->direction = glm::normalize(this->direction);
	}
	void addDirection(glm::vec3 increment) {
		this->direction = this->direction + increment;
		this->direction = glm::normalize(this->direction);
	}
	// add direction by angel
	void rotateDirection(float angelX, float angelY, float angelZ) {
		glm::mat4 rotate = glm::mat4(1.0f);
		if (angelX != 0.0f) {
			rotate = glm::rotate(rotate, glm::radians(angelX), glm::vec3(1.0f, 0.0f, 0.0f));
			this->direction = (glm::mat3)rotate*direction;
		}
		if (angelY != 0.0f) {
			rotate = glm::mat4(1.0f);
			rotate = glm::rotate(rotate, glm::radians(angelY), glm::vec3(0.0f, 1.0f, 0.0f));
			this->direction = (glm::mat3)rotate*direction;
		}
		if (angelZ != 0.0f) {
			rotate = glm::mat4(1.0f);
			rotate = glm::rotate(rotate, glm::radians(angelZ), glm::vec3(0.0f, 0.0f, 1.0f));
			this->direction = (glm::mat3)rotate*direction;
		}
		this->direction = glm::normalize(this->direction);
	}
	void setSpeed(float speed) {
		this->speed = speed;
	}
};
class FireParticle : public BaseOBJ
{
private:
	glm::vec4 color;
	float scale;
public:
	const static float DEFAULT_LIFE;
	const static float DEFAULT_SPEED;
public:
	FireParticle(glm::vec3 pos, glm::vec3 dir, float life, float speed, glm::vec4 color) :BaseOBJ(ObjectionType::fireParticle,pos, dir, life, speed) {
		this->color = color;
		scale = 1.0f;
	}
	FireParticle() :BaseOBJ(ObjectionType::fireParticle, glm::vec3(0.0f), glm::vec3(0.0f,1.0f,0.0f), DEFAULT_LIFE, DEFAULT_SPEED) {
		color = glm::vec4(1.0f,1.0f, 0.0f, 1.0f);
		scale = 1.0f;
	}
	virtual ObjectionType getType() {
		return this->type;
	}
	void setScale(float scale) {
		this->scale = scale;
	}
	float getScale() {
		return this->scale;
	}
	glm::vec4 getColor() {
		return this->color;
	}
	void setColor(glm::vec4 color) {
		this->color = color;
		if (this->color.r < 0.0f) this->color.r = 0.0f;
		//else if (this->color.r > 1.0f) this->color.r = 1.0f;
		if (this->color.g < 0.0f) this->color.g = 0.0f;
		//else if (this->color.g > 1.0f) this->color.g = 1.0f;
		if (this->color.b < 0.0f) this->color.b = 0.0f;
		//else if (this->color.b > 1.0f) this->color.b = 1.0f;
		if (this->color.a < 0.0f) this->color.a = 0.0f;
		//else if (this->color.a > 1.0f) this->color.a = 1.0f;
	}
	void setColorR(float value) {
		if(value>=0.0f) this->color.r = value;
	}
	void addColorR(float dv) {
		this->color.r += dv;
		if (this->color.r < 0.0f) this->color.r = 0.0f;
		//else if (this->color.r > 1.0f) this->color.r = 1.0f;
	}
	void setColorG(float value) {
		if (value >= 0.0f) this->color.g = value;
	}
	void addColorG(float dv) {
		this->color.g += dv;
		if (this->color.g < 0.0f) this->color.g = 0.0f;
		//else if (this->color.g > 1.0f) this->color.g = 1.0f;
	}
	void setColorB(float value) {
		if (value >= 0.0f) this->color.b = value;
	}
	void addColorB(float dv) {
		this->color.b += dv;
		if (this->color.b < 0.0f) this->color.b = 0.0f;
		//else if (this->color.b > 1.0f) this->color.b = 1.0f;
	}
	void setColorA(float value) {
		if (value >= 0.0f) this->color.a = value;
	}
	void addColorA(float dv) {
		this->color.a += dv;
		if (this->color.a < 0.0f) this->color.a = 0.0f;
		//else if (this->color.a > 1.0f) this->color.a = 1.0f;
	}
};
/* 在做完fire的部分后尝试泛型解决bullet */
class ParticleGenerator
{
public:
	// Constructor
	ParticleGenerator(Shader shader, Texture2D& texture, GLuint amount);
	// Update all particles
	void Update(GLfloat dt, BaseOBJ &object, GLuint newParticles, glm::vec3 offset = glm::vec3(0.0f));
	// Render all particles
	void Draw();
private:
	// State
	std::vector<FireParticle> particles;
	GLuint amount;
	// Render state
	Shader shader;
	//Texture2D texture;
	Texture2D texture;
	GLuint VAO;
	// Initializes buffer and vertex attributes
	void init();
	// Returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
	GLuint firstUnusedParticle();
	// Respawns particle
	void respawnParticle(FireParticle &particle, BaseOBJ &object, glm::vec3 offset = glm::vec3(0.0f));
};

class Bullet : public BaseOBJ
{
private:
	glm::vec4 color;
	float scale;
public:
public:
	const static float DEFAULT_LIFE;
	const static float DEFAULT_SPEED;
public:
	Bullet(glm::vec3 pos, glm::vec3 dir, float life, float speed, glm::vec4 color) :BaseOBJ(ObjectionType::bullet, pos, dir, life, speed) {
		this->color = color;
		scale = 1.0f;
		extents = scale * glm::vec3(0.02, 0.02, 0.02)*glm::vec3(0.5,0.5,0.5);
	}
	Bullet() :BaseOBJ(ObjectionType::bullet, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), DEFAULT_LIFE, DEFAULT_SPEED) {
		color = glm::vec4(15.0f, 15.0f, 6.0f, 1.0f);
		scale = 1.0f;
		extents = scale * glm::vec3(0.02, 0.02, 0.02)*glm::vec3(0.5, 0.5, 0.5);
	}
	virtual ObjectionType getType() {
		return this->type;
	}
	float getScale() {
		return this->scale;
	}
	glm::vec4 getColor() {
		return this->color;
	}
};

class Gun
{
public:
	// Constructor
	Gun(Shader shader, Texture2D& texture);
	// Update all particles
	void Update(GLfloat dt);
	std::vector<Bullet>& getBullet() {
		return bullets;
	}
	// Render all particles
	void Draw();
	void addBullet(BaseOBJ &object, glm::vec3 offset = glm::vec3(0.0f));
private:
	// State
	std::vector<Bullet> bullets;
	// Render state
	Shader shader;
	//Texture2D texture;
	Texture2D texture;
	GLuint VAO;
	int size;
	// Initializes buffer and vertex attributes
	void init();
	// Respawns particle
};

class ResourceManager
{
public:
	static vector<BaseOBJ*> recycleBin;
public:
	static void controlLife(BaseOBJ* &o1, BaseOBJ* &o2) {
		switch (o1->getType()) {
		case ObjectionType::stone:
			o1->life = 0.0f;
			recycleBin.push_back(o1);
			break;
		case ObjectionType::bullet:
			o1->life = 0.0f;
			break;
		case ObjectionType::spaceship:
			o1->life -= 1.0f;
			break;
		case ObjectionType::enemy:
			o1->life -= 1.0f;
			break;
		default:
			break;
		}
		switch (o2->getType()) {
		case ObjectionType::stone:
			o2->life = 0.0f;
			recycleBin.push_back(o1);
			break;
		case ObjectionType::bullet:
			o2->life = 0.0f;
			break;
		case ObjectionType::spaceship:
			o2->life -= 1.0f;
			break;
		case ObjectionType::enemy:
			o2->life -= 1.0f;
			break;
		default:
			break;
		}
		//freshRecycleBin();
	}
	static void freshRecycleBin() {
		for (int i = 0; i < recycleBin.size();i++) {
			if (recycleBin[i]!=NULL && recycleBin[i]->isDead) {
				delete recycleBin[i];
				recycleBin[i] = NULL;
				recycleBin.erase(recycleBin.begin() + i);
			}
		}
	}
	static void freshRecycleBin(BaseOBJ* obj) {
		delete obj;
		obj = NULL;
	}
};
#endif
