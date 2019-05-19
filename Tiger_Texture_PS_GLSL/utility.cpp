#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <FreeImage/FreeImage.h>
#include <GL/gl.h>


#include <vector>
#include <functional>
#include "utility.h"

// codes for the 'general' triangular-mesh object
//enum OBJ_TYPE { TYPE_V = 0, TYPE_VN, TYPE_VNT };
// GEOM_OBJ_TYPE_V: (x, y, z)
// GEOM_OBJ_TYPE_VN: (x, y, z, nx, ny, nz)
// GEOM_OBJ_TYPE_VNT: (x, y, z, nx, ny, nz, s, t)
//constexpr int elements_per_vertex[3] = { 3, 6, 8 };

int read_geometry_file_binary(GLfloat **object, OBJ_TYPE obj_type, char *filename) {
	int n_triangles;
	FILE *fp;

	// fprintf(stdout, "Reading geometry from the geometry file %s...\n", filename);
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open the object file %s ...", filename);
		return -1;
	}

	fread(&n_triangles, sizeof(int), 1, fp);
	*object = (float *)malloc(n_triangles * 3 * elements_per_vertex[obj_type] * sizeof(float));
	if (*object == NULL) {
		fprintf(stderr, "Cannot allocate memory for the geometry file %s ...", filename);
		return -1;
	}

	fread(*object, 3 * elements_per_vertex[obj_type] * sizeof(float), n_triangles, fp);
	// fprintf(stdout, "Read %d primitives successfully.\n\n", n_triangles);
	fclose(fp);

	return n_triangles;
}

int read_geometry_file_txt(GLfloat **object, OBJ_TYPE obj_type, char *filename) {
	int n_triangles;
	FILE *fp;

	// fprintf(stdout, "Reading geometry from the geometry file %s...\n", filename);
	fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open the object file %s ...", filename);
		return -1;
	}

	fscanf(fp, "%d", &n_triangles);
	*object = (float *)malloc(n_triangles * 3 * elements_per_vertex[obj_type] * sizeof(float));
	if (*object == NULL) {
		fprintf(stderr, "Cannot allocate memory for the geometry file %s ...", filename);
		return -1;
	}

	// fprintf(stdout, "Read %d primitives successfully.\n\n", n_triangles);
	for (size_t i = 0; i < n_triangles * 3 * elements_per_vertex[obj_type]; i++)
	{
		fscanf(fp, "%f", *object + i);
	}

	fclose(fp);

	return n_triangles;
}

glm::vec4 vec3_to_4(const glm::vec3 &  v, int x) {
	return { v.x, v.y, v.z, x };
}


glm::vec3 vec4_to_3(const glm::vec4 &  v) {
	return { v.x, v.y, v.z };
}

glm::mat4 align(glm::vec3 from, glm::vec3 to) {
	auto axis = glm::cross(from, to);
	float angle = acos( glm::dot(from, to) / glm::length(from) / glm::length(to));

	if (glm::length(axis) == 0)
		return glm::mat4(1);

	return glm::rotate(glm::mat4(1), angle, axis);
}


void My_glTexImage2D_from_file(char *filename) {
	FREE_IMAGE_FORMAT tx_file_format;
	int tx_bits_per_pixel;
	FIBITMAP *tx_pixmap, *tx_pixmap_32;

	int width, height;
	GLvoid *data;

	tx_file_format = FreeImage_GetFileType(filename, 0);
	// assume everything is fine with reading texture from file: no error checking
	tx_pixmap = FreeImage_Load(tx_file_format, filename);
	tx_bits_per_pixel = FreeImage_GetBPP(tx_pixmap);

	fprintf(stdout, " * A %d-bit texture was read from %s.\n", tx_bits_per_pixel, filename);
	if (tx_bits_per_pixel == 32)
		tx_pixmap_32 = tx_pixmap;
	else {
		fprintf(stdout, " * Converting texture from %d bits to 32 bits...\n", tx_bits_per_pixel);
		tx_pixmap_32 = FreeImage_ConvertTo32Bits(tx_pixmap);
	}

	width = FreeImage_GetWidth(tx_pixmap_32);
	height = FreeImage_GetHeight(tx_pixmap_32);
	data = FreeImage_GetBits(tx_pixmap_32);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
	fprintf(stdout, " * Loaded %dx%d RGBA texture into graphics memory.\n\n", width, height);

	FreeImage_Unload(tx_pixmap_32);
	if (tx_bits_per_pixel != 32)
		FreeImage_Unload(tx_pixmap);
}

#define ABS(x) abs(abs(t) + x)
#define PI 3.141592

float betman_x(float t) {
	return abs(t) / t * (
		1.0 / 2 * abs(t)
		- 1.0 / 4 * ABS(-1)
		+ 1.0 / 2 * ABS(-3)
		- 3.0 / 4 * ABS(-5)
		- 3.0 / 2 * ABS(-13)
		+ 1.0 / 4 * ABS(-17)
		+ 5.0 / 4 * ABS(-21)
		+ 7 * sin(PI / 12 * (ABS(-5) - ABS(-8) + 3))
		+ 1.0 / 100 * (pow(ABS(-8) - ABS(-13) - 5, 3) + 1.5)
		);
}
float betman_y(float t) {
	return 3.0 / 4 * ABS(-1)
		- 3.0 / 4 * ABS(-3)
		- 7.0 / 5 * ABS(-8)
		+ 7.0 / 5 * ABS(-13)
		+ 7.0 / 16 * pow(ABS(-3) - ABS(-5) - 2, 2)
		+ 4.0 * sin(PI / 12 * (ABS(-5) - ABS(-8) - 3))
		- 5.0 / 16 * pow(ABS(-13) - ABS(-17), 2)
		- 1.0 / 4 * pow(ABS(-17) - ABS(-21) + 2, 2)
		+ 11.5
		;
}