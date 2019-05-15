#pragma once
#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>
#include <vector>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <FreeImage/FreeImage.h>
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
#include <glm/gtc/matrix_inverse.hpp> // inverseTranspose, etc.


#include "Shaders/LoadShaders.h"
#include "My_Shading.h"

#include "main.h"

extern GLuint texture_names[N_TEXTURES_USED];
constexpr int MAX_FILENAME = 512;
constexpr Material_Parameters tiger_material = {
	{ 0.24725f, 0.1995f, 0.0745f, 1.0f },
	{ 0.75164f, 0.60648f, 0.22648f, 1.0f },
	{ 0.728281f, 0.655802f, 0.466065f, 1.0f },
	{ 0.1f, 0.1f, 0.0f, 1.0f },
	51.2f
};

extern loc_Material_Parameters loc_material;
extern GLint loc_ModelViewProjectionMatrix_TXPS, loc_ModelViewMatrix_TXPS, loc_ModelViewMatrixInvTrans_TXPS;
extern GLint loc_texture, loc_flag_texture_mapping, loc_flag_fog;

// codes for the 'general' triangular-mesh object
enum GEOM_OBJ_TYPE { GEOM_OBJ_TYPE_V = 0, GEOM_OBJ_TYPE_VN, GEOM_OBJ_TYPE_VNT };
// GEOM_OBJ_TYPE_V: (x, y, z)
// GEOM_OBJ_TYPE_VN: (x, y, z, nx, ny, nz)
// GEOM_OBJ_TYPE_VNT: (x, y, z, nx, ny, nz, s, t)
int GEOM_OBJ_ELEMENTS_PER_VERTEX[3] = { 3, 6, 8 };

int read_geometry(GLfloat **object, GEOM_OBJ_TYPE obj_type, char *filename) {
	int n_triangles;
	FILE *fp;

	// fprintf(stdout, "Reading geometry from the geometry file %s...\n", filename);
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open the object file %s ...", filename);
		return -1;
	}

	fread(&n_triangles, sizeof(int), 1, fp);
	*object = (float *)malloc(n_triangles * 3 * GEOM_OBJ_ELEMENTS_PER_VERTEX[obj_type] * sizeof(float));
	if (*object == NULL) {
		fprintf(stderr, "Cannot allocate memory for the geometry file %s ...", filename);
		return -1;
	}

	fread(*object, 3 * GEOM_OBJ_ELEMENTS_PER_VERTEX[obj_type] * sizeof(float), n_triangles, fp);
	// fprintf(stdout, "Read %d primitives successfully.\n\n", n_triangles);
	fclose(fp);

	return n_triangles;
}


class object
{
private:
	GLfloat *vertices;
	int num_triangles;

public:
	GLuint vbo, vao;
	Material_Parameters material;
	GEOM_OBJ_TYPE obj_type;

	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 scale;
	glm::vec3 rotate;

	std::string name;
	std::string filename_vertices;
	std::string filename_texture;

	object(std::string fv, std::string ft, glm::vec3 position, Material_Parameters material);
	virtual void prepare(void);
	virtual void draw(const glm::mat4& ViewMatrix, const glm::mat4& ProjectionMatrix);


	glm::mat4 getModelMatrix();
	glm::mat4 getModelViewMatrix(glm::mat4 ViewMatrix);
};

object::object
(
	std::string fv
	, std::string ft = "Data/dynamic_objects/tiger/tiger_tex2.jpg"
	, glm::vec3 position = glm::vec3(0.0f)
	, Material_Parameters material = tiger_material
)
	: filename_vertices(fv)
	, filename_texture(ft)
	, position(position)
	, velocity(0)
	, acceleration(0)
	, scale(1)
	, rotate(0)
	, material(material)
	, obj_type(GEOM_OBJ_TYPE_VNT)
{

}

inline void object::prepare(void)
{
	int num_bytes_per_vertex, num_bytes_per_triangle, num_total_triangles = 0;

	num_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	num_bytes_per_triangle = 3 * num_bytes_per_vertex;

	num_triangles = read_geometry(&vertices, obj_type, &filename_vertices[0]);
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

inline void object::draw(const glm::mat4& ViewMatrix, const glm::mat4& ProjectionMatrix) {
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

	return ModelViewMatrix;
}
