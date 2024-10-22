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
extern bool togle[3];

extern object ironman;


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


	if (key == '1')
		togle[0] = !togle[0];

	if (key == '2')
		togle[1] = !togle[1];

	if (key == '3')
		togle[2] = !togle[2];
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

		if (last_mouse_x == -1 && last_mouse_y == -1) {
			last_mouse_x = x;
			last_mouse_y = y;
		}
		else
		{
			if (x - last_mouse_x > 0) {
				cams[0].fovy *= 1.01;
			}
			else if(x - last_mouse_x < 0)
			{
				cams[0].fovy *= 0.99;
			}


			last_mouse_x = x;
			last_mouse_y = y;
		}
	}
	else if (mod == GLUT_ACTIVE_ALT) {

		if (last_mouse_x == -1 && last_mouse_y == -1) {
			last_mouse_x = x;
			last_mouse_y = y;
		}
		else
		{
			if (x - last_mouse_x > 0) {
				cams[1].fovy *= 1.01;
			}
			else if (x - last_mouse_x < 0)
			{
				cams[1].fovy *= 0.99;
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
	if (togle[0]) {
		if (key_state['a']) {
			cams[1].move_right(-5);
		}

		if (key_state['s']) {
			cams[1].move_forward(-5);
		}

		if (key_state['d']) {
			cams[1].move_right(5);
		}

		if (key_state['w']) {
			cams[1].move_forward(5);
		}

		if (key_state['q']) {
			cams[1].move_up(5);
		}

		if (key_state['e']) {
			cams[1].move_up(-5);
		}

		if (key_state['c']) {
			cams[1].turn_left(-2 * TO_RADIAN);
		}

		if (key_state['z']) {
			cams[1].turn_left(2 * TO_RADIAN);
		}
	}
	else if (slected != nullptr)
	{
		if (key_state['a']) {
			slected->turn_left(3 * TO_RADIAN);
			//printf("%f %f %f\n", slected->position.x, slected->position.y, slected->position.z);
		}

		if (key_state['s']) {
			slected->move_forward(-5);
		}

		if (key_state['d']) {
			slected->turn_left(-3 * TO_RADIAN);
		}

		if (key_state['w']) {
			slected->move_forward(5);
		}

		if (key_state[' ']) {
			//slected->velocity.y += 100;
		}
	}

	if (key_state['u']) {
		ironman.velocity.y = 300;
	}

	if (key_state['i']) {
		ironman.vel_angle += 0.01;
	}

	if (key_state['o']) {
		ironman.vel_angle = 0;
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