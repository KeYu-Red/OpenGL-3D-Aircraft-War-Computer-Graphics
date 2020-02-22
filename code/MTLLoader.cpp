#include "MTLLoader.h"


bool loadMTL(const char * path, Material & out_material)
{
	cout << "Loading MTL file " << path << " ... " << endl;

	FILE *file = fopen(path, "r");
	if (file == NULL)
	{
		cout << "MTL File Not Exist!!!" << endl;
		getchar();
		return false;
	}
	
	while (1)
	{
		char lineHeader[128];
		// read the first word of the line
		//cout << "test" << endl;
		int res = fscanf(file, "%s", lineHeader);
		//cout << "test" << endl;
		if (res == EOF)
			break;
		//cout << *lineHeader << endl;
		if (strcmp(lineHeader, "Ka") == 0) {
			glm::vec3 temp;
			int matches = fscanf(file, "%f %f %f\n", &temp.x, &temp.y, &temp.z);
			if (matches == 3)
				out_material.ambient = temp;
	
		}
		else if (strcmp(lineHeader, "Kd") == 0) {
			glm::vec3 temp;
			int matches = fscanf(file, "%f %f %f\n", &temp.x, &temp.y, &temp.z);
			if (matches == 3)	
				out_material.diffuse = temp;
		}
		else if (strcmp(lineHeader, "Ks") == 0) {
			glm::vec3 temp;
			int matches = fscanf(file, "%f %f %f\n", &temp.x, &temp.y, &temp.z);
			if (matches == 3)
				out_material.specular = temp;
		}
		else if (strcmp(lineHeader, "Ns") == 0) {
			float temp = 0;
			int matches = fscanf(file, "%f\n", &temp);
			if (matches == 1)
				out_material.shininess = temp;
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}

	fclose(file);
	cout << "Loading OBJ file " << path << " Successfully " << endl;
	return true;
}