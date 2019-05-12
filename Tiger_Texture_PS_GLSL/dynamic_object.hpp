#pragma once
#include "object.hpp"

class dynamic_object
	: object
{
public:
	dynamic_object(char* fv, char* ft);
	virtual void prepare(void);
	virtual void draw(void);
};

dynamic_object::dynamic_object(char* fv, char* ft) 
	: object(fv, ft)
{

}

inline void dynamic_object::prepare(void)
{
}

inline void dynamic_object::draw(void)
{
}
