#include "Model.h"
#include "stb_image.h"


Model::Model(const char* objPath, const char* texturePath, const char * mtlPath)
{
	loadModel(objPath);
	loadTexture(texturePath,true);
	loadMTL(mtlPath,material);
}

Model::Model(const char* objPath, const char* texturePath, Material& material)
{
	loadModel(objPath);
	loadTexture(texturePath,true);
	this->material.ambient = material.ambient;
	this->material.diffuse = material.diffuse;
	this->material.specular = material.specular;
	this->material.shininess = material.shininess;
}

void Model::setMaterial(Material& material)
{
	this->material.ambient = material.ambient;
	this->material.diffuse = material.diffuse;
	this->material.specular = material.specular;
	this->material.shininess = material.shininess;
}

void Model::loadModel(const char * path)
{
	std::vector<glm::vec3> v_vertices;
	std::vector<glm::vec2> v_uvs;
	std::vector<glm::vec3> v_normals; // Won't be used at the moment.
	loadOBJ(path, v_vertices, v_uvs, v_normals, &triFaces);

	v_size = v_vertices.size();
	uv_size = v_uvs.size();
	n_size = v_normals.size();

	vertices = new Point[v_size];
	uvs = new UV[uv_size];
	normals = new Normal[n_size];
	for (int i = 0; i < v_size; i++)
	{
		vertices[i].x = v_vertices[i].x;
		vertices[i].y = v_vertices[i].y;
		vertices[i].z = v_vertices[i].z;
		//cout << out_vertices[i].x << " " << out_vertices[i].y << " " << out_vertices[i].z << endl;
	}
	for (int i = 0; i < uv_size; i++)
	{
		uvs[i].u = v_uvs[i].x;
		uvs[i].v = v_uvs[i].y;
	}
	for (int i = 0; i < n_size; i++)
	{
		normals[i].x = v_normals[i].x;
		normals[i].y = v_normals[i].y;
		normals[i].z = v_normals[i].z;
	}

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &v_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, v_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Point)*v_size, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);
	//cout << vertices << " "<< &vertices[0].x << endl;

	glGenBuffers(1, &n_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, n_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Normal)*n_size, normals, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
	//cout << normals << " " << &normals[0].x << endl;

	glGenBuffers(1, &uv_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, uv_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(UV)*uv_size, uvs, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);
}



void Model::loadTexture(const char * path, bool gammaCorrection)
{
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum internalFormat;
		GLenum dataFormat;
		if (nrComponents == 1)
		{
			internalFormat = dataFormat = GL_RED;
		}
		else if (nrComponents == 3)
		{
			internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
			dataFormat = GL_RGB;
		}
		else if (nrComponents == 4)
		{
			internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
			dataFormat = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}
	glBindTexture(GL_TEXTURE_2D,0);
}

Model::~Model()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &v_VBO);
	glDeleteBuffers(1, &uv_VBO);
	glDeleteBuffers(1, &n_VBO);
	delete[] vertices;
	delete[] uvs;
	delete[] normals;

}

GLuint Model::getTexture()
{
	return textureID;
}
Material Model::getMaterial()
{
	return material;
}
