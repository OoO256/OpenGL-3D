#pragma once
#include <array>
#include <map>

class keyboard
{
public:

	std::array<bool, 1 << (sizeof(char)*8)>key_state;
	std::map<int, bool>special_state;
	// 

	keyboard();
	~keyboard();


	void key_down(unsigned char key, int x, int y);
	void key_up(unsigned char key, int x, int y);

	void special_down(int key, int x, int y);
	void special_up(int key, int x, int y);

	void action();
};

extern keyboard mykeyboard;