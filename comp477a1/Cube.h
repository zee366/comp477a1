#include <glm/glm.hpp>

// cube vertices
float cube[] {
	// Position          // Texture coordinates
	// back face
	-5.0f, -5.0f, -5.0f,  0.0f, 0.0f,
	 5.0f, -5.0f, -5.0f,  1.0f, 0.0f,
	 5.0f,  5.0f, -5.0f,  1.0f, 1.0f,
	 5.0f,  5.0f, -5.0f,  1.0f, 1.0f,
	-5.0f,  5.0f, -5.0f,  0.0f, 1.0f,
	-5.0f, -5.0f, -5.0f,  0.0f, 0.0f,

	// front face
	-5.0f, -5.0f,  5.0f,  0.0f, 0.0f,
	 5.0f, -5.0f,  5.0f,  1.0f, 0.0f,
	 5.0f,  5.0f,  5.0f,  1.0f, 1.0f,
	 5.0f,  5.0f,  5.0f,  1.0f, 1.0f,
	-5.0f,  5.0f,  5.0f,  0.0f, 1.0f,
	-5.0f, -5.0f,  5.0f,  0.0f, 0.0f,

	// left face
	-5.0f,  5.0f,  5.0f,  1.0f, 0.0f,
	-5.0f,  5.0f, -5.0f,  1.0f, 1.0f,
	-5.0f, -5.0f, -5.0f,  0.0f, 1.0f,
	-5.0f, -5.0f, -5.0f,  0.0f, 1.0f,
	-5.0f, -5.0f,  5.0f,  0.0f, 0.0f,
	-5.0f,  5.0f,  5.0f,  1.0f, 0.0f,

	// right face
	 5.0f,  5.0f,  5.0f,  1.0f, 0.0f,
	 5.0f,  5.0f, -5.0f,  1.0f, 1.0f,
	 5.0f, -5.0f, -5.0f,  0.0f, 1.0f,
	 5.0f, -5.0f, -5.0f,  0.0f, 1.0f,
	 5.0f, -5.0f,  5.0f,  0.0f, 0.0f,
	 5.0f,  5.0f,  5.0f,  1.0f, 0.0f,

	 // bottom face
	-5.0f, -5.0f, -5.0f,  0.0f, 1.0f,
	 5.0f, -5.0f, -5.0f,  1.0f, 1.0f,
	 5.0f, -5.0f,  5.0f,  1.0f, 0.0f,
	 5.0f, -5.0f,  5.0f,  1.0f, 0.0f,
	-5.0f, -5.0f,  5.0f,  0.0f, 0.0f,
	-5.0f, -5.0f, -5.0f,  0.0f, 1.0f,

	// top face
	-5.0f,  5.0f, -5.0f,  0.0f, 1.0f,
	 5.0f,  5.0f, -5.0f,  1.0f, 1.0f,
	 5.0f,  5.0f,  5.0f,  1.0f, 0.0f,
	 5.0f,  5.0f,  5.0f,  1.0f, 0.0f,
	-5.0f,  5.0f,  5.0f,  0.0f, 0.0f,
	-5.0f,  5.0f, -5.0f,  0.0f, 1.0f
};

// calculate the norms for each face of the cube
glm::vec3 topLeftFront(cube[50], cube[51], cube[52]);
glm::vec3 botLeftFront(cube[30], cube[31], cube[32]);
glm::vec3 botRightFront(cube[35], cube[36], cube[37]);
glm::vec3 topRightFront(cube[40], cube[41], cube[42]);

glm::vec3 topLeftBack(cube[20], cube[21], cube[22]);
glm::vec3 botLeftBack(cube[0], cube[1], cube[2]);
glm::vec3 botRightBack(cube[5], cube[6], cube[7]);
glm::vec3 topRightBack(cube[10], cube[11], cube[12]);

glm::vec3 leftFaceNorm = glm::normalize(glm::cross(topLeftFront - topLeftBack, botLeftBack - topLeftBack));
glm::vec3 rightFaceNorm = glm::normalize(glm::cross(topRightBack - topRightFront, botRightFront - topRightFront));
glm::vec3 backFaceNorm = glm::normalize(glm::cross(topLeftBack - topRightBack, botRightBack - topRightBack));
glm::vec3 frontFaceNorm = glm::normalize(glm::cross(topRightFront - topLeftFront, botLeftFront - topLeftFront));
glm::vec3 topFaceNorm = glm::normalize(glm::cross(topLeftFront - topRightFront, topRightBack - topRightFront));
glm::vec3 botFaceNorm = glm::normalize(glm::cross(botLeftBack - botRightBack, botRightFront - botRightBack));