#pragma once
#include <vector>
#include "object.h"


class car
{
public:
	object* body;
	std::vector<object *> wheels;
	std::vector<std::vector<object *>> nuts;

	car(void);
	~car();
	void move_forward(float s);


};