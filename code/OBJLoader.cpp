#include "OBJLoader.h"
bool loadOBJ(
	const char * path,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals,
	int* out_tri_faces
) {
	cout << "Loading OBJ file " << path << " ... " << endl;

	*out_tri_faces = 0;

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	std::vector<glm::vec3> temp2_vertices;
	std::vector<glm::vec2> temp2_uvs;
	std::vector<glm::vec3> temp2_normals;

	FILE *file = fopen(path, "r");
	if (file == NULL)
	{
		cout << "OBJ File Not Exist!!!" << endl;
		getchar();
		return false;
	}
	while(1)
	{ 
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y; 
			// Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			unsigned long long offset = ftell(file);

			fpos_t position;
			// record temp position
			fgetpos(file, &position);
			std::string vertex1, vertex2, vertex3;
			int vertexIndex[4], uvIndex[4], normalIndex[4];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2], &vertexIndex[3], &uvIndex[3], &normalIndex[3]);
			//cout << matches << endl;
			// 12 - the number of matching variables
			if (matches == 12)
			{
				vertexIndices.push_back(abs(vertexIndex[0]));
				vertexIndices.push_back(abs(vertexIndex[1]));
				vertexIndices.push_back(abs(vertexIndex[2]));
				uvIndices.push_back(abs(uvIndex[0]));
				uvIndices.push_back(abs(uvIndex[1]));
				uvIndices.push_back(abs(uvIndex[2]));
				normalIndices.push_back(abs(normalIndex[0]));
				normalIndices.push_back(abs(normalIndex[1]));
				normalIndices.push_back(abs(normalIndex[2]));

				vertexIndices.push_back(abs(vertexIndex[0]));
				vertexIndices.push_back(abs(vertexIndex[2]));
				vertexIndices.push_back(abs(vertexIndex[3]));
				uvIndices.push_back(abs(uvIndex[0]));
				uvIndices.push_back(abs(uvIndex[2]));
				uvIndices.push_back(abs(uvIndex[3]));
				normalIndices.push_back(abs(normalIndex[0]));
				normalIndices.push_back(abs(normalIndex[2]));
				normalIndices.push_back(abs(normalIndex[3]));
				(*out_tri_faces) += 2;

				//cout << (*out_tri_faces) << endl;

			}
			else if(matches == 9)
			{
				//fseek(file, offset, 0);
				////fsetpos(file, &position);
				//matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				//
				vertexIndices.push_back(abs(vertexIndex[0]));
				vertexIndices.push_back(abs(vertexIndex[1]));
				vertexIndices.push_back(abs(vertexIndex[2]));
				uvIndices.push_back(abs(uvIndex[0]));
				uvIndices.push_back(abs(uvIndex[1]));
				uvIndices.push_back(abs(uvIndex[2]));
				normalIndices.push_back(abs(normalIndex[0]));
				normalIndices.push_back(abs(normalIndex[1]));
				normalIndices.push_back(abs(normalIndex[2]));
				(*out_tri_faces)++;			
				//cout << (*out_tri_faces) << endl;
			
			}
			else
			{
				cout << "File can't be read by our simple parser :-( Try exporting with other options\n";
				fclose(file);
				return false;
			}
			

		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}




	// For each vertex of each triangle
	
	out_vertices.clear();
	out_uvs.clear();
	out_normals.clear();
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);
	}



	fclose(file);
	cout << "Loading OBJ file " << path << " Successfully " << endl;
	return true;
}