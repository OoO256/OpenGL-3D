#pragma once
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
#include <glm/gtc/matrix_inverse.hpp> // inverseTranspose, etc.

struct spherical_coordinate
{
	float radius, angle1, angle2;

	spherical_coordinate() {}

	spherical_coordinate(float radius, float angle1, float angle2)
		: radius(radius)
		, angle1(angle1)
		, angle2(angle2)
	{}

	void operator+=(const spherical_coordinate& rhs) {
		this->radius += rhs.radius;
		this->angle1 += rhs.angle1;
		this->angle2 += rhs.angle2;
	}
};


class carmera
{
public:
	glm::vec3 pos;
	spherical_coordinate spher;
	float fovy = 45.0;

	glm::vec3 center;
	glm::vec3 up;

	carmera(glm::vec3 pos); 
	carmera(float, float, float);
	
	void move(spherical_coordinate diff);
	void move(float radius, float angle1, float angle2);
	void move(glm::vec3);
	glm::mat4 getView();
	glm::mat4 getProj();

	void move_forward(float);
	void move_right(float);
	void move_up(float);
	void turn_left(float d);

private:

};
