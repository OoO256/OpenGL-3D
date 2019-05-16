#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <FreeImage/FreeImage.h>
#include <GL/gl.h>

#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
#include <glm/gtc/matrix_inverse.hpp> // inverseTranspose, etc.
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