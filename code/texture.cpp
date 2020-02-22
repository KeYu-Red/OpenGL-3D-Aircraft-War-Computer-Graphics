#include <iostream>
#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture2D::Texture2D(char const * path, bool gammaCorrection)
	: Width(0), Height(0), Internal_Format(GL_RGB), Image_Format(GL_RGB), 
	Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT), Filter_Min(GL_LINEAR_MIPMAP_LINEAR), Filter_Max(GL_LINEAR),
	path(path),gammaCorrection(gammaCorrection)
{
	glGenTextures(1, &this->ID);
	this->Generate();
}

void Texture2D::Generate()
{
	stbi_set_flip_vertically_on_load(true);
	// Create Texture
	unsigned char *data = stbi_load(path, &Width, &Height, &nrComponents, 0);
	if (data)
	{
		if (nrComponents == 1)
		{
			Internal_Format = Image_Format = GL_RED;
		}
		else if (nrComponents == 3)
		{
			Internal_Format = gammaCorrection ? GL_SRGB : GL_RGB;
			Image_Format = GL_RGB;
		}
		else if (nrComponents == 4)
		{
			Internal_Format = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
			Image_Format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, this->ID);
		glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, this->Width, this->Height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}
	// Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_set_flip_vertically_on_load(false);
}

void Texture2D::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, this->ID);
}