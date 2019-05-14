#pragma once
#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>
#include <vector>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <FreeImage/FreeImage.h>
#include "Shaders/LoadShaders.h"
#include "My_Shading.h"
#include <glm/gtc/matrix_transform.hpp>


#include "main.h"

extern GLuint texture_names[N_TEXTURES_USED];
constexpr int MAX_FILENAME = 512;

class object
{
private:
	GLfloat *vertices;
	int num_triangles;

public:
	GLuint vbo, vao;
	Material_Parameters material;

	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 scale;
	glm::vec3 rotate;

	std::string name;
	std::string filename_vertices;
	std::string filename_texture;

	object(std::string fv, std::string ft, glm::vec3 position);
	virtual void prepare(void);
	virtual void draw(void);


	glm::mat4 getModelMatrix();
	glm::mat4 getModelViewMatrix(glm::mat4 ViewMatrix);
};

object::object
(
	std::string fv
	, std::string ft = "Data/dynamic_objects/tiger/tiger_tex2.jpg"
	, glm::vec3 position = glm::vec3(0.0f)
)
	: filename_vertices(fv)
	, filename_texture(ft)
	, position(position)
	, velocity(0)
	, acceleration(0)
	, scale(1)
	, rotate(0)
{

}

inline void object::prepare(void)
{
	int num_bytes_per_vertex, num_bytes_per_triangle, num_total_triangles = 0;

	num_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	num_bytes_per_triangle = 3 * num_bytes_per_vertex;

	num_triangles = read_geometry(&vertices, num_bytes_per_triangle, &filename_vertices[0]);
	// assume all geometry files are effective
	num_total_triangles += num_triangles;


	// initialize vertex buffer object
	glGenBuffers(1, &vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, num_total_triangles * 3 * num_bytes_per_vertex, vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, num_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, num_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, num_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glActiveTexture(GL_TEXTURE0 + TEXTURE_ID_TIGER);
	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_ID_TIGER]);

	My_glTexImage2D_from_file(&filename_texture[0]);

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

inline void object::draw(void) {
	glFrontFace(GL_CW);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 3 * num_triangles);
	glBindVertexArray(0);
}

glm::mat4 object::getModelMatrix()
{
	glm::mat4 ModelMatrix(1.0f);

	ModelMatrix = glm::translate(
		ModelMatrix,
		this->position
	);
	
	ModelMatrix = glm::rotate(
		ModelMatrix,
		this->rotate.x,
		glm::vec3(1.0f, 0.0f, 0.0f)
	);

	ModelMatrix = glm::rotate(
		ModelMatrix,
		this->rotate.y,
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	ModelMatrix = glm::rotate(
		ModelMatrix,
		this->rotate.z,
		glm::vec3(0.0f, 0.0f, 1.0f)
	);

	ModelMatrix = glm::scale(
		ModelMatrix,
		this->scale
	);

	return ModelMatrix;
}

inline glm::mat4 object::getModelViewMatrix
(
	glm::mat4 ViewMatrix
)
{
	glm::mat4 ModelViewMatrix(1.0f);

	ModelViewMatrix = glm::translate(
		ViewMatrix,
		this->position
	);
	/*
	ModelViewMatrix = glm::rotate(
		ModelViewMatrix,
		this->rotate.x,
		glm::vec3(1.0f, 0.0f, 0.0f)
	);

	ModelViewMatrix = glm::rotate(
		ModelViewMatrix,
		this->rotate.y,
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	ModelViewMatrix = glm::rotate(
		ModelViewMatrix,
		this->rotate.z,
		glm::vec3(0.0f, 0.0f, 1.0f)
	);

	ModelViewMatrix = glm::scale(
		ModelViewMatrix,
		this->scale
	);
	*/
	return ModelViewMatrix;
}
