#include <glm/glm.hpp>

float cube[] {
	// back face
	-5.0f, -5.0f, -5.0f,  0.0f, 0.0f, // 0
	 5.0f, -5.0f, -5.0f,  1.0f, 0.0f, // 5
	 5.0f,  5.0f, -5.0f,  1.0f, 1.0f, // 10
	 5.0f,  5.0f, -5.0f,  1.0f, 1.0f, // 15
	-5.0f,  5.0f, -5.0f,  0.0f, 1.0f, // 20
	-5.0f, -5.0f, -5.0f,  0.0f, 0.0f, // 25

	// front face
	-5.0f, -5.0f,  5.0f,  0.0f, 0.0f, // 30
	 5.0f, -5.0f,  5.0f,  1.0f, 0.0f, // 35
	 5.0f,  5.0f,  5.0f,  1.0f, 1.0f, // 40
	 5.0f,  5.0f,  5.0f,  1.0f, 1.0f, // 45
	-5.0f,  5.0f,  5.0f,  0.0f, 1.0f, // 50
	-5.0f, -5.0f,  5.0f,  0.0f, 0.0f, // 55

	// left face
	-5.0f,  5.0f,  5.0f,  1.0f, 0.0f, // 60
	-5.0f,  5.0f, -5.0f,  1.0f, 1.0f, // 65
	-5.0f, -5.0f, -5.0f,  0.0f, 1.0f, // 70
	-5.0f, -5.0f, -5.0f,  0.0f, 1.0f, // 75
	-5.0f, -5.0f,  5.0f,  0.0f, 0.0f, // 80
	-5.0f,  5.0f,  5.0f,  1.0f, 0.0f, // 85

	// right face
	 5.0f,  5.0f,  5.0f,  1.0f, 0.0f, // 90
	 5.0f,  5.0f, -5.0f,  1.0f, 1.0f, // 95
	 5.0f, -5.0f, -5.0f,  0.0f, 1.0f, // 100
	 5.0f, -5.0f, -5.0f,  0.0f, 1.0f, // 105
	 5.0f, -5.0f,  5.0f,  0.0f, 0.0f, // 110
	 5.0f,  5.0f,  5.0f,  1.0f, 0.0f, // 115

	 // bottom face
	-5.0f, -5.0f, -5.0f,  0.0f, 1.0f, // 120
	 5.0f, -5.0f, -5.0f,  1.0f, 1.0f, // 125
	 5.0f, -5.0f,  5.0f,  1.0f, 0.0f, // 130
	 5.0f, -5.0f,  5.0f,  1.0f, 0.0f, // 135
	-5.0f, -5.0f,  5.0f,  0.0f, 0.0f, // 140
	-5.0f, -5.0f, -5.0f,  0.0f, 1.0f, // 145

	// top face
	-5.0f,  5.0f, -5.0f,  0.0f, 1.0f, // 150
	 5.0f,  5.0f, -5.0f,  1.0f, 1.0f, // 155
	 5.0f,  5.0f,  5.0f,  1.0f, 0.0f, // 160
	 5.0f,  5.0f,  5.0f,  1.0f, 0.0f, // 165
	-5.0f,  5.0f,  5.0f,  0.0f, 0.0f, // 170
	-5.0f,  5.0f, -5.0f,  0.0f, 1.0f  // 175
};

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