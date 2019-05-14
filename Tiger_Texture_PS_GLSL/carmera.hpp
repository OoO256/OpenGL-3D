#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
#include <glm/gtc/matrix_inverse.hpp> // inverseTranspose, etc.

#include "main.h"
#include <tuple>
#include <cmath>

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

spherical_coordinate rect_to_spher(const glm::vec3& pos) {
	spherical_coordinate ret;
	ret.radius = sqrt(pos.x*pos.x+ pos.y*pos.y+ pos.z*pos.z);
	ret.angle1 = atan2(pos.z, pos.x);
	ret.angle2 = atan2(pos.y, pos.x);

	return ret;
}


glm::vec3 spher_to_rect(const spherical_coordinate& s) {
	return {
		s.radius*cos(s.angle2)*cos(s.angle1),
		s.radius*sin(s.angle2),
		s.radius*cos(s.angle2)*sin(s.angle1)
	};
}

glm::mat3 co;

class carmera
{
public:
	glm::vec3 pos;
	spherical_coordinate spher;

	glm::vec3 center; 
	glm::vec3 up;

	carmera(glm::vec3 pos); 
	carmera(float, float, float);
	
	void move(spherical_coordinate diff);
	void move(float radius, float angle1, float angle2);
	glm::mat4 getView();

private:

};


inline carmera::carmera(glm::vec3 pos)
	: pos(pos)
	, center(0)
	, up(0, 1, 0)
{
	spher = rect_to_spher(pos);
}

inline carmera::carmera(float radius, float angle1, float angle2)
	: spher({ radius, angle1, angle2 })
	, center(0)
	, up(0, 1, 0)
{
	pos = spher_to_rect(spher);
}

inline void carmera::move(float radius, float angle1, float angle2)
{
	this->move({ radius, angle1, angle2 });
}

inline void carmera::move(spherical_coordinate diff)
{
	spher += diff;
	pos = spher_to_rect(spher);
}

inline glm::mat4 carmera::getView()
{
	return glm::lookAt(pos, center, up);
}