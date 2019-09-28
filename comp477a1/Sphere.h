#ifndef SPHERE_H
#define SPHERE_H
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

using namespace std;

class Sphere
{
public:
	Sphere();
	Sphere(float radius, int sectors, int stacks);

	vector<float> getVertices() const;
	vector<float> getNormals() const;
	vector<float> getTexCoords() const;
	vector<int> getIndices() const;

	glm::vec3 getPointAt(float theta, float phi) const;
	glm::vec3 getCenter() const;
	void setCenter(glm::mat4);

private:
	float _radius;
	int _sectors;
	int _stacks;
	vector<float> _vertices;
	vector<float> _normals;
	vector<float> _texCoords;
	vector<int> _indices;

	glm::vec3 _center;

	void genVertexAttributes();
	void genIndices();
};
#endif SPHERE_H