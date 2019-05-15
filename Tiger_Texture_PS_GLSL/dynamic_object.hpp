#pragma once
#include "object.hpp"
#include <vector>

class dynamic_object
	: public object
{
public:
	const int num_frames;
	int cur_frame;

	std::vector<int>num_triangles;
	std::vector<int>vertex_offset;
	std::vector<GLfloat *>vertices;

	dynamic_object(int num_frames, std::string fv, std::string ft);
	virtual void prepare(void);
	virtual void draw(const glm::mat4& ViewMatrix, const glm::mat4& ProjectionMatrix);
};

dynamic_object::dynamic_object(int num_frames, std::string fv, std::string ft = "Data/dynamic_objects/tiger/tiger_tex2.jpg")
	: object(fv, ft)
	, num_frames(num_frames)
	, num_triangles(num_frames)
	, vertex_offset(num_frames)
	, vertices(num_frames)
	, cur_frame(0)
{

}

inline void dynamic_object::prepare(void)
{
	int wolf_n_total_triangles = 0;
	char filename[512];

	int n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	int n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	for (int i = 0; i < num_frames; i++) {
		sprintf(filename, filename_vertices.c_str(), i);
		num_triangles[i] = read_geometry(&vertices[i], obj_type, filename);
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

	for (int i = 0; i < num_frames; i++)
		glBufferSubData(GL_ARRAY_BUFFER, vertex_offset[i] * n_bytes_per_vertex,
			num_triangles[i] * n_bytes_per_triangle, vertices[i]);

	// as the geometry data exists now in graphics memory, ...
	for (int i = 0; i < num_frames; i++)
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

inline void dynamic_object::draw(const glm::mat4& ViewMatrix, const glm::mat4& ProjectionMatrix)
{
	glUniform4fv(loc_material.ambient_color, 1, material.ambient_color);
	glUniform4fv(loc_material.diffuse_color, 1, material.diffuse_color);
	glUniform4fv(loc_material.specular_color, 1, material.specular_color);
	glUniform1f(loc_material.specular_exponent, material.specular_exponent);
	glUniform4fv(loc_material.emissive_color, 1, material.emissive_color);
	// set colors

	glUniform1i(loc_texture, TEXTURE_ID_TIGER);
	auto ModelViewMatrix = ViewMatrix * getModelMatrix();
	auto ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	auto ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);



	glFrontFace(GL_CW);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, vertex_offset[cur_frame], 3 * num_triangles[cur_frame]);
	glBindVertexArray(0);
}
