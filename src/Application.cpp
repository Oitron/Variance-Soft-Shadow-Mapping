#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <filesystem>


// opengl dependencies
#include <GL/glew.h>
#include <GLFW/glfw3.h>
// glm math lib
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// UI dependencies
#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw_gl3.h"



#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Mesh.h"



#include "Camera.h"
#include "lights/PointLight.h"
#include "lights/DirectionalLight.h"
#include "lights/SpotLight.h"





//screen settings
const unsigned int SCREEN_WIDTH = 1600;
const unsigned int SCREEN_HEIGHT = 1200;

//camera settings
Camera cam;

//mouse settings
int xLast = SCREEN_WIDTH / 2;
int yLast = SCREEN_HEIGHT / 2;
bool rightButtonPressed = false;
bool firstMouse = true;

//time settings
float deltaTime = 0.0f;



void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_W))
		cam.ProcessKeyboard(FORWORD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S))
		cam.ProcessKeyboard(BACKWORD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A))
		cam.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D))
		cam.ProcessKeyboard(RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(window, 1);
	}
}


void cursor_callback(GLFWwindow* window, double xPos, double yPos) {
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		rightButtonPressed = true;
	else
		rightButtonPressed = false;

	if (firstMouse) {
		xLast = xPos;
		yLast = yPos;
		firstMouse = false;
	}

	if (rightButtonPressed)
		cam.ProcessMouseMovement(xPos - xLast, yLast - yPos);
	else
		firstMouse = true;

	xLast = xPos;
	yLast = yPos;
}

/*-----------------------------load OBJ model function--------------------------------*/
bool loadOBJData(
	const char* path,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals
) {
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE* file = fopen(path, "r");
	if (file == NULL) {
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			//uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
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

	return true;
}

/*-----------------------------Debug (visualization shadow map) rendering function---------------------------------*/

void renderQuad(Shader &shader)
{
	//glEnable(GL_DEPTH_TEST);
	float quadVertices[] = {
		// positions        // texture Coords
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};

	VertexArray VAO;
	VertexBuffer VBO(quadVertices, sizeof(quadVertices));
	VertexBufferLayout VBL;

	VBL.Push<float>(3);
	VBL.Push<float>(2);
	VAO.AddBuffer(VBO, VBL);
	glBindVertexArray(VAO.GetID());

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
};



int main(void) {
	GLFWwindow* window;
	/* Initialize the library */
	if (!glfwInit()) //GLFW
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	const char* glsl_version = "#version 330 core";

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "VSSM", NULL, NULL);
	if (!window/*window == NULL*/)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	//glfw mouse setting
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//glfwSetMouseButtonCallback(window, mouse_callback);
	glfwSetCursorPosCallback(window, cursor_callback);

	glfwSwapInterval(1); //update every frame

	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;


	
	// load OBJ model
	std::vector<glm::vec3> SphereGroupVertices;
	std::vector<glm::vec2> SphereGroupUVs;
	std::vector<glm::vec3> SphereGroupNormals;
	std::string SphereGroupModelPath = "F:\\M2\\IG3DA\\Project\\models\\SphereGroup.obj";
	bool sg_res = loadOBJData(SphereGroupModelPath.c_str(), SphereGroupVertices, SphereGroupUVs, SphereGroupNormals);
	if (sg_res) {
		std::cout << "Model: " << SphereGroupModelPath << " loaded!" << std::endl;
	}
	else {
		std::cout << "Fail to load model: " << SphereGroupModelPath << std::endl;
	}


	
	float PlaneVertices[] = {
		// positions          // normals         // texcoords
		 5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,  5.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,  0.0f, 5.0f,
		 5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,  5.0f, 5.0f
	};

	unsigned int PlaneIndices[] = {
		0,  1,  2, // first triangle
		2,  3,  0, // second triangle 
	};

	float BoxVertices[] = {
		//position					 //normal					//texture coords				
		//back plane
		 0.5f,  0.5f,  0.5f,		 0.0f,  0.0f,  1.0f,		1.0f, 1.0f, // 00: top right		
		 0.5f, -0.5f,  0.5f,		 0.0f,  0.0f,  1.0f,		1.0f, 0.0f, // 01: bottom right
		-0.5f, -0.5f,  0.5f,		 0.0f,  0.0f,  1.0f,		0.0f, 0.0f, // 02: bottom left
		-0.5f,  0.5f,  0.5f,		 0.0f,  0.0f,  1.0f,		0.0f, 1.0f, // 03: top left
		//top plane
		 0.5f,  0.5f, -0.5f,		 0.0f,  1.0f,  0.0f,		1.0f, 0.0f, // 04: bottom right
		 0.5f,  0.5f,  0.5f,		 0.0f,  1.0f,  0.0f,		1.0f, 1.0f, // 05: top right
		-0.5f,  0.5f, -0.5f,		 0.0f,  1.0f,  0.0f,		0.0f, 0.0f, // 06: bottom left
		-0.5f,  0.5f,  0.5f,		 0.0f,  1.0f,  0.0f,		0.0f, 1.0f, // 07: top left
		//bottom plane
		 0.5f, -0.5f, -0.5f,		 0.0f, -1.0f,  0.0f,		1.0f, 0.0f, // 08: bottom right
		 0.5f, -0.5f,  0.5f,		 0.0f, -1.0f,  0.0f,		1.0f, 1.0f, // 09: top right
		-0.5f, -0.5f, -0.5f,		 0.0f, -1.0f,  0.0f,		0.0f, 0.0f, // 10: bottom left
		-0.5f, -0.5f,  0.5f,		 0.0f, -1.0f,  0.0f,		0.0f, 1.0f, // 11: top left
		//left plane
		-0.5f, -0.5f, -0.5f,		-1.0f,  0.0f,  0.0f,		1.0f, 0.0f, // 12: bottom right
		-0.5f, -0.5f,  0.5f,		-1.0f,  0.0f,  0.0f,		0.0f, 0.0f, // 13: bottom left
		-0.5f,  0.5f, -0.5f,		-1.0f,  0.0f,  0.0f,		1.0f, 1.0f, // 14: top right
		-0.5f,  0.5f,  0.5f,		-1.0f,  0.0f,  0.0f,		0.0f, 1.0f, // 15: top left
		//right plane
		 0.5f, -0.5f, -0.5f,		 1.0f,  0.0f,  0.0f,		1.0f, 0.0f, // 16: bottom right
		 0.5f, -0.5f,  0.5f,		 1.0f,  0.0f,  0.0f,		0.0f, 0.0f, // 17: bottom left
		 0.5f,  0.5f, -0.5f,		 1.0f,  0.0f,  0.0f,		1.0f, 1.0f, // 18: top right
		 0.5f,  0.5f,  0.5f,		 1.0f,  0.0f,  0.0f,		0.0f, 1.0f, // 19: top left
		//front plane
		 0.5f,  0.5f, -0.5f,		 0.0f,  0.0f, -1.0f,		1.0f, 1.0f, // 20: top right
		 0.5f, -0.5f, -0.5f,		 0.0f,  0.0f, -1.0f,		1.0f, 0.0f, // 21: bottom right
		-0.5f, -0.5f, -0.5f,		 0.0f,  0.0f, -1.0f,		0.0f, 0.0f, // 22: bottom left
		-0.5f,  0.5f, -0.5f,		 0.0f,  0.0f, -1.0f,		0.0f, 1.0f  // 23: top left
	};

	unsigned int BoxIndices[] = {
		//back plane
		0,  1,  2, // first triangle
		2,  3,  0, // second triangle 
		//top plane
		4,  5,  7,
		4,  6,  7,
		//bottom plane
		8,  9,  11,
		8,  10, 11,
		//left plane
		12, 13, 15,
		12, 14, 15,
		//right plane
		16, 17, 19,
		16, 18, 19,
		//front plane
		20, 21, 22,
		22, 23, 20
	};


	/*-------Generate SphereGroup Mesh-------*/
	Mesh SphereGroupMesh(SphereGroupVertices, SphereGroupUVs, SphereGroupNormals);
	Shader SphereGroupShader(VF_SHADER, "src/shaders/VSSM_Scene.shader");
	SphereGroupShader.Bind();
	SphereGroupMesh.setup(SphereGroupShader.GetProgram());


	VertexArray PlaneVA;
	VertexBuffer PlaneVB(PlaneVertices, sizeof(PlaneVertices));
	VertexBufferLayout PlaneLayout;
	PlaneLayout.Push<float>(3);
	PlaneLayout.Push<float>(3);
	PlaneLayout.Push<float>(2);
	PlaneVA.AddBuffer(PlaneVB, PlaneLayout);
	IndexBuffer PlaneIB(PlaneIndices, 6);
	Shader PlaneShader(VF_SHADER, "src/shaders/VSSM_Scene.shader");
	PlaneShader.Bind();
	

	VertexArray LightVA;
	VertexBuffer LightVB(BoxVertices, sizeof(BoxVertices));
	VertexBufferLayout LightLayout;
	LightLayout.Push<float>(3);
	LightLayout.Push<float>(3);
	LightLayout.Push<float>(2);
	LightVA.AddBuffer(LightVB, LightLayout);
	IndexBuffer LightIB(BoxIndices, 36);
	Shader LightShader(VF_SHADER,"src/shaders/Light.shader");
	LightShader.Bind();

		

	Shader SimpleDepthShader(VF_SHADER, "src/shaders/ShadowMap.shader");
	Shader DebugShader(VF_SHADER, "src/shaders/Debug.shader");

	Shader ComputeSATShader(CP_SHADER, "src/shaders/ComputeSAT.shader");


	//create depth map FBO
	const int SHADOW_MAP_WIDTH = 1024;
	const int SHADOW_MAP_HEIGHT = SHADOW_MAP_WIDTH;
	float textureSize = float(SHADOW_MAP_WIDTH); //send to fragment shader
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	//create depth texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 0, GL_RG, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //GL_TEXTURE_MIN_FILTER
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //GL_TEXTURE_MAG_FILTER
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_TEXTURE_WRAP_S
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //GL_TEXTURE_WRAP_T
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		
		
		
	//create buffer object
	unsigned int depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);


	//attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthMap, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	//glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);


	//frame buffer for compute variance
	unsigned int varianceFBO[2];
	unsigned int varianceTexture[2];
	glGenFramebuffers(2, varianceFBO);
	glGenTextures(2, varianceTexture);

	for (int i = 0; i < 2; ++i) {
		glBindFramebuffer(GL_FRAMEBUFFER, varianceFBO[i]);
		glBindTexture(GL_TEXTURE_2D, varianceTexture[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 0, GL_RG, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, varianceTexture[i], 0);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);


	DebugShader.Bind();
	DebugShader.SetUniform1i("u_DebugTexture", 0);

		
	PlaneShader.Bind();
	PlaneShader.SetUniform1i("u_DepthMap", 0);
	PlaneShader.SetUniform1i("u_DepthSAT", 1);


	SphereGroupShader.Bind();
	SphereGroupShader.SetUniform1i("u_DepthMap", 0);
	SphereGroupShader.SetUniform1i("u_DepthSAT", 1);


	ComputeSATShader.Bind();
	ComputeSATShader.SetUniform1i("input_image", 0);
	ComputeSATShader.SetUniform1i("output_image", 1);



	//create render
	Renderer renderer;
	//create UI
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplGlfwGL3_Init(window, true);
	//setup style
	ImGui::StyleColorsDark();


	glm::mat4 SphereGroupModel(1.0f);
	glm::mat4 PlaneModel(1.0f);
	glm::mat4 LightModel(1.0f);


	//camera settings
	float aspect_ratio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
	float fov = 45.0f;
	cam.AspectRatio = aspect_ratio;
	//deltaTime
	float lastFrame = 0.0f;
	float currentFrame = 0.0f;

	float cameraSpeed = cam.MovementSpeed;
	float mouseSensitivity = cam.MouseSensitivity;


	//objects settings
	glm::vec3 planePosition(0.0f, -2.0f, 0.0f);
	float planeScale = 3.5f;
	glm::vec3 SphereGroupPosition(0.0f, 0.0f, 0.0f);
	float SphereGroupScale = 0.2f;

	//Material settings
	glm::vec3 planeColor(1.0f, 1.0f, 1.0f);
	float planeShininess = 3.0f;
	glm::vec3 SphereGroupColor(0.9f, 0.9f, 1.0f);
	float SphereGroupShininess = 32.0f;
	

	//light settings
	PointLight pointLight;
	pointLight.Position = glm::vec3(3.0f, 2.5f, 3.0f);
	pointLight.Intensity = 1.0f;
	float lightWidth = 50.0f;

	//shadow rander
	int ShadowRenderType = 0;
	

	glEnable(GL_DEPTH_TEST);
		
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		
		/**** ---------- Render here ---------- ****/
		//Choose Shadow rander type
		if (glfwGetKey(window, GLFW_KEY_1)) {
			ShadowRenderType = 0;
		}
		if (glfwGetKey(window, GLFW_KEY_2)) {
			ShadowRenderType = 1;
		}
		if (glfwGetKey(window, GLFW_KEY_3)) {
			ShadowRenderType = 2;
		}
		if (glfwGetKey(window, GLFW_KEY_4)) {
			ShadowRenderType = 3;
		}

		renderer.Clear();

		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		cam.Fov = fov;
		cam.MovementSpeed = cameraSpeed;
		cam.MouseSensitivity = mouseSensitivity;

		//light position can change £¬so shadow map update per frame.
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 1.0f, far_plane = 100.0f;

		lightProjection = glm::perspective(glm::radians(45.0f), (float)SHADOW_MAP_WIDTH / SHADOW_MAP_HEIGHT, near_plane, far_plane);
		lightView = glm::lookAt(pointLight.Position, SphereGroupPosition, glm::vec3(0.0f, 1.0f, 0.0f));
		//transform matrix from world space to light view space.
		lightSpaceMatrix = lightProjection * lightView;
		//render scene from light's point of view
		SimpleDepthShader.Bind();
		SimpleDepthShader.SetUniformM4fv("u_LightSpaceMatrix", 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
			

		//***********----------------First Pass rendering from light view space-----------------**********************//
		//glDepthFunc(GL_LESS);
		glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glCullFace(GL_FRONT);

		SimpleDepthShader.Bind();
		SphereGroupModel = glm::mat4(1.0);
		SphereGroupModel = glm::translate(SphereGroupModel, SphereGroupPosition);
		SphereGroupModel = glm::scale(SphereGroupModel, glm::vec3(1.0f, 1.0f, 1.0f) * SphereGroupScale);
		SimpleDepthShader.SetUniformM4fv("u_Model", 1, GL_FALSE, glm::value_ptr(SphereGroupModel));
		SphereGroupMesh.draw();

			
		SimpleDepthShader.Bind();
		PlaneModel = glm::mat4(1.0);
		PlaneModel = glm::translate(PlaneModel, planePosition);
		PlaneModel = glm::scale(PlaneModel, glm::vec3(1.0f, 1.0f, 1.0f) * planeScale);
		SimpleDepthShader.SetUniformM4fv("u_Model", 1, GL_FALSE, glm::value_ptr(PlaneModel));
			
		renderer.Draw(PlaneVA, PlaneIB, SimpleDepthShader);
			
		glCullFace(GL_BACK);


		// calculate SAT
		ComputeSATShader.Bind();
		glBindImageTexture(0, depthMap, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
		glBindImageTexture(1, varianceTexture[0], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
		glDispatchCompute(SHADOW_MAP_WIDTH, 1, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		glBindImageTexture(0, varianceTexture[0], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
		glBindImageTexture(1, varianceTexture[1], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
		glDispatchCompute(SHADOW_MAP_WIDTH, 1, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);



		/***********--------------------------	Second Pass Rendering from camera view space ---------------------***********/

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_DEPTH_TEST);

		//reset viewport
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glDeleteFramebuffers(1, &depthMapFBO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, varianceTexture[1]);
			
		//SphereGroup
		SphereGroupShader.Bind();
		// light parameters
		SphereGroupShader.SetUniform1f("u_Light.intensity", pointLight.Intensity);
		SphereGroupShader.SetUniform3f("u_Light.position", pointLight.Position.x, pointLight.Position.y, pointLight.Position.z);
		SphereGroupShader.SetUniform3f("u_Light.color", pointLight.Color.x, pointLight.Color.y, pointLight.Color.z);
		SphereGroupShader.SetUniform3f("u_Light.ambient", pointLight.GetAmbient().x, pointLight.GetAmbient().y, pointLight.GetAmbient().z);
		SphereGroupShader.SetUniform3f("u_Light.diffuse", pointLight.GetDiffuse().x, pointLight.GetDiffuse().y, pointLight.GetDiffuse().z);
		SphereGroupShader.SetUniform3f("u_Light.specular", pointLight.GetSpecular().x, pointLight.GetSpecular().y, pointLight.GetSpecular().z);
		SphereGroupShader.SetUniform1f("u_Light.kc", pointLight.Constant);
		SphereGroupShader.SetUniform1f("u_Light.kl", pointLight.Linear);
		SphereGroupShader.SetUniform1f("u_Light.kq", pointLight.Quadratic);
		// material parameters
		SphereGroupShader.SetUniform3f("u_Material.color", SphereGroupColor.x, SphereGroupColor.y, SphereGroupColor.z);
		SphereGroupShader.SetUniform1f("u_Material.shininess", SphereGroupShininess);
		SphereGroupShader.SetUniform3f("u_ViewPos", cam.GetCamPos().x, cam.GetCamPos().y, cam.GetCamPos().z);
		// MVP
		SphereGroupShader.SetUniformM4fv("u_View", 1, GL_FALSE, glm::value_ptr(cam.GetViewMatrix()));
		SphereGroupShader.SetUniformM4fv("u_Projection", 1, GL_FALSE, glm::value_ptr(cam.GetProjectionMatrix(PERSPECTIVE)));
		SphereGroupShader.SetUniformM4fv("u_Model", 1, GL_FALSE, glm::value_ptr(SphereGroupModel));
		// shadow parameters
		SphereGroupShader.SetUniform1i("u_ShadowRenderType", ShadowRenderType);
		SphereGroupShader.SetUniformM4fv("u_LightSpaceMatrix", 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
		SphereGroupShader.SetUniform1f("u_TextureSize", textureSize);
		SphereGroupShader.SetUniform1f("u_LightSize", lightWidth);
		// render
		SphereGroupMesh.draw();


			
		//PLANE
		PlaneShader.Bind();
		// light parameters
		PlaneShader.SetUniform1f("u_Light.intensity", pointLight.Intensity);
		PlaneShader.SetUniform3f("u_Light.position", pointLight.Position.x, pointLight.Position.y, pointLight.Position.z);
		PlaneShader.SetUniform3f("u_Light.color", pointLight.Color.x, pointLight.Color.y, pointLight.Color.z);
		PlaneShader.SetUniform3f("u_Light.ambient", pointLight.GetAmbient().x, pointLight.GetAmbient().y, pointLight.GetAmbient().z);
		PlaneShader.SetUniform3f("u_Light.diffuse", pointLight.GetDiffuse().x, pointLight.GetDiffuse().y, pointLight.GetDiffuse().z);
		PlaneShader.SetUniform3f("u_Light.specular", pointLight.GetSpecular().x, pointLight.GetSpecular().y, pointLight.GetSpecular().z);
		PlaneShader.SetUniform1f("u_Light.kc", pointLight.Constant);
		PlaneShader.SetUniform1f("u_Light.kl", pointLight.Linear);
		PlaneShader.SetUniform1f("u_Light.kq", pointLight.Quadratic);
		// material parameters
		PlaneShader.SetUniform3f("u_Material.color", planeColor.x, planeColor.y, planeColor.z);
		PlaneShader.SetUniform1f("u_Material.shininess", planeShininess);
		PlaneShader.SetUniform3f("u_ViewPos", cam.GetCamPos().x, cam.GetCamPos().y, cam.GetCamPos().z);
		// MVP
		PlaneShader.SetUniformM4fv("u_View", 1, GL_FALSE, glm::value_ptr(cam.GetViewMatrix()));
		PlaneShader.SetUniformM4fv("u_Projection", 1, GL_FALSE, glm::value_ptr(cam.GetProjectionMatrix(PERSPECTIVE)));
		PlaneShader.SetUniformM4fv("u_Model", 1, GL_FALSE, glm::value_ptr(PlaneModel));
		// shadow parameters
		PlaneShader.SetUniform1i("u_ShadowRenderType", ShadowRenderType);
		PlaneShader.SetUniformM4fv("u_LightSpaceMatrix", 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
		PlaneShader.SetUniform1f("u_TextureSize", textureSize);
		PlaneShader.SetUniform1f("u_LightSize", lightWidth);
		// render
		renderer.Draw(PlaneVA, PlaneIB, PlaneShader);


		//LIGHT
		LightShader.Bind();
		// light color
		LightShader.SetUniform3f("u_LightColor", pointLight.Color.x, pointLight.Color.y, pointLight.Color.z);
		// MVP
		LightShader.SetUniformM4fv("u_View", 1, GL_FALSE, glm::value_ptr(cam.GetViewMatrix()));
		LightShader.SetUniformM4fv("u_Projection", 1, GL_FALSE, glm::value_ptr(cam.GetProjectionMatrix(PERSPECTIVE)));
		LightModel = glm::mat4(1.0);
		LightModel = glm::translate(LightModel, pointLight.Position);
		LightModel = glm::scale(LightModel, glm::vec3(0.2f, 0.2f, 0.2f));
		LightShader.SetUniformM4fv("u_Model", 1, GL_FALSE, glm::value_ptr(LightModel));
		// render
		renderer.Draw(LightVA, LightIB, LightShader);

		
		// Debug rendering
		glViewport(0, 0, (int)(SCREEN_WIDTH/4), (int)(SCREEN_WIDTH/4));
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		DebugShader.Bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		renderQuad(DebugShader);


		processInput(window);

		//**********---------------------- UI settings ----------------------**********// 
		ImGui_ImplGlfwGL3_NewFrame();

		{
			ImGui::Begin("Camera");
			//Camera control
			ImGui::Text("Camera control: ");
			ImGui::SliderFloat("Fov", &fov, 0.0f, 90.0f);
			ImGui::SliderFloat("Speed", &cameraSpeed, 1.0f, 8.0f);
			ImGui::SliderFloat("Sensitivity", &mouseSensitivity, 0.01f, 0.4f);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}
		{
			ImGui::Begin("Object");
			//Object control
			ImGui::Text("Object control: ");
			ImGui::SliderFloat3("Sphere group position", &SphereGroupPosition.x, -5.0f, 5.0f);
			ImGui::SliderFloat("Sphere group size", &SphereGroupScale, 0.1f, 0.5f);
			ImGui::SliderFloat3("Plane position", &planePosition.x, -5.0f, 5.0f);
			ImGui::SliderFloat("Plane size", &planeScale, 0.1f, 5.0f);
			ImGui::End();
		}
		{
			ImGui::Begin("Material");
			//Material control
			ImGui::Text("Material control: ");
			ImGui::ColorEdit3("Sphere group color", &SphereGroupColor.x);
			ImGui::SliderFloat("Sphere group shininess", &SphereGroupShininess, 1.0f, 256.0f);
			ImGui::ColorEdit3("Plane color", &planeColor.x);
			ImGui::SliderFloat("Plane shininess", &planeShininess, 1.0f, 256.0f);
			ImGui::End();
		}
		{
			ImGui::Begin("Lamp");
			//lamp control
			ImGui::Text("Lamp control: ");
			ImGui::SliderFloat3("Light position", &pointLight.Position.x, -5.0f, 5.0f);
			ImGui::SliderFloat("Light width", &lightWidth, 2.0f, 250.0f);
			ImGui::ColorEdit3("Light color", &pointLight.Color.x);
			ImGui::SliderFloat("Intensity", &pointLight.Intensity, 0.0f, 5.0f);
			//ImGui::SliderFloat("Ambient Strength", &pointLight.AmbientCoef, 0.01f, 0.9f);
			//ImGui::SliderFloat("Diffuse intensity", &pointLight.DiffuseCoef, 0.0f, 2.0f);
			//ImGui::SliderFloat("Specular strength", &pointLight.SpecularCoef, 0.0f, 1.0f);
			//ImGui::SliderFloat("Attenuation constant", &pointLight.Constant, 0.0f, 1.0f);
			//ImGui::SliderFloat("Attenuation linear", &pointLight.Linear, 0.0f, 1.0f);
			//ImGui::SliderFloat("Attenuation quadratic", &pointLight.Quadratic, 0.0f, 2.0f);
			ImGui::End();
		}
		{
			ImGui::Begin("Shadow Render Mode");
			if (ShadowRenderType == 0) {
				ImGui::Text("Basic");
			}
			else if (ShadowRenderType == 1) {
				ImGui::Text("PCF");
			}
			else if (ShadowRenderType == 2) {
				ImGui::Text("PCSS");
			}
			else if (ShadowRenderType == 3) {
				ImGui::Text("VSSM");
			}
			ImGui::End();
		}


		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	//glDeleteProgram(shader);
	
	//Cleanup
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	//glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}