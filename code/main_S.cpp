#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "model.h"

#include <iostream>
#include <cmath>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow * window,int key, int scancode,int action,int mods);
void getFPS();
void drawTable(Shader& ourShader);
void drawModelList(Shader& ourShader, Model& ourmodel);

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int FPS = 0;
float interVal = 0;

bool spaceMode = false;
bool runMode = false;
bool lightColorMode = false;
bool spotMode = false;
//int numberOfModel = 1;

//initialize the light info
glm::vec3 lightPos(0.8f, 0.7f, 0.6f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
glm::vec3 spotPos(0.0f, 0.8f, 0.0f);
glm::vec3 spotDir(0.0f, -1.0f, 0.0f);
glm::vec3 spotColor(1.0f, 1.0f, 1.0f);

//teapot location
float dx = 0.0f;
float dz = 0.0f;
int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader ourShader("4.5.shader.vs", "4.5.shader.fs");
	Shader tableShader("table.vs","table.fs");
	Shader lightShader("light.vs", "light.fs");
	Shader spotShader("spot.vs", "spot.fs");

	// load models
	// -----------
	Model ourModel("Futuristic combat jet/Futuristic combat jet.obj");// gun/KSR - 29 sniper rifle new_obj.obj


	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	float table[] = {
		// positions          // colors       // normalvector coords  
		 0.5f, 0.0f, 0.4f,   1.0f, 0.5f, 0.31f, 0.0f, 1.0f, 0.0f,// surface_0
		 0.5f, 0.0f,-0.4f,   1.0f, 0.5f, 0.31f, 0.0f, 1.0f, 0.0f,// surface_1
		-0.5f, 0.0f,-0.4f,   1.0f, 0.5f, 0.31f, 0.0f, 1.0f, 0.0f,// surface_2
		-0.5f, 0.0f, 0.4f,   1.0f, 0.5f, 0.31f, 0.0f, 1.0f, 0.0f,// surface_3
		 0.5f, -0.1f, 0.4f,  1.0f, 0.5f, 0.31f, 0.0f, -1.0f, 0.0f,// surface_4
		 0.5f, -0.1f,-0.4f,  1.0f, 0.5f, 0.31f, 0.0f, -1.0f, 0.0f,// surface_5
		-0.5f, -0.1f,-0.4f,  1.0f, 0.5f, 0.31f, 0.0f, -1.0f, 0.0f,// surface_6
		-0.5f, -0.1f, 0.4f,  1.0f, 0.5f, 0.31f, 0.0f, -1.0f, 0.0f,// surface_7
		//leg1
		0.45f, -0.1f, 0.35f,   0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,//leg1_8
		0.45f, -0.1f, 0.25f,   0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,//leg1_9
		0.35f, -0.1f, 0.25f,   0.0f, 1.0f, 1.0f, 0.0f, 0.0f,-1.0f,//leg1_10
		0.35f, -0.1f, 0.35f,   0.0f, 1.0f, 1.0f, 0.0f, 0.0f,-1.0f,//leg1_11
		0.45f, -0.4f, 0.35f,   1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,//leg1_12
		0.45f, -0.4f, 0.25f,   1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,//leg1_13
		0.35f, -0.4f, 0.25f,   1.0f, 1.0f, 1.0f, 0.0f, 0.0f,-1.0f,//leg1_14
		0.35f, -0.4f, 0.35f,   1.0f, 1.0f, 1.0f, 0.0f, 0.0f,-1.0f,//leg1_15
		//leg2
		0.45f, -0.1f, -0.35f,   0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,//leg1_16
		0.45f, -0.1f, -0.25f,   0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,//leg1_17
		0.35f, -0.1f, -0.25f,   0.0f, 1.0f, 1.0f, 0.0f, 0.0f,-1.0f,//leg1_18
		0.35f, -0.1f, -0.35f,   0.0f, 1.0f, 1.0f, 0.0f, 0.0f,-1.0f,//leg1_19
		0.45f, -0.4f, -0.35f,   1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,//leg1_20
		0.45f, -0.4f, -0.25f,   1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,//leg1_21
		0.35f, -0.4f, -0.25f,   1.0f, 1.0f, 1.0f, 0.0f, 0.0f,-1.0f,//leg1_22
		0.35f, -0.4f, -0.35f,   1.0f, 1.0f, 1.0f, 0.0f, 0.0f,-1.0f,//leg1_23
		//leg3
		-0.45f, -0.1f, -0.35f,   0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,//leg1_8
		-0.45f, -0.1f, -0.25f,   0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,//leg1_9
		-0.35f, -0.1f, -0.25f,   0.0f, 1.0f, 1.0f, 0.0f, 0.0f,-1.0f,//leg1_10
		-0.35f, -0.1f, -0.35f,   0.0f, 1.0f, 1.0f, 0.0f, 0.0f,-1.0f,//leg1_11
		-0.45f, -0.4f, -0.35f,   1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,//leg1_12
		-0.45f, -0.4f, -0.25f,   1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,//leg1_13
		-0.35f, -0.4f, -0.25f,   1.0f, 1.0f, 1.0f, 0.0f, 0.0f,-1.0f,//leg1_14
		-0.35f, -0.4f, -0.35f,   1.0f, 1.0f, 1.0f, 0.0f, 0.0f,-1.0f,//leg1_15
		//leg4
		-0.45f, -0.1f, 0.35f,   0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,//leg1_16
		-0.45f, -0.1f, 0.25f,   0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,//leg1_17
		-0.35f, -0.1f, 0.25f,   0.0f, 1.0f, 1.0f, 0.0f, 0.0f,-1.0f,//leg1_18
		-0.35f, -0.1f, 0.35f,   0.0f, 1.0f, 1.0f, 0.0f, 0.0f,-1.0f,//leg1_19
		-0.45f, -0.4f, 0.35f,   1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,//leg1_20
		-0.45f, -0.4f, 0.25f,   1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,//leg1_21
		-0.35f, -0.4f, 0.25f,   1.0f, 1.0f, 1.0f, 0.0f, 0.0f,-1.0f,//leg1_22
		-0.35f, -0.4f, 0.35f,   1.0f, 1.0f, 1.0f, 0.0f, 0.0f,-1.0f,//leg1_23
	};
	float light[] = {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
	};
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(table), table, GL_STREAM_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//normal attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unsigned int LightVBO, LightVAO;
	glGenVertexArrays(1, &LightVAO);
	glGenBuffers(1, &LightVBO);

	glBindVertexArray(LightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, LightVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(light), light, GL_STREAM_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//set the layout of model
	/*glm::vec3 translations[6];
	int index = 0;
	for (int zoff = 8; zoff >=-8; zoff -= 16)
	{
		for (int xoff = 10; xoff >= -10; xoff -= 10)
		{
			glm::vec3 translation;
			translation.y = 0.0f;
			translation.x = xoff;
			translation.z = zoff;
			translations[index++] = translation;
		}
	}*/
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		getFPS();

		// input
		// -----
		processInput(window);
		glfwSetKeyCallback(window,key_callback);
		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//create a matrix to rotate......
		//
		// don't forget to enable shader before setting uniforms
		ourShader.use();
		ourShader.setVec3("lightColor", lightColor);
		ourShader.setVec3("viewPos", camera.Position);
		//first we set point light
		ourShader.setVec3("pointLight.position", lightPos);
		ourShader.setVec3("pointLight.ambient", 0.1f, 0.1f, 0.1f);
		ourShader.setVec3("pointLight.diffuse", 0.8f, 0.8f, 0.8f);
		ourShader.setVec3("pointLight.specular", 0.6f, 0.6f, 0.6f);
		ourShader.setFloat("pointLight.constant", 1.0f);
		ourShader.setFloat("pointLight.linear", 0.09);
		ourShader.setFloat("pointLight.quadratic", 0.032);
		//then we set spotlight
		ourShader.setVec3("spotLight.position", spotPos);
		ourShader.setVec3("spotLight.direction", spotDir);
		ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		ourShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		ourShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		ourShader.setFloat("spotLight.constant", 1.0f);
		ourShader.setFloat("spotLight.linear", 0.09);
		ourShader.setFloat("spotLight.quadratic", 0.032);
		ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(30.0f)));
		ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(32.0f)));
		if (spotMode) {
			ourShader.setBool("spotOn", true);
		}
		else {
			ourShader.setBool("spotOn", false);
		}
		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);

		// render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 globalRotate = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(dx, 0.2f, dz)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));	// it's a bit too big for our scene, so scale it down
		if (spaceMode == true) {
			globalRotate = glm::rotate(globalRotate, glm::radians((float)glfwGetTime()*100.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = globalRotate * model;
		}
		ourShader.setMat4("model", model);
		//draw lots of models
		/*for (int i = 0; i < numberOfModel; i++) {
			stringstream ss;
			string index;
			ss << i;
			index = ss.str();
			ourShader.setVec3(("offsets[" + index + "]").c_str(), translations[i]);
		}*/
		ourModel.Draw(ourShader);//change N draws


		//now we draw table
		tableShader.use();
		tableShader.setVec3("lightColor", lightColor);
		tableShader.setVec3("viewPos", camera.Position);
		//first we set point light
		tableShader.setVec3("pointLight.position",lightPos);
		tableShader.setVec3("pointLight.ambient", 0.1f, 0.1f, 0.1f);
		tableShader.setVec3("pointLight.diffuse", 0.8f, 0.8f, 0.8f);
		tableShader.setVec3("pointLight.specular", 0.6f, 0.6f, 0.6f);
		tableShader.setFloat("pointLight.constant", 1.0f);
		tableShader.setFloat("pointLight.linear", 0.09);
		tableShader.setFloat("pointLight.quadratic", 0.032);
		//then we set spotlight
		tableShader.setVec3("spotLight.position", spotPos);
		tableShader.setVec3("spotLight.direction", spotDir);
		tableShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		tableShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		tableShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		tableShader.setFloat("spotLight.constant", 1.0f);
		tableShader.setFloat("spotLight.linear", 0.09);
		tableShader.setFloat("spotLight.quadratic", 0.032);
		tableShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(15.0f)));
		tableShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(18.0f)));
		if (spotMode) {
			tableShader.setBool("spotOn", true);
		}
		else {
			tableShader.setBool("spotOn", false);
		}
		//set tableShader's matrix
		tableShader.setMat4("projection",projection);
		tableShader.setMat4("view", view);
		glm::mat4 tableModel = glm::mat4(1.0f);
		tableModel = glm::scale(tableModel, glm::vec3(1.5f, 1.5f, 1.5f));
		if (spaceMode == true) {
			tableModel = globalRotate * tableModel;
		}
		tableShader.setMat4("model", tableModel);
		glBindVertexArray(VAO);
		drawTable(tableShader);

		//now draw lights
		lightShader.use();
		lightShader.setMat4("projection", projection);
		lightShader.setMat4("view", view);
		glm::mat4 lightModel = glm::mat4(1.0f);
		if (runMode == true) {
			lightPos.x = 1.0f*cos((float)glfwGetTime());
			lightPos.z = 1.0f*sin((float)glfwGetTime());
		}
		if (lightColorMode == true) {
			lightColor.g = 0.0f;
		}
		else {
			lightColor.g = 1.0f;
		}
		lightModel = glm::translate(lightModel, lightPos);
		lightModel = glm::scale(lightModel, glm::vec3(0.1f)); // a smaller cube
		lightShader.setMat4("model", lightModel);
		lightShader.setVec3("lightColor", lightColor);

		glBindVertexArray(LightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//now draw spot
		spotShader.use();
		spotShader.setMat4("projection", projection);
		spotShader.setMat4("view", view);
		glm::mat4 spotModel = glm::mat4(1.0f);
		if (spotMode == true) {
			spotColor.r = 1.0f;
			spotColor.g = 1.0f;
			spotColor.b = 1.0f;
		}
		else {
			spotColor.r = 0.0f;
			spotColor.g = 0.0f;
			spotColor.b = 0.0f;
		}
		spotModel = glm::translate(spotModel, spotPos);
		spotModel = glm::scale(spotModel, glm::vec3(0.1f)); // a smaller cube
		spotShader.setMat4("model", spotModel);
		spotShader.setVec3("lightColor", spotColor);

		glBindVertexArray(LightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		if (spotDir.z <= -1);
		else spotDir.z -= 0.01;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		if (spotDir.z >= 1);
		else spotDir.z += 0.01;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		if (spotDir.x <= -1);
		else spotDir.x -= 0.01;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		if (spotDir.x >= 1);
		else spotDir.x += 0.01;
	}
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		if (dz <= -0.6);
		else dz -= 0.001;
	}
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		if (dz >= 0.6);
		else dz += 0.001;
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
		if (dx <= -0.8);
		else dx -= 0.001;
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		if (dx >= 0.8);
		else dx += 0.001;
	}
}
void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods) {
	
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		if (spaceMode == false) spaceMode = true;
		else spaceMode = false;
	}
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		if (runMode == false) runMode = true;
		else runMode = false;
	}
	if (key == GLFW_KEY_C && action == GLFW_PRESS) {
		if (lightColorMode == false) lightColorMode = true;
		else lightColorMode = false;
	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		if (spotMode == false) spotMode = true;
		else spotMode = false;
	}
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
void getFPS() {
	FPS = (int)(1.0f / deltaTime);
	interVal += deltaTime;
	if (interVal > 2) {
		interVal -= 2;
		std::cout << "FPS: " << FPS << endl;
	}
}
void drawModelList(Shader& ourShader, Model& ourmodel) {

}
void drawTable(Shader& ourShader)
{
	unsigned int indices_t1[] = {
		0,1,2,
		0,2,3,
		4,5,6,
		4,6,7,
		0,1,5,
		0,4,5,
		0,3,7,
		0,4,7,
		2,6,5,
		2,1,5,
		2,3,7,
		2,6,7
	};
	unsigned int indices_l1[36];
	unsigned int indices_l2[36];
	unsigned int indices_l3[36];
	unsigned int indices_l4[36];
	for (int j = 0; j < 36; j++) {
		indices_l1[j] = indices_t1[j] + 8;
		indices_l2[j] = indices_l1[j] + 8;
		indices_l3[j] = indices_l2[j] + 8;
		indices_l4[j] = indices_l3[j] + 8;
	}

	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_t1), indices_t1, GL_STREAM_DRAW);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_l1), indices_l1, GL_STREAM_DRAW);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_l2), indices_l2, GL_STREAM_DRAW);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_l3), indices_l3, GL_STREAM_DRAW);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_l4), indices_l4, GL_STREAM_DRAW);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glDeleteBuffers(1, &EBO);
}