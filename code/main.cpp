#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CollisionController.h"
#include "Shader.h"
#include "Camera.h"
#include "partical_life.h"
#include "texture.h"

#include "SpaceShip.h"
#include "Stone.h"

#define _STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void renderQuad();
void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods);
void saveimage(const string& file, int width, int height);
unsigned int loadCubemap(vector<std::string> faces);
void MakeStonePosition(Stone *stone, int N);
// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 1000;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


//halo setting
bool bloom = true;
float exposure = 1.0f;

//spaceship
bool postureMode = false;
bool boostSpeed = false;
bool fireBullet = false;

//glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 lightPos(0.0f, 25.0f,0.0f);
//initialize the light info

#define NumberOfStone 3

int main()
{

	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "The War of Partical", NULL, NULL);
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
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// you can use glfwSwapInterval(0) to test your maximum FPS
	// glfwSwapInterval(0);

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

	unsigned int hdrFBO;
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	// create 2 floating point color buffers (1 for normal rendering, other for brightness treshold values)
	unsigned int colorBuffers[2];
	glGenTextures(2, colorBuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// attach texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
	}
	// create and attach depth buffer (renderbuffer)
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// ping-pong-framebuffer for blurring
	unsigned int pingpongFBO[2];
	unsigned int pingpongColorbuffers[2];
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongColorbuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
		// also check if framebuffers are complete (no need for depth buffer)
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
	}
	Shader lightingShader("lights.vs", "lights.fs");
	Shader splitShader("split.vs", "split.fs", "split.gs");
	Shader particalShader = Shader("particle.vs", "particle.fs");
	Shader shaderBlur = Shader("blur.vs", "blur.fs");
	Shader shaderBloomFinal = Shader("bloom_final.vs", "bloom_final.fs");
	Shader SkyShader("Sky.vs", "Sky.fs");
	Shader shadowDepthShader("shadow_mapping_depth.vs", "shadow_mapping_depth.fs");

	//depth test VBO
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	Texture2D texture("./img/gold.jpg", true);
	texture.Generate();
	ParticleGenerator pg = ParticleGenerator(particalShader, texture, 500);
	ParticleGenerator pg2 = ParticleGenerator(particalShader, texture, 500);
	Gun gun = Gun(particalShader, texture);
	Gun gun2 = Gun(particalShader, texture);

	//Make Stone
	SpaceShip spaceship = SpaceShip();


	Stone *stone = new Stone[NumberOfStone];
	MakeStonePosition(stone, NumberOfStone);

	Material stone_material;
	stone_material.ambient = glm::vec3(0.4f);
	stone_material.diffuse = glm::vec3(0.4f);
	stone_material.specular = glm::vec3(0.8f);
	stone_material.shininess = 32.0f;


	// The vertices of skybox
	float skyboxVertices[] = {
		// positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	vector<std::string> faces
	{
		"rt",
		"lf",
		"up",
		"dn",
		"ft",
		"bk"
	};
	for (int i = 0; i != faces.size(); i++) {
		faces[i] = "skybox/purplenebula_" + faces[i] + ".jpg";
	}

	//Load the skybox
	unsigned int cubemapTexture = loadCubemap(faces);

	// shader configuration
	// --------------------

	SkyShader.use();
	SkyShader.setInt("skybox", 0);

	shaderBlur.use();
	shaderBlur.setInt("image", 0);
	shaderBloomFinal.use();
	shaderBloomFinal.setInt("scene", 0);
	shaderBloomFinal.setInt("bloomBlur", 1);

	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};


	splitShader.use();
	splitShader.setVec3("viewPos", camera.Position);
	// directional light
	splitShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	splitShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
	splitShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
	splitShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);


	lightingShader.use();
	lightingShader.setVec3("viewPos", camera.Position);

	// render loop
	// -----------
	CollisionController cc;
	bool *lastBreak = new bool[NumberOfStone];
	bool *isbreak = new bool[NumberOfStone];
	float *currentbreakFrame = new float[NumberOfStone];
	for (int i = 0; i < NumberOfStone; i++)
	{	
		lastBreak[i] = false;
		isbreak[i] = false;
		currentbreakFrame[i] = -1.57f;
	}

    //calculate fps
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    
    
    
    // Measure speed
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // input
        // -----
        processInput(window);
        
        // render
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // depth texture
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 0.1f, far_plane = 50.0f;
        lightProjection = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
        // render scene from light's point of view
        shadowDepthShader.use();
        shadowDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glm::mat4 model = glm::mat4(1.0f);
        
        
        for(int i=0; i!=NumberOfStone; i++){
            double set1 = (double(rand()%10)) /2000 ;
            double set2 = (double(rand()%10)) /2000 ;
            double set3 = (double(rand()%10)) /2000 ;
            int t = 1;
            model = glm::translate(model, stone[i].getPosition());
            stone[i].addPosition(set1, t*set2, set3);
            shadowDepthShader.setMat4("model", model);
            stone[i].Draw(shadowDepthShader);
            t = -t;
        }
        
        spaceship.Draw(shadowDepthShader, postureMode, deltaTime);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        // 1. render scene into floating point framebuffer
        // -----------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        /*
         Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
         the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
         by defining light types as classes and set their values in there, or by using a more efficient uniform approach
         by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
         */
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        /*
         We will draw a lot of things, however projection and view must be consistent
         */
        
        // view/projection transformations
        lightingShader.use();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        
        // set light uniforms
        //lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setVec3("lightPos", lightPos);
        lightingShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        spaceship.Draw(lightingShader, postureMode, deltaTime, depthMap);
        
        
        //StoneBullet(Stone *stone, glm::mat4 model)

        for(int count=0; count< NumberOfStone; count++){
			model = glm::mat4(1.0f);
            stone[count].update(stone[count].getPosition(), stone[count].getDirection());
			model = glm::translate(model, stone[count].getPosition());
            vector<Bullet> bullets = gun.getBullet();
            vector<Bullet> bullets2 = gun2.getBullet();
            
            if (!isbreak[count] && cc.checkCollision(stone[count], spaceship))
            {
                isbreak[count] = true;
            }
            for (int i = 0; i < bullets.size(); i++)
            {
                if (isbreak[count] == true)
                    break;
                if (cc.checkCollision(stone[count], bullets[i]))
					isbreak[count] = true;
            }
            for (int i = 0; i < bullets2.size(); i++)
            {
                if (isbreak[count] == true)
                    break;
                if (cc.checkCollision(stone[count], bullets2[i]))
					isbreak[count] = true;
            }
            if (isbreak[count])
            {
                //breaking speed
                currentbreakFrame[count] += 0.1f;
                if (!lastBreak[count] || currentbreakFrame[count] > 1.57f)
                {
					currentbreakFrame[count] = -1.57f;
					isbreak[count] = false;
                }
                splitShader.use();
                splitShader.setVec3("lightPos", lightPos);
                splitShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
                splitShader.setMat4("projection", projection);
                splitShader.setMat4("view", view);
                splitShader.setMat4("model", model);
                splitShader.setFloat("time", currentbreakFrame[count]);
                stone[count].Draw(splitShader);
                lastBreak[count] = true;
            }
            else
            {
                lightingShader.use();
                lightingShader.setMat4("projection", projection);
                lightingShader.setMat4("view", view);
                lightingShader.setMat4("model", model);
                stone[count].Draw(lightingShader, depthMap);
                lastBreak[count] = false;
            }
        }
        
        //model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
        /*
         then we draw partical
         */
        
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.001f, 0.001f, 0.001f));
        particalShader.use();
        particalShader.setMat4("projection", projection);
        particalShader.setMat4("view", view);
        particalShader.setMat4("model", model);
        //glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        pg.Draw();
        glDisable(GL_BLEND);
        //glEnable(GL_DEPTH_TEST);
        float offx = 0.03f*((float)(rand() % 100 - 50) / 100.0f);
        float offz = 0.03f*((float)(rand() % 100 - 50) / 100.0f);
        float offy = 0.03f*((float)(rand() % 100 - 50) / 100.0f);
        pg.Update(deltaTime, spaceship, 5, glm::vec3(offx, offy, offz)
                  + spaceship.hJetOffset*spaceship.directionRight + spaceship.vJetOffset*spaceship.directionUp
                  + spaceship.fbJetOffset*spaceship.getDirection());
        
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.001f, 0.001f, 0.001f));
        particalShader.use();
        particalShader.setMat4("projection", projection);
        particalShader.setMat4("view", view);
        particalShader.setMat4("model", model);
        //glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        pg2.Draw();
        glDisable(GL_BLEND);
        //glEnable(GL_DEPTH_TEST);
        offx = 0.03f*((float)(rand() % 100 - 50) / 100.0f);
        offz = 0.03f*((float)(rand() % 100 - 50) / 100.0f);
        offy = 0.03f*((float)(rand() % 100 - 50) / 100.0f);
        pg2.Update(deltaTime, spaceship, 5, glm::vec3(offx, offy, offz)
                   + (-spaceship.hJetOffset)*spaceship.directionRight + spaceship.vJetOffset*spaceship.directionUp
                   + spaceship.fbJetOffset*spaceship.getDirection());
        
        if (fireBullet) {
            gun.addBullet(spaceship, 4.5f*spaceship.hJetOffset*spaceship.directionRight
                          + 0.5f*spaceship.vJetOffset*spaceship.directionUp
                          + (-spaceship.fbJetOffset)*spaceship.getDirection());
            gun2.addBullet(spaceship, (-4.5f*spaceship.hJetOffset)*spaceship.directionRight
                           + 0.5f*spaceship.vJetOffset*spaceship.directionUp
                           + (-spaceship.fbJetOffset)*spaceship.getDirection());
            fireBullet = false;
        }
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
        particalShader.use();
        particalShader.setMat4("projection", projection);
        particalShader.setMat4("view", view);
        particalShader.setMat4("model", model);
        gun.Draw();
        gun.Update(deltaTime);
        
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
        particalShader.use();
        particalShader.setMat4("projection", projection);
        particalShader.setMat4("view", view);
        particalShader.setMat4("model", model);
        gun2.Draw();
        gun2.Update(deltaTime);
        
        spaceship.update(camera.Position + (-0.8f*camera.Up) + (4.0f*camera.Front)
                         , camera.Front);
        
        
        //Create the Skybox
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        SkyShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        SkyShader.setMat4("view", view);
        SkyShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default
        
        
        ////return to default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        // 2. blur bright fragments with two-pass Gaussian Blur
        // --------------------------------------------------
        bool horizontal = true, first_iteration = true;
        unsigned int amount = 8;
        shaderBlur.use();
        for (unsigned int i = 0; i < amount; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            shaderBlur.setInt("horizontal", horizontal);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
            renderQuad();
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        // 3. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
        // --------------------------------------------------------------------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderBloomFinal.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
        shaderBloomFinal.setInt("bloom", bloom);
        shaderBloomFinal.setFloat("exposure", exposure);
        renderQuad();
        
        
        
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        
        
        double currentTime = glfwGetTime();
        nbFrames++;
        if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago
            // printf and reset timer
            //printf("%f ms/frame\n", 1000.0 / double(nbFrames));
            printf("%d frame/s\n", nbFrames);
			printf("spaceship: %lf %lf %lf \n", spaceship.getPosition().x, spaceship.getPosition().y, spaceship.getPosition().z);
			for(int i = 0;i < NumberOfStone;i++)
				printf("stone[%d]: %lf %lf %lf \n",i,stone[i].getPosition().x, stone[i].getPosition().y, stone[i].getPosition().z);
            nbFrames = 0;
            lastTime += 1.0;
        }
        
    }
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
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
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        saveimage("scrennshot.jpg", SCR_WIDTH, SCR_HEIGHT);
    
    if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS)
        lightPos.z -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS)
        lightPos.z += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS)
        lightPos.x -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS)
        lightPos.x += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_KP_3) == GLFW_PRESS)
        lightPos.y -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS)
        lightPos.y += 1.0f;
    
    
}
void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods) {
    
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        postureMode = true;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS) {
        switch (button)
        {
            case GLFW_MOUSE_BUTTON_LEFT:
                fireBullet = true;
                break;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
                break;
            default:
                return;
        }
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
// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}


unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    
    int width, height, nrChannels;
    
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
                         );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    return textureID;
}



void saveimage(const string& file, int width, int height)
{
    std::string suffix = file.substr(file.find_last_of('.') + 1);
    int comp = 3;
    enum TextureType
    {
        PNG, JPG, BMP
    };
    
    TextureType type;
    if (!suffix.compare("png"))
    {
        comp = 4;
        type = PNG;
    }
    else if (!suffix.compare("jpg"))
        type = JPG;
    else if (!suffix.compare("bmp"))
        type = BMP;
    
    unsigned char* data = new unsigned char[width*height*comp];
    
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, &data[0]);
    
    unsigned char* newdata = new unsigned char[width*height*comp];
    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width * 3; j++)
            newdata[i*width*comp + j] = data[width * (height - i - 1) * comp + j];
    }
    
    switch (type)
    {
        case PNG:
            stbi_write_png(file.c_str(), width, height, comp, newdata, width * 4);
            break;
        case JPG:
            stbi_write_jpg(file.c_str(), width, height, comp, newdata, 100);
            break;
        case BMP:
            stbi_write_bmp(file.c_str(), width, height, comp, newdata);
            break;
        default:
            std::cout << "save texture failed" << std::endl;
            break;
    }
}

void MakeStonePosition(Stone *stone,int N)
{
    glm::vec3 stonePositions[] = {
        //glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    for(int i=0; i!=N; i++){
        //stone[i].setPosition(stonePositions[i]);
		stone[i].update(stonePositions[i], stone[i].getDirection());
    }
}
