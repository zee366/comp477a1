#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Camera.h"
#include "Sphere.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);
float distanceToPlane(glm::vec3 point, glm::vec3 planePoint, glm::vec3 planeNorm);

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 30.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
double deltaTime = 0.0f; // time between current frame and last frame
double lastFrame = 0.0f; // time of last frame

int main() {
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// build and compile our shader program
	// ------------------------------------
	Shader sphereShader("vertex.glsl", "fragment.glsl"); // Add path to vertex and fragment shaders here
	Shader cubeShader("cubeVertex.glsl", "cubeFragment.glsl");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------

	// SPHERE
	Sphere sphere(1, 32, 16);

	unsigned int sphereVAO, sphereVBO, sphereEBO;
	glGenVertexArrays(1, &sphereVAO);
	glGenBuffers(1, &sphereVBO);
	glGenBuffers(1, &sphereEBO);

	// bind the Vertex Array Object first, then bind and set the vertex buffer(s), and then configure the vertex attribute(s).
	glBindVertexArray(sphereVAO);

	glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
	glBufferData(GL_ARRAY_BUFFER, sphere.getVertices().size() * sizeof(float), &sphere.getVertices().front(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere.getIndices().size() * sizeof(int), &sphere.getIndices().front(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind.
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	// BOX
	float cube[] = {
		-5.0f,  5.0f,  5.0f,
		-5.0f, -5.0f,  5.0f,
		 5.0f, -5.0f,  5.0f,
		 5.0f,  5.0f,  5.0f,
		-5.0f,  5.0f, -5.0f,
		-5.0f, -5.0f, -5.0f,
		 5.0f, -5.0f, -5.0f,
		 5.0f,  5.0f, -5.0f,
	};

	float cube2[] = {
		-5.0f, -5.0f, -5.0f,  0.0f, 0.0f,
		 5.0f, -5.0f, -5.0f,  1.0f, 0.0f,
		 5.0f,  5.0f, -5.0f,  1.0f, 1.0f,
		 5.0f,  5.0f, -5.0f,  1.0f, 1.0f,
		-5.0f,  5.0f, -5.0f,  0.0f, 1.0f,
		-5.0f, -5.0f, -5.0f,  0.0f, 0.0f,

		-5.0f, -5.0f,  5.0f,  0.0f, 0.0f,
		 5.0f, -5.0f,  5.0f,  1.0f, 0.0f,
		 5.0f,  5.0f,  5.0f,  1.0f, 1.0f,
		 5.0f,  5.0f,  5.0f,  1.0f, 1.0f,
		-5.0f,  5.0f,  5.0f,  0.0f, 1.0f,
		-5.0f, -5.0f,  5.0f,  0.0f, 0.0f,

		-5.0f,  5.0f,  5.0f,  1.0f, 0.0f,
		-5.0f,  5.0f, -5.0f,  1.0f, 1.0f,
		-5.0f, -5.0f, -5.0f,  0.0f, 1.0f,
		-5.0f, -5.0f, -5.0f,  0.0f, 1.0f,
		-5.0f, -5.0f,  5.0f,  0.0f, 0.0f,
		-5.0f,  5.0f,  5.0f,  1.0f, 0.0f,

		 5.0f,  5.0f,  5.0f,  1.0f, 0.0f,
		 5.0f,  5.0f, -5.0f,  1.0f, 1.0f,
		 5.0f, -5.0f, -5.0f,  0.0f, 1.0f,
		 5.0f, -5.0f, -5.0f,  0.0f, 1.0f,
		 5.0f, -5.0f,  5.0f,  0.0f, 0.0f,
		 5.0f,  5.0f,  5.0f,  1.0f, 0.0f,

		-5.0f, -5.0f, -5.0f,  0.0f, 1.0f,
		 5.0f, -5.0f, -5.0f,  1.0f, 1.0f,
		 5.0f, -5.0f,  5.0f,  1.0f, 0.0f,
		 5.0f, -5.0f,  5.0f,  1.0f, 0.0f,
		-5.0f, -5.0f,  5.0f,  0.0f, 0.0f,
		-5.0f, -5.0f, -5.0f,  0.0f, 1.0f,

		-5.0f,  5.0f, -5.0f,  0.0f, 1.0f,
		 5.0f,  5.0f, -5.0f,  1.0f, 1.0f,
		 5.0f,  5.0f,  5.0f,  1.0f, 0.0f,
		 5.0f,  5.0f,  5.0f,  1.0f, 0.0f,
		-5.0f,  5.0f,  5.0f,  0.0f, 0.0f,
		-5.0f,  5.0f, -5.0f,  0.0f, 1.0f
	};

	unsigned int cubeIndices[] = {
		// outside
		0, 1, 2,
		0, 2, 3,
		3, 2, 6,
		3, 6, 7,
		7, 6, 5,
		7, 5, 4,
		4, 5, 1,
		4, 1, 0,
		1, 5, 6,
		1, 6, 2,
		4, 0, 3,
		4, 3, 7,
		// inside
		//3, 7, 1,
		//3, 1, 0,
		//0, 1, 5,
		//0, 5, 4,
		//4, 5, 6,
		//4, 6, 7,
		//7, 6, 2,
		//7, 2, 3,
		//0, 4, 7,
		//0, 7, 3,
		//5, 1, 7,
		//5, 7, 6
	};

	glm::vec3 topLeftFront(cube[0], cube[1], cube[2]);
	glm::vec3 botLeftFront(cube[3], cube[4], cube[5]);
	glm::vec3 botRightFront(cube[6], cube[7], cube[8]);
	glm::vec3 topRightFront(cube[9], cube[10], cube[11]);

	glm::vec3 topLeftBack(cube[12], cube[13], cube[14]);
	glm::vec3 botLeftBack(cube[15], cube[16], cube[17]);
	glm::vec3 botRightBack(cube[18], cube[19], cube[20]);
	glm::vec3 topRightBack(cube[21], cube[22], cube[23]);

	glm::vec3 leftFaceNorm = glm::normalize(glm::cross(topLeftFront - topLeftBack, botLeftBack - topLeftBack));
	glm::vec3 rightFaceNorm = glm::normalize(glm::cross(topRightBack - topRightFront, botRightFront - topRightFront));
	glm::vec3 backFaceNorm = glm::normalize(glm::cross(topLeftBack - topRightBack, botRightBack - topRightBack));
	glm::vec3 frontFaceNorm = glm::normalize(glm::cross(topRightFront - topLeftFront, botLeftFront - topLeftFront));
	glm::vec3 topFaceNorm = glm::normalize(glm::cross(topLeftFront - topRightFront, topRightBack - topRightFront));
	glm::vec3 botFaceNorm = glm::normalize(glm::cross(botLeftBack - botRightBack, botRightFront - botRightBack));

	/*
	std::cout << "left face " << leftFaceNorm.x << std::endl;
	std::cout << "right face " << rightFaceNorm.x << std::endl;
	std::cout << "back face " << backFaceNorm.z << std::endl;
	std::cout << "front face " << frontFaceNorm.z << std::endl;
	std::cout << "top face " << topFaceNorm.y << std::endl;
	std::cout << "bot face " << botFaceNorm.y << std::endl;
	*/
	//std::cout << distanceToPlane(glm::vec3(-1.0f, 1.0f, -1.0f), topLeftFront, leftFaceNorm);

	int numIndices = sizeof(cubeIndices) / sizeof(int);

	unsigned int cubeVAO, cubeVBO, cubeEBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	//glGenBuffers(1, &cubeEBO);

	glBindVertexArray(cubeVAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube2), &cube2[0], GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), &cubeIndices[0], GL_STATIC_DRAW);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// texture coordinates
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	// TEXTURE
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set texture wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate texture
	int width, height, numChannels;
	unsigned char* data = stbi_load("box.png", &width, &height, &numChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	cubeShader.use();
	cubeShader.setInt("texture1", 0);

	// set modes
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glDisable(GL_CULL_FACE);

	// render loop
	// -----------
	glm::vec3 movement = glm::vec3(0.0f, 0.0f, 0.0f);
	float move = 0.0f;
	float moveIncrement = -0.02f;
	while (!glfwWindowShouldClose(window)) {
		double currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		sphereShader.use();

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		movement.x += moveIncrement;
		glm::mat4 model = glm::translate(glm::mat4(1.0f), movement);
		sphere.setCenter(model);

		sphereShader.setMat4("projection", projection);
		sphereShader.setMat4("view", view);
		sphereShader.setMat4("model", model);

		if (distanceToPlane(sphere.getCenter(), topLeftFront, leftFaceNorm) < 1.0f) {
			moveIncrement *= -1;
		}
		
		glBindVertexArray(sphereVAO);
		glDrawElements(GL_TRIANGLES, sphere.getIndices().size(), GL_UNSIGNED_INT, 0);

		cubeShader.use();
		cubeShader.setMat4("projection", projection);
		cubeShader.setMat4("view", view);

		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &sphereVAO);
	glDeleteBuffers(1, &sphereVBO);
	glDeleteBuffers(1, &sphereEBO);

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &cubeEBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// Camera movement
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		camera.ProcessKeyboard(YAWLEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		camera.ProcessKeyboard(YAWRIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		camera.ProcessKeyboard(PITCHUP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		camera.ProcessKeyboard(PITCHDOWN, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// make sure the viewport matches the new dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// handle mouse movement (while left button is also pressed)
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
		return;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (firstMouse) {
			lastY = ypos;
			firstMouse = false;
		}

		float yoffset = lastY - ypos;
		lastY = ypos;

		camera.ProcessMouseMovement(yoffset, deltaTime);
	}
}

float distanceToPlane(glm::vec3 point, glm::vec3 planePoint, glm::vec3 planeNorm) {
	return abs(-glm::dot(planeNorm, planePoint) + glm::dot(planeNorm, point));
}