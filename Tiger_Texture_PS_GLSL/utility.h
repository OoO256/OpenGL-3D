#pragma once
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
#include <glm/gtc/matrix_inverse.hpp> // inverseTranspose, etc.

// codes for the 'general' triangular-mesh object
enum OBJ_TYPE { TYPE_V = 0, TYPE_VN, TYPE_VNT };
// GEOM_OBJ_TYPE_V: (x, y, z)
// GEOM_OBJ_TYPE_VN: (x, y, z, nx, ny, nz)
// GEOM_OBJ_TYPE_VNT: (x, y, z, nx, ny, nz, s, t)
constexpr int elements_per_vertex[3] = { 3, 6, 8 };

int read_geometry_file_binary(GLfloat **object, OBJ_TYPE obj_type, char *filename);

int read_geometry_file_txt(GLfloat **object, OBJ_TYPE obj_type, char *filename);

glm::vec4 vec3_to_4(const glm::vec3 &  v, int x);

glm::vec3 vec4_to_3(const glm::vec4 & v);

glm::mat4 align(glm::vec3 from, glm::vec3 to);

void My_glTexImage2D_from_file(char *filename);

float betman_x(float t);

float betman_y(float t);