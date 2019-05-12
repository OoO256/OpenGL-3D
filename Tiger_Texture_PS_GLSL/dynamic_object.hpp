#pragma once
#include "object.hpp"
#include <vector>

class dynamic_object
	: object
{
public:
	int num_frames;

	std::vector<int>num_triangles;
	std::vector<int>vertex_offset;
	std::vector<GLfloat *>vertices;

	dynamic_object(int num_frames, char* fv, char* ft);
	virtual void prepare(void);
	virtual void draw(void);
};

dynamic_object::dynamic_object(int num_frames, char* fv, char* ft)
	: object(fv, ft)
	, num_frames(num_frames)
	, num_triangles(num_frames)
	, vertex_offset(num_frames)
	, vertices(num_frames)
{

}

inline void dynamic_object::prepare(void)
{
	int wolf_n_total_triangles = 0;
	char filename[512];

	int n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	int n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	for (int i = 0; i < num_frames; i++) {
		sprintf(filename, filename_vertices, i);
		num_triangles[i] = read_geometry(&vertices[i], n_bytes_per_triangle, filename);
		// assume all geometry files are effective
		wolf_n_total_triangles += num_triangles[i];

		if (i == 0)
			vertex_offset[i] = 0;
		else
			vertex_offset[i] = vertex_offset[i - 1] + 3 * num_triangles[i - 1];
	}

	// initialize vertex buffer object
	glGenBuffers(1, &vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, wolf_n_total_triangles*n_bytes_per_triangle, NULL, GL_STATIC_DRAW);

	for (i = 0; i < num_frames; i++)
		glBufferSubData(GL_ARRAY_BUFFER, vertex_offset[i] * n_bytes_per_vertex,
			num_triangles[i] * n_bytes_per_triangle, vertices[i]);

	// as the geometry data exists now in graphics memory, ...
	for (i = 0; i < num_frames; i++)
		free(vertices[i]);

	// initialize vertex array object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//material_wolf.ambient_color[0] = 0.24725f;
	//material_wolf.ambient_color[1] = 0.1995f;
	//material_wolf.ambient_color[2] = 0.0745f;
	//material_wolf.ambient_color[3] = 1.0f;
	//
	//material_wolf.diffuse_color[0] = 0.75164f;
	//material_wolf.diffuse_color[1] = 0.60648f;
	//material_wolf.diffuse_color[2] = 0.22648f;
	//material_wolf.diffuse_color[3] = 1.0f;
	//
	//material_wolf.specular_color[0] = 0.728281f;
	//material_wolf.specular_color[1] = 0.655802f;
	//material_wolf.specular_color[2] = 0.466065f;
	//material_wolf.specular_color[3] = 1.0f;
	//
	//material_wolf.specular_exponent = 51.2f;
	//
	//material_wolf.emissive_color[0] = 0.1f;
	//material_wolf.emissive_color[1] = 0.1f;
	//material_wolf.emissive_color[2] = 0.0f;
	//material_wolf.emissive_color[3] = 1.0f;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glActiveTexture(GL_TEXTURE0 + TEXTURE_ID_TIGER);
	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_ID_TIGER]);

	My_glTexImage2D_from_file(filename_texture);

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

inline void dynamic_object::draw(void)
{
}
