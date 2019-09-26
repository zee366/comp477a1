#include "Sphere.h"

Sphere::Sphere() :
	_radius(1.0f), _sectors(360), _stacks(360)
{
	genVertexAttributes();
	genIndices();
}

Sphere::Sphere(float radius, int sectors, int stacks) :
	_radius(radius), _sectors(sectors), _stacks(stacks)
{
	genVertexAttributes();
	genIndices();
}

vector<float> Sphere::getVertices() const
{
	return _vertices;
}

vector<float> Sphere::getNormals() const
{
	return _normals;
}

vector<float> Sphere::getTexCoords() const
{
	return _texCoords;
}

vector<int> Sphere::getIndices() const
{
	return _indices;
}

glm::vec3 Sphere::getPointAt(float theta, float phi) const
{
	float x = _radius * glm::cos(phi) * glm::cos(theta);
	float y = _radius * glm::cos(phi) * glm::sin(theta);
	float z = _radius * glm::sin(phi);
	
	return glm::vec3(x, y, z);
}

void Sphere::genVertexAttributes()
{
	float x, y, z, xy;
	float nx, ny, nz;
	float s, t;
	float radiusInverse = 1.0f / _radius;
	float constexpr pi = glm::pi<float>();
	float constexpr half_pi = glm::half_pi<float>();
	
	float sectorWidth = 2 * pi / _sectors;
	float stackHeight = pi / _stacks;
	float theta; // sector (longitude) angle
	float phi; // stack (latitude) angle
	
	for(int i = 0; i < _stacks; i++)
	{
		phi = half_pi - (i * stackHeight);
		xy = _radius * glm::cos(phi);
		z = _radius * glm::sin(phi);
		
		for(int j = 0; j <= _sectors; j++)
		{
			theta = j * sectorWidth;
			
			x = xy * glm::cos(theta);
			y = xy * glm::sin(theta);
			_vertices.push_back(x);
			_vertices.push_back(y);
			_vertices.push_back(z);
			
			nx = x * radiusInverse;
			ny = y * radiusInverse;
			nz = z * radiusInverse;
			_normals.push_back(nx);
			_normals.push_back(ny);
			_normals.push_back(nz);
			
			s = (float)j / _sectors;
			t = (float)i / _stacks;
			_texCoords.push_back(s);
			_texCoords.push_back(t);
		}
	}
}

void Sphere::genIndices()
{
	int k1, k2;
	
	for(int i = 0; i < _stacks; i++)
	{
		k1 = i * (_sectors + 1);
		k2 = k1 + (_sectors + 1);
		
		for(int j = 0; j < _sectors; j++, k1++, k2++)
		{
			if(i != 0)
			{
				_indices.push_back(k1);
				_indices.push_back(k2);
				_indices.push_back(k1 + 1);
			}
			
			if(i != (_stacks - 1))
			{
				_indices.push_back(k1 + 1);
				_indices.push_back(k2);
				_indices.push_back(k2 + 1);
			}
		}
	}
}