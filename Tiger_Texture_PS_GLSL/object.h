#pragma once
#include <vector>
#include <deque>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <FreeImage/FreeImage.h>
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
#include <glm/gtc/matrix_inverse.hpp> // inverseTranspose, etc.

#include "utility.h"
#include "My_Shading.h"

extern unsigned int timestamp_scene;

class object
{
public:
	GLuint vbo, vao;
	Material_Parameters material;
	OBJ_TYPE type;

	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 scale;
	unsigned int timestamp_last = 0;
	glm::vec3 original_dir;
	float vel_angle;

	std::deque<glm::mat4> rotate;

	const int num_frames;
	int cur_frame;
	std::vector<int>num_triangles;
	std::vector<int>vertex_offset;
	std::vector<GLfloat *>vertices;

	std::string name;
	std::string filename_vertices;
	std::string filename_texture;
	bool is_texture_on;
	bool is_binary_file;

	object* parent;

	object() : num_frames(0) {};
	object(int num_frames, std::string fv, OBJ_TYPE type);
	void prepare(void);
	void draw(const glm::mat4& ViewMatrix, const glm::mat4& ProjectionMatrix);
	void next_frame() { cur_frame = (cur_frame + 1) % num_frames; };


	void updata_pos();

	void move_forward(float s);
	void turn_left(float rad);
	//void align(glm::vec3);

	~object();


	glm::mat4 getModelMatrix();
};