#include "carmera.h"
//#include "utility.h"
#include <tuple>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
#include <glm/gtc/matrix_inverse.hpp>

constexpr float TO_RADIAN = 0.01745329252f;
extern float aspect_ratio;


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

glm::mat4 carmera::getProj()
{
	return glm::perspective(fovy*TO_RADIAN, aspect_ratio, 100.0f, 20000.0f);
}


void carmera::move_forward(float d) {
	auto f = glm::normalize(center - pos);
	auto up = glm::vec3(0, 1, 0);
	auto r = glm::cross(f, up);

	center += d * f;
	pos += d * f;
	
	
	this->spher = rect_to_spher(pos);
}


void carmera::move_right(float d) {
	auto f = glm::normalize(center - pos);
	auto up = glm::vec3(0, 1, 0);
	auto r = glm::cross(f, up);

	center += d * r;
	pos += d * r;


	this->spher = rect_to_spher(pos);
}


void carmera::move_up(float d) {
	auto f = glm::normalize(center - pos);
	auto up = glm::vec3(0, 1, 0);
	auto r = glm::cross(f, up);

	center += d * up;
	pos += d * up;


	this->spher = rect_to_spher(pos);
}


void carmera::turn_left(float d) {
	auto up = glm::vec3(0, 1, 0);

	auto v2 = glm::rotate(glm::mat4(1), d, up)
		* glm::vec4(center.x - pos.x, center.y - pos.y, center.z - pos.z, 0);

	center = glm::vec3(v2.x, v2.y, v2.z) + pos;
}