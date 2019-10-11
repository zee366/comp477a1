/*
COMP 477 Fall 2019
Assignment 1
Submitted by:
Jason Brennan - 27793928
October 11, 2019

Please view the readme to see the controls

This program contains code borrowed from:
https://learnopengl.com/
	- Camera.h (used for handling the camera)
	- Shader.h (used to build, compile and run shaders)
	- stb_image.h/cpp (used for creating a loading textures)
http://www.songho.ca/opengl/gl_sphere.html
	- Sphere.h/cpp (used for constructing a sphere object)
https://github.com/smokhov/comp477-samples/tree/master/src/Demos/buildcurve
	- buildcurve.cpp (used for interpolating a curve on a set of control points)
	- cugl.h/cpp (needed by buildcurve.cpp)
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Camera.h"
#include "Sphere.h"
#include "Cube.h"
#include "cugl.h"
#include "Curve.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Forward declaration of utility functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);
float distanceToPlane(glm::vec3 point, glm::vec3 planePoint, glm::vec3 planeNorm);
bool checkCollisions(glm::vec3 sphereCenter, glm::vec3& velocity);
void buildCurve(Curve& c);
cugl::Point glmVecToCuglPoint(const glm::vec3& v);
glm::vec3 cuglPointToGlmVec(const cugl::Point& p);

// *****************************
// GLOBAL VARIABLES AND SETTINGS
// *****************************

// viewport settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

// camera settings
jbogl::Camera camera(glm::vec3(0.0f, 0.0f, 30.0f));
double lastX = SCR_WIDTH / 2.0f;
double lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
double deltaTime = 0.0f;
double lastFrame = 0.0f;

// physics
float gravity = -9.81f;
float dragForce = 1.2f;
bool kineticLoss = false;


// general
const float INIT_POSITION = 0.0f;
unsigned int menuChoice = 1;
double constexpr pi = glm::pi<double>();

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

	// build and compile shaders
	// -------------------------
	Shader sphereShader("vertex.glsl", "fragment.glsl"); // Add path to vertex and fragment shaders here
	Shader cubeShader("cubeVertex.glsl", "cubeFragment.glsl");
	Shader curveShader("curveVertex.glsl", "curveFragment.glsl");
	Shader sphereOnCurveShader("sphereOnCurveVertex.glsl", "sphereOnCurveFragment.glsl");

	// set up vertex data (and buffer(s)) and configure vertex attributes for each object
	// ----------------------------------------------------------------------------------

	// ******
	// SPHERE
	// ******
	glm::vec3 initialPosition = glm::vec3(INIT_POSITION);
	Sphere sphere(1.0f, 32, 16, initialPosition);

	unsigned int sphereVAO, sphereVBO, sphereEBO;
	glGenVertexArrays(1, &sphereVAO);
	glGenBuffers(1, &sphereVBO);
	glGenBuffers(1, &sphereEBO);

	glBindVertexArray(sphereVAO);
	glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
	glBufferData(GL_ARRAY_BUFFER, sphere.getVertices().size() * sizeof(float), &sphere.getVertices().front(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere.getIndices().size() * sizeof(int), &sphere.getIndices().front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// ****
	// CUBE
	// ****
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);

	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), &cube[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	// ****************
	// TEXTURE FOR CUBE
	// ****************
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
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

	// set uniform for texture
	cubeShader.use();
	cubeShader.setInt("texture1", 0);

	// *****
	// CURVE
	// *****
	Curve c;
	buildCurve(c);
	double t = 0.0;
	vector<float> curvePoints;
	const int NP = 1000;
	for (int i = 0; i <= NP; ++i) {
		glm::vec3 coordinate = cuglPointToGlmVec(c.getPos(double(i) / double(NP)));
		curvePoints.push_back(coordinate.x);
		curvePoints.push_back(coordinate.y);
		curvePoints.push_back(coordinate.z);
		//std::cout << coordinate.x << " " << coordinate.y << " " << coordinate.z << std::endl;
	}

	unsigned int curveVAO, curveVBO;
	glGenVertexArrays(1, &curveVAO);
	glGenBuffers(1, &curveVBO);

	glBindVertexArray(curveVAO);
	glBindBuffer(GL_ARRAY_BUFFER, curveVBO);
	glBufferData(GL_ARRAY_BUFFER, curvePoints.size() * sizeof(float), &curvePoints.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// ***************
	// SPHERE ON CURVE
	// ***************
	Sphere sphereOnCurve(1.0f, 32, 16, initialPosition);
	unsigned int sphereOnCurveVAO, sphereOnCurveVBO, sphereOnCurveEBO;
	glGenVertexArrays(1, &sphereOnCurveVAO);
	glGenBuffers(1, &sphereOnCurveVBO);
	glGenBuffers(1, &sphereOnCurveEBO);

	glBindVertexArray(sphereOnCurveVAO);
	glBindBuffer(GL_ARRAY_BUFFER, sphereOnCurveVBO);
	glBufferData(GL_ARRAY_BUFFER, sphereOnCurve.getVertices().size() * sizeof(float), &sphereOnCurve.getVertices().front(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereOnCurveEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereOnCurve.getIndices().size() * sizeof(int), &sphereOnCurve.getIndices().front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// ****************
	// SET OPENGL MODES
	// ****************
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_LINE_STIPPLE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ZERO, GL_SRC_COLOR);

	// settings for sphere inside cube
	glm::vec3 acceleration = glm::vec3(0.0f);
	glm::vec3 velocity = glm::vec3(-3.0f, 1.5f, -2.1f);
	glm::vec3 newVelocity(0.0f);
	glm::vec3 position = glm::vec3(0.0f);

	// settings for sphere on curve
	glm::vec3 sphereOnCurvePosition = glm::vec3(0.0f);
	const double DT = 10;
	const double totalTime = 10000; // Milliseconds needed to traverse curve
	double startTime = GetTickCount64();
	double pathTime = 0;
	double i = 0.0f;

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window)) {
		double currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		
		// for sphere on curve
		double currTime = GetTickCount64() - startTime;
		while (pathTime < currTime)
			pathTime += DT;
		i = pathTime / totalTime;
		if (i > 1.0)
		{
			startTime = GetTickCount64();
			pathTime = 0;
		}

		// ease in, ease out
		t = 0.5f * (glm::sin((i - 0.5f) * pi) + 1.0f);

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		sphereShader.use();

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();

		switch (menuChoice) {
		case 1:
			kineticLoss = false;
			newVelocity = velocity + acceleration * (float)deltaTime * 0.5f;
			break;
		case 2:
			kineticLoss = false;
			newVelocity = velocity + (glm::vec3(acceleration.x, acceleration.y + gravity, acceleration.z)) * (float)deltaTime * 0.5f;
			break;
		case 3:
			kineticLoss = false;
			newVelocity = velocity + (acceleration - (0.5f * dragForce * (velocity * glm::abs(velocity)))) * (0.5f * (float)deltaTime);
			break;
		case 4:
			kineticLoss = true;
			newVelocity = velocity;
			break;
		case 5:
			kineticLoss = true;
			newVelocity = velocity + (glm::vec3(acceleration.x, acceleration.y + gravity, acceleration.z) - (0.5f * 0.5f * (velocity * glm::abs(velocity)))) * ((float)deltaTime * 0.5f);
			break;
		}

		acceleration = (newVelocity - velocity) / (float)deltaTime;
		velocity = newVelocity;

		if (checkCollisions(sphere.getCenter(), velocity)) {
			if (kineticLoss) {
				velocity *= 0.9f;
			}
		}

		// set position of sphere in cube
		position += velocity * (float)deltaTime + acceleration * ((float)deltaTime * (float)deltaTime * 0.5f);

		glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
		sphere.setCenter(model);

		sphereShader.setMat4("projection", projection);
		sphereShader.setMat4("view", view);
		sphereShader.setMat4("model", model);
		
		// DRAW SPHERE IN CUBE
		glBindVertexArray(sphereVAO);
		glDrawElements(GL_TRIANGLES, sphere.getIndices().size(), GL_UNSIGNED_INT, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		cubeShader.use();
		cubeShader.setMat4("projection", projection);
		cubeShader.setMat4("view", view);

		// DRAW CUBE
		glDepthMask(GL_FALSE);
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glDepthMask(GL_TRUE);

		curveShader.use();
		curveShader.setMat4("projection", projection);
		curveShader.setMat4("view", view);

		// DRAW CURVE
		glBindVertexArray(curveVAO);
		glDrawArrays(GL_LINE_STRIP, 0, NP);

		sphereOnCurvePosition = cuglPointToGlmVec(c.getPos(t));

		glm::mat4 sphereOnCurveModel = glm::translate(glm::mat4(1.0f), sphereOnCurvePosition);
		sphereOnCurve.setCenter(sphereOnCurveModel);

		sphereOnCurveShader.use();
		sphereOnCurveShader.setMat4("projection", projection);
		sphereOnCurveShader.setMat4("view", view);
		sphereOnCurveShader.setMat4("model", sphereOnCurveModel);

		// DRAW SPHERE ON CURVE
		glBindVertexArray(sphereOnCurveVAO);
		glDrawElements(GL_TRIANGLES, sphereOnCurve.getIndices().size(), GL_UNSIGNED_INT, 0);

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

	glDeleteVertexArrays(1, &curveVAO);
	glDeleteBuffers(1, &curveVBO);

	glDeleteVertexArrays(1, &sphereOnCurveVAO);
	glDeleteBuffers(1, &sphereOnCurveVBO);
	glDeleteBuffers(1, &sphereOnCurveEBO);

	glDeleteTextures(1, &texture);

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
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		camera.ProcessKeyboard(YAWLEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		camera.ProcessKeyboard(YAWRIGHT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		camera.ProcessKeyboard(PITCHUP, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		camera.ProcessKeyboard(PITCHDOWN, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		menuChoice = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		menuChoice = 2;
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
		menuChoice = 3;
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
		menuChoice = 4;
	}
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
		menuChoice = 5;
	}
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

		double yoffset = lastY - ypos;
		lastY = ypos;

		camera.ProcessMouseMovement((float)yoffset, (float)deltaTime);
	}
}

// returns the distance from a point to a plane
float distanceToPlane(glm::vec3 point, glm::vec3 planePoint, glm::vec3 planeNorm) {
	return abs(-glm::dot(planeNorm, planePoint) + glm::dot(planeNorm, point));
}

// check all faces of a cube for collision, since the sphere is radius 1, collision occurs if distance is less than 1
bool checkCollisions(glm::vec3 sphereCenter, glm::vec3& velocity) {
	if (distanceToPlane(sphereCenter, topLeftFront, leftFaceNorm) < 1.0f || distanceToPlane(sphereCenter, topRightFront, rightFaceNorm) < 1.0f) {
		velocity.x = -velocity.x;
		return true;
	}
	else if (distanceToPlane(sphereCenter, topLeftFront, topFaceNorm) < 1.0f || distanceToPlane(sphereCenter, botLeftFront, botFaceNorm) < 1.0f) {
		velocity.y = -velocity.y;
		return true;
	}
	else if (distanceToPlane(sphereCenter, topLeftFront, frontFaceNorm) < 1.0f || distanceToPlane(sphereCenter, topLeftBack, backFaceNorm) < 1.0f) {
		velocity.z = -velocity.z;
		return true;
	}
	return false;
}

// Control points for the curve
void buildCurve(Curve& c)
{
	vector<Point> sp;
	sp.push_back(Point(-7.0f, 6.0f, -8.0f));
	sp.push_back(Point(7.0f, 5.0f, -8.5f));
	sp.push_back(Point(7.3f, 7.5f, 7.5f));
	sp.push_back(Point(-8.0f, 3.0f, 6.5f));
	sp.push_back(Point(-8.2f, 2.0f, -8.3f));
	sp.push_back(Point(9.0f, 1.0f, -9.5f));
	sp.push_back(Point(7.5f, 0.0f, 7.0f));
	sp.push_back(Point(-10.0f, -1.0f, 8.8f));

	vector<Point> ep;
	ep.push_back(Point(0.0f, 5.5f, -8.5f));
	ep.push_back(Point(6.9f, 4.5f, 0.0f));
	ep.push_back(Point(0.0f, 3.5f, 7.5f));
	ep.push_back(Point(-8.5f, 2.5f, 0.0f));
	ep.push_back(Point(0.0f, -1.5f, -13.5f));
	ep.push_back(Point(10.5f, 0.5f, 0.0f));
	ep.push_back(Point(0.0f, -3.5f, 9.5f));
	ep.push_back(Point(-12.5f, -8.5f, 0.0f));
	c.setPoints(sp, ep);
}

// utility function that converts a glm::vec3 to cugl::point
cugl::Point glmVecToCuglPoint(const glm::vec3& v) {
	return cugl::Point(v.x, v.y, v.z);
}

// utility function that converts a cugl::point to a glm::vec3
glm::vec3 cuglPointToGlmVec(const cugl::Point& p) {
	return glm::vec3(p[0], p[1], p[2]);
}