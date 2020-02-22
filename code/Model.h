#ifndef _MODEL_H
#define _MODEL_H





#include "OBJLoader.h"
#include "MTLLoader.h"
#include "partical_life.h"
#include "Shader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


class Model 
{
public:
	Model(const char* objPath,const char* texturePath, const char * mtlPath);
	Model(const char* objPath,const char* texturePath, Material& material);
	~Model();
	void Draw(Shader shader)
	{
		shader.use();
		shader.setVec3("offSet", glm::vec3(0.0f));
		

		shader.setMaterial(this->getMaterial());
		shader.setTexture(this->getTexture());
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, triFaces * 3);
	}
	void Draw(Shader shader,GLuint &depthMap)
	{
		shader.use();
		shader.setVec3("offSet", glm::vec3(0.0f));
		

		shader.setMaterial(this->getMaterial());
		shader.setTexture(this->getTexture());
		// set the shadow texture
		shader.setInt("shadowMap", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, triFaces * 3);
	}
	
	void setMaterial(Material& material);
	GLuint getTexture();
	Material getMaterial();
	


protected:
	void loadModel(const char* path);
	void loadTexture(const char* path, bool gammaCorrection);

	Point* vertices;
	UV* uvs;
	Normal* normals;
	GLuint textureID;
	Material material;

	int v_size;
	int uv_size;
	int n_size;

	int triFaces;
	GLuint v_VBO;
	GLuint uv_VBO;
	GLuint n_VBO;

	GLuint VAO;
};
#endif