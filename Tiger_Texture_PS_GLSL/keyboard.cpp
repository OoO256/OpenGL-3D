#include "keyboard.h"

#include <iostream>
#include <vector>
#include <array>

#include "main.h"
#include "car.h"
#include "object.h"
#include "carmera.h"

extern car* car1;
extern object* slected;
extern std::vector<carmera>cams;
extern carmera* cur_cam;



keyboard mykeyboard;

keyboard::keyboard()
{
	key_state.fill(false);

	special_state[GLUT_KEY_UP] = false;
	special_state[GLUT_KEY_DOWN] = false; 
	special_state[GLUT_KEY_RIGHT] = false;
	special_state[GLUT_KEY_LEFT] = false;
}

keyboard::~keyboard()
{
}

void keyboard::key_down(unsigned char key, int x, int y)
{
	key_state[key] = true;
}

void keyboard::key_up(unsigned char key, int x, int y)
{
	key_state[key] = false;
}

void keyboard::special_down(int key, int x, int y)
{
	if (special_state.find(key) != special_state.end()) {
		special_state[key] = true;
	}
}

void keyboard::special_up(int key, int x, int y)
{
	if (special_state.find(key) != special_state.end()) {
		special_state[key] = false;
	}
}

void keyboard::action()
{
	if(key_state['0'])
		cur_cam = &cams[0];

	if (key_state['1'])
		cur_cam = &cams[0];


	if (key_state['a']) {
		slected->turn_left(1 * TO_RADIAN);
		car1->move_forward(0.01);
	}

	if (key_state['s']) {
		car1->move_forward(-1);
	}

	if (key_state['d']) {
		slected->turn_left(-1 * TO_RADIAN);
		car1->move_forward(1);
	}

	if (key_state['w']) {
		car1->move_forward(1);
	}

	if (key_state[44]) {
		cams[0].move(-1, 0, 0);
	}

	if (key_state[46]) {
		cams[0].move(1, 0, 0);
	}

	if (key_state[27]) {
		glutLeaveMainLoop();
	}

	if (special_state[GLUT_KEY_UP]) {
		cams[0].move(0, 0, 0.01);
	}

	if (special_state[GLUT_KEY_DOWN]) {
		cams[0].move(0, 0, -0.01);
	}

	if (special_state[GLUT_KEY_LEFT]) {
		cams[0].move(0, 0.01, 0);
	}

	if (special_state[GLUT_KEY_RIGHT]) {
		cams[0].move(0, -0.01, 0);
	}
}