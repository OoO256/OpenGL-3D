#pragma once
#include <array>
#include <map>

class keyboard
{
public:

	std::array<bool, 1 << (sizeof(char)*8)>key_state;
	std::map<int, bool>special_state;
	std::map<int, bool>mod_state;
	// 

	int last_mouse_x = -1;
	int last_mouse_y = -1;

	keyboard();
	~keyboard();


	void key_down(unsigned char key, int x, int y);
	void key_up(unsigned char key, int x, int y);

	void special_down(int key, int x, int y);
	void special_up(int key, int x, int y);

	void motion(int x, int y, int mod);

	void action();
};

extern keyboard mykeyboard;