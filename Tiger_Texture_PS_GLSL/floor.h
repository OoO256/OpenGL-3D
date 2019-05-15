#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <FreeImage/FreeImage.h>

#include "Shaders/LoadShaders.h"
#include "My_Shading.h"

#define LOC_VERTEX 0
#define LOC_NORMAL 1
#define LOC_TEXCOORD 2

// texture stuffs
#define N_TEXTURES_USED 2
#define TEXTURE_ID_FLOOR 0
#define TEXTURE_ID_TIGER 1

// texture stuffs
extern GLuint texture_names[N_TEXTURES_USED];
extern int flag_texture_mapping;

void prepare_floor(void);

void set_material_floor(void);

void draw_floor(void);

void cleanup_floor(void);