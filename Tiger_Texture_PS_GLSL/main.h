#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <FreeImage/FreeImage.h>
#include <GL/gl.h>

constexpr float TO_RADIAN = 0.01745329252f;
constexpr float TO_DEGREE = 57.295779513f;


#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

#define LOC_VERTEX 0
#define LOC_NORMAL 1
#define LOC_TEXCOORD 2

// texture stuffs
constexpr int N_TEXTURES_USED = 2;
constexpr int TEXTURE_ID_FLOOR = 0;
constexpr int TEXTURE_ID_TIGER = 1;

void My_glTexImage2D_from_file(char *filename);