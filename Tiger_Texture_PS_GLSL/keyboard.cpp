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
extern int is_time_running;
extern float fovy;

keyboard mykeyboard;

keyboard::keyboard()
{
	key_state.fill(false);

	special_state[GLUT_KEY_UP] = false;
	special_state[GLUT_KEY_DOWN] = false; 
	special_state[GLUT_KEY_RIGHT] = false;
	special_state[GLUT_KEY_LEFT] = false; 
	special_state[GLUT_KEY_END] = false;

	mod_state[GLUT_ACTIVE_SHIFT] = false;
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

void keyboard::motion(int x, int y, int mod)
{
	if (mod == GLUT_ACTIVE_SHIFT) {
		printf("%d %d\n", x, y);

		if (last_mouse_x == -1 && last_mouse_y == -1) {
			last_mouse_x = x;
			last_mouse_y = y;
		}
		else
		{
			if (x - last_mouse_x > 0) {
				fovy *= 1.01;
			}
			else if(x - last_mouse_x < 0)
			{
				fovy *= 0.99;
			}


			last_mouse_x = x;
			last_mouse_y = y;
		}
	}
	else
	{
		last_mouse_x = -1;
		last_mouse_y = -1;
	}
}

void keyboard::action()
{
	if(key_state['0'])
		cur_cam = &cams[0];

	if (key_state['1'])
		cur_cam = &cams[1];


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
	if (special_state[GLUT_KEY_END]) {
		is_time_running = !special_state[GLUT_KEY_END];
	}
}