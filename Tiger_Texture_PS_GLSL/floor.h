#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <FreeImage/FreeImage.h>

#include "Shaders/LoadShaders.h"
#include "My_Shading.h"

#include "main.h"


// texture stuffs
extern GLuint texture_names[N_TEXTURES_USED];
extern int flag_texture_mapping;

void prepare_floor(void);

void set_material_floor(void);

void draw_floor(void);

void cleanup_floor(void);