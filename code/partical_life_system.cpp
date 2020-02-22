#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "partical_life.h"
#include "texture.h"

const float Bullet::DEFAULT_LIFE = 10.0f;
const float Bullet::DEFAULT_SPEED = 30.5f;
const float FireParticle::DEFAULT_LIFE = 0.38f;
const float FireParticle::DEFAULT_SPEED = 0.45f;
/* 
	next part is the function of ParticleGenerator Class
*/

ParticleGenerator::ParticleGenerator(Shader shader, Texture2D& texture, GLuint amount)
	: shader(shader), texture(texture), amount(amount)
{
	this->init();
}

void ParticleGenerator::Update(GLfloat dt, BaseOBJ &object, GLuint newParticles, glm::vec3 offset)
{
	//random effect
	float offx = 0.06f*((float)(rand() % 100 - 50) / 100.0f);
	float offz = 0.06f*((float)(rand() % 100 - 50) / 100.0f);
	float offy = 0.06f*((float)(rand() % 100 - 50) / 100.0f);
	offset = offset + glm::vec3(offx, offy, offz);
	// Add new particles
	for (GLuint i = 0; i < newParticles; ++i)
	{
		int unusedParticle = this->firstUnusedParticle();
		this->respawnParticle(this->particles[unusedParticle], object, offset);
	}
	// Update all particles
	for (GLuint i = 0; i < this->amount; ++i)
	{
		FireParticle &p = this->particles[i];
		float coff = (float)(rand() % 100) / 100.f;
		float cofff = coff > 0.6f ? coff : 1.0f;
		p.life -= coff*dt; // reduce life
		if (p.life > 0.0f)
		{	// particle is alive, thus update
			p.addPosition(p.getDirection()*p.speed*dt);
			p.addColorR(-dt * 50.0f);
			p.addColorG(-dt * 20.0f);
			p.addColorA(-dt * 11.0f);
		}
	}
}

// Render all particles
void ParticleGenerator::Draw()
{
	// Use additive blending to give it a 'glow' effect
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	this->shader.use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.002f, 0.002f, 0.002f));
	this->shader.setMat4("model", model);
	for (FireParticle particle : this->particles)
	{
		if (particle.life > 0.0f)
		{
			this->shader.setFloat("scale", particle.getScale());
			this->shader.setVec3("offset", particle.getPosition());
			this->shader.setVec4("color", particle.getColor());
			glActiveTexture(GL_TEXTURE0);
			this->texture.Bind();
			glBindVertexArray(this->VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}
	}
	// Don't forget to reset to default blending mode
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::init()
{
	// Set up mesh and attribute properties
	GLuint VBO;
	GLfloat particle_quad[] = {
			-1.0f, -1.0f, -1.0f,  0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f, 0.0f  // bottom-left   
	};
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(this->VAO);
	// Fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
	// Set mesh attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)3);
	glBindVertexArray(0);

	// Create this->amount default particle instances
	for (int i = 0; i < this->amount; i++) {
		this->particles.push_back(FireParticle());
	}
}

// Stores the index of the last particle used (for quick access to next dead particle)
GLuint lastUsedParticle = 0;
GLuint ParticleGenerator::firstUnusedParticle()
{
	// First search from last used particle, this will usually return almost instantly
	for (GLuint i = lastUsedParticle; i < this->amount; ++i) {
		if (this->particles[i].life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// Otherwise, do a linear search
	for (GLuint i = 0; i < lastUsedParticle; ++i) {
		if (this->particles[i].life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
	lastUsedParticle = 0;
	return 0;
}

void ParticleGenerator::respawnParticle(FireParticle &particle, BaseOBJ &object, glm::vec3 offset)
{
	particle.setPosition(object.getPosition()+offset);
	float diff = 5.0f*(float)(rand() % 100-50) / 100.0f;
	particle.setColor(glm::vec4(15.0f+diff, 15.0f+diff, 15.0f+diff, 15.0f));
	particle.setScale(1.0f);
	particle.life = FireParticle::DEFAULT_LIFE;
	particle.setDirection(-object.getDirection());
}

/*
	******************************************************
	******************************************************
	******************************************************
	******************************************************
	next part is the function of Gun Class
	******************************************************
	******************************************************
	******************************************************
	******************************************************
	******************************************************
*/
Gun::Gun(Shader shader, Texture2D& texture)
	: shader(shader), texture(texture), size(0)
{
	this->init();
}

void Gun::Update(GLfloat dt)
{
	// Update all particles
	for (int i=0;i<this->bullets.size();i++)
	{
		Bullet &p = this->bullets[i];
		p.life -= 0.6f * dt; // reduce life
		if (p.life > 0.0f)
		{	// particle is alive, thus update
			p.addPosition(p.getDirection()*p.speed*dt);
		}
		else {
			this->bullets.erase(this->bullets.begin()+i);
			i--;
			size--;
		}
		p.update(p.getPosition(), p.getDirection());
	}
}

// Render all particles
void Gun::Draw()
{
	// Use additive blending to give it a 'glow' effect
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	this->shader.use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
	this->shader.setMat4("model", model);
	for (Bullet bt : this->bullets)
	{
		if (bt.life > 0.0f)
		{
			this->shader.setFloat("scale", bt.getScale());
			this->shader.setVec3("offset", bt.getPosition());
			this->shader.setVec4("color", bt.getColor());
			glActiveTexture(GL_TEXTURE0);
			this->texture.Bind();
			glBindVertexArray(this->VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}
	}
	// Don't forget to reset to default blending mode
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Gun::init()
{
	// Set up mesh and attribute properties
	GLuint VBO;
	GLfloat particle_quad[] = {
			-1.0f, -1.0f, -1.0f,  0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f, 0.0f  // bottom-left   
	};
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(this->VAO);
	// Fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
	// Set mesh attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)3);
	glBindVertexArray(0);
}

void Gun::addBullet(BaseOBJ &object, glm::vec3 offset)
{
	Bullet bt = Bullet();
	//
	bt.extents = glm::vec3(1.0, 1.0, 1.0)*glm::vec3(0.02, 0.02, 0.02);
	//
	bt.setDirection(object.getDirection());
	bt.setPosition(object.getPosition() + offset);
	this->bullets.push_back(bt);
	size++;
}

vector<BaseOBJ*> ResourceManager::recycleBin;