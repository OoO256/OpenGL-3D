#include "keyboard.h"

#include <iostream>
#include <vector>

#include "main.h"
#include "car.h"
#include "object.h"
#include "carmera.h"


extern car* car1;
extern object* slected;
extern std::vector<carmera>cams;
extern carmera* cur_cam;



void keyboard(unsigned char key, int x, int y) {
	printf("key : %d\n", key);

	static int flag_cull_face = 0;
	static int PRP_distance_level = 4;

	switch (key) {
	case '0':
	case '1':
		cur_cam = &cams[key - '0'];
		break;
	case 'a':
		slected->turn_left(5 * TO_RADIAN);
		car1->move_forward(5);
		break;
	case 's':
		car1->move_forward(-5);
		break;
	case 'd':
		slected->turn_left(-5 * TO_RADIAN);
		car1->move_forward(5);
		break;
	case 'w':
		car1->move_forward(5);
		break;
	case 44:
		cams[0].move(-10, 0, 0);
		break;
	case 46:
		cams[0].move(10, 0, 0);
		break;
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups
		break;
	}
}

void special_keyboard(int key, int x, int y) {
	printf("special key entered : %d\n", key);

	switch (key)
	{
	case GLUT_KEY_UP:
		//do something here
		cams[0].move(0, 0, 0.1);
		break;
	case GLUT_KEY_DOWN:
		//do something here
		cams[0].move(0, 0, -0.1);
		break;
	case GLUT_KEY_LEFT:
		//do something here
		cams[0].move(0, 0.1, 0);
		break;
	case GLUT_KEY_RIGHT:
		//do something here
		cams[0].move(0, -0.1, 0);
		break;
	default:
		break;
	}
}