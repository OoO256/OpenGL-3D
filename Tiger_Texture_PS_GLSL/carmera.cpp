#include "carmera.h"

#include <tuple>
#include <cmath>


spherical_coordinate rect_to_spher(const glm::vec3& pos) {
	spherical_coordinate ret;
	ret.radius = sqrt(pos.x*pos.x + pos.y*pos.y + pos.z*pos.z);
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


carmera::carmera(glm::vec3 pos)
	: pos(pos)
	, center(0)
	, up(0, 1, 0)
{
	spher = rect_to_spher(pos);
}

carmera::carmera(float radius, float angle1, float angle2)
	: spher({ radius, angle1, angle2 })
	, center(0)
	, up(0, 1, 0)
{
	pos = spher_to_rect(spher);
}

void carmera::move(float radius, float angle1, float angle2)
{
	this->move(spherical_coordinate{ radius, angle1, angle2 });
}

void carmera::move(spherical_coordinate diff)
{
	spher += diff;
	pos = spher_to_rect(spher);
}

void carmera::move(glm::vec3 pos)
{
	this->pos = pos;
	this->spher = rect_to_spher(pos);
}

glm::mat4 carmera::getView()
{
	return glm::lookAt(pos, center, up);
}