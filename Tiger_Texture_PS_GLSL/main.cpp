#include <vector>

#include "main.h"
#include "axes.h"
#include "floor.h"
#include "object.hpp"
#include "carmera.hpp"
#include "utility.h"

#define _CRT_SECURE_NO_WARNINGS

#include "Shaders/LoadShaders.h"
#include "My_Shading.h"
GLuint h_ShaderProgram_simple, h_ShaderProgram_TXPS; 
// handles to shader programs

// for simple shaders
GLint loc_ModelViewProjectionMatrix_simple, loc_primitive_color;

// for Phong Shading (Textured) shaders
#define NUMBER_OF_LIGHT_SUPPORTED 4 
GLint loc_global_ambient_color;
loc_light_Parameters loc_light[NUMBER_OF_LIGHT_SUPPORTED];
loc_Material_Parameters loc_material;
GLint loc_ModelViewProjectionMatrix_TXPS, loc_ModelViewMatrix_TXPS, loc_ModelViewMatrixInvTrans_TXPS;
GLint loc_texture, loc_flag_texture_mapping, loc_flag_fog;

// include glm/*.hpp only if necessary
//#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
#include <glm/gtc/matrix_inverse.hpp> // inverseTranspose, etc.
glm::mat4 ModelViewProjectionMatrix, ModelViewMatrix;
glm::mat3 ModelViewMatrixInvTrans;
glm::mat4 ViewMatrix, ProjectionMatrix;

// lights in scene
Light_Parameters light[NUMBER_OF_LIGHT_SUPPORTED];

// texture stuffs
GLuint texture_names[N_TEXTURES_USED];
int flag_texture_mapping;

// codes for the 'general' triangular-mesh object
//extern enum OBJ_TYPE { TYPE_V = 0, TYPE_VN, TYPE_VNT };
// GEOM_OBJ_TYPE_V: (x, y, z)
// GEOM_OBJ_TYPE_VN: (x, y, z, nx, ny, nz)
// GEOM_OBJ_TYPE_VNT: (x, y, z, nx, ny, nz, s, t)
//extern constexpr int elements_per_vertex[3] = { 3, 6, 8 };

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

// fog stuffs
// you could control the fog parameters interactively: FOG_COLOR, FOG_NEAR_DISTANCE, FOG_FAR_DISTANCE   
int flag_fog;

// for tiger animation
unsigned int timestamp_scene = 0; // the global clock in the scene
int is_time_running, flag_polygon_fill;
float rotation_angle_tiger = 0.0f;

// my object
std::vector<object *>objects;
object optimus(1, "Data/static_objects/optimus_vnt.geom", TYPE_VNT);
object cow(1, "Data/static_objects/cow_vn.geom", TYPE_VNT);
object bike(1, "Data/static_objects/bike_vnt.geom", TYPE_VNT);
object bus(1, "Data/static_objects/bus_vnt.geom", TYPE_VNT);
object godzilla(1, "Data/static_objects/godzilla_vnt.geom", TYPE_VNT);
object ironman(1, "Data/static_objects/ironman_vnt.geom", TYPE_VNT);
object tank(1, "Data/static_objects/tank_vnt.geom", TYPE_VNT);

object tiger(12, "Data/dynamic_objects/tiger/Tiger_%02d_triangles_vnt.geom", TYPE_VNT);
object ben(30, "Data/dynamic_objects/ben/ben_vn%02d.geom", TYPE_VNT);
object wolf(17, "Data/dynamic_objects/wolf/wolf_%02d_vnt.geom", TYPE_VNT);
object spider(16, "Data/dynamic_objects/spider/spider_vnt_%02d.geom", TYPE_VNT);


object cow2(1, "Data/static_objects/txtdata/cow_triangles_v.txt", TYPE_V);

// callbacks
float PRP_distance_scale[6] = { 0.5f, 1.0f, 2.5f, 5.0f, 10.0f, 20.0f };

carmera cam1(1000, 1000, 1000);
carmera cam2(1000, 1000, 1000);
carmera* cur_cam = &cam1;

void display(void) {
	cam2.move(ben.position + glm::vec3{0, 0, 50});
	cam2.center = ben.position + glm::normalize(ben.velocity);

	ViewMatrix = cur_cam->getView();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(h_ShaderProgram_simple);
	ModelViewMatrix = glm::scale(ViewMatrix, glm::vec3(50.0f, 50.0f, 50.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_simple, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glLineWidth(2.0f);
	draw_axes();
	glLineWidth(1.0f);	

	glUseProgram(h_ShaderProgram_simple);
	ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(20.0f, 20.0f, 20.0f));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_simple, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_axes();


	glUseProgram(h_ShaderProgram_TXPS);
	set_material_floor();
	glUniform1i(loc_texture, TEXTURE_ID_FLOOR);
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-500.0f, 0.0f, 500.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(1000.0f, 1000.0f, 1000.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_floor();

	glUseProgram(h_ShaderProgram_TXPS);
	for (auto& obj : objects)
	{		
		obj->draw(ViewMatrix, ProjectionMatrix);
	}

	glUseProgram(0);

	glutSwapBuffers();
}

void timer_scene(int value) {
	timestamp_scene = (timestamp_scene + 1) % UINT_MAX;
	
	if (timestamp_scene % 10 == 0) {
		for (auto& obj : objects)
		{
			obj->next_frame();
		}
	}

	glutPostRedisplay();
	if (is_time_running)
		glutTimerFunc(10, timer_scene, 0);
}


void keyboard(unsigned char key, int x, int y) {
	printf("key : %d\n", key);

	static int flag_cull_face = 0;
	static int PRP_distance_level = 4;

	glm::vec4 position_EC;
	glm::vec3 direction_EC;

	if ((key >= '0') && (key <= '0' + NUMBER_OF_LIGHT_SUPPORTED - 1)) {
		int light_ID = (int)(key - '0');

		glUseProgram(h_ShaderProgram_TXPS);
		light[light_ID].light_on = 1 - light[light_ID].light_on;
		glUniform1i(loc_light[light_ID].light_on, light[light_ID].light_on);
		glUseProgram(0);

		glutPostRedisplay();
		return;
	}

	switch (key) {
	case 'a':
		ben.turn_left(5 * TO_RADIAN);
		ben.move_forward(5);
		break;
	case 's':
		ben.move_forward(-5);
		break;
	case 'd':
		ben.turn_left(-5 * TO_RADIAN);
		ben.move_forward(5);
		break;
	case 'w':
		ben.move_forward(5);
		break;
	case 'q':
		cur_cam = &cam2;
		break; 
	case 'e':
		cur_cam = &cam1;
		break;
	case 'f':
		flag_fog = 1 - flag_fog;
		glUseProgram(h_ShaderProgram_TXPS);
		glUniform1i(loc_flag_fog, flag_fog);
		glUseProgram(0);
		glutPostRedisplay();
		break;
	case 't':
		flag_texture_mapping = 1 - flag_texture_mapping;
		glUseProgram(h_ShaderProgram_TXPS);
		glUniform1i(loc_flag_texture_mapping, flag_texture_mapping);
		glUseProgram(0);
		glutPostRedisplay();
		break;
	case 'c':
		flag_cull_face = (flag_cull_face + 1) % 3;
		switch (flag_cull_face) {
		case 0:
			glDisable(GL_CULL_FACE);
			glutPostRedisplay();
			break;
		case 1: // cull back faces;
			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);
			glutPostRedisplay();
			break;
		case 2: // cull front faces;
			glCullFace(GL_FRONT);
			glEnable(GL_CULL_FACE);
			glutPostRedisplay();
			break;
		}
		break;
	case 'p':
		flag_polygon_fill = 1 - flag_polygon_fill;
		if (flag_polygon_fill)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glutPostRedisplay();
		break;
	case 44:
		cam1.move(-10, 0, 0);
		break;
	case 46:
		cam1.move(10, 0, 0);
		break;
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups
		break;
	}
}

void special_keyboard(int key, int x, int y) {
	printf("special key entered : %d\n", key);

	switch (key)
	{
	case GLUT_KEY_UP:
		//do something here
		cam1.move(0, 0, 0.1);
		break;
	case GLUT_KEY_DOWN:
		//do something here
		cam1.move(0, 0, -0.1);
		break;
	case GLUT_KEY_LEFT:
		//do something here
		cam1.move(0, 0.1, 0);
		break;
	case GLUT_KEY_RIGHT:
		//do something here
		cam1.move(0, -0.1, 0);
		break;
	default:
		break;
	}
}

void reshape(int width, int height) {
	float aspect_ratio;

	glViewport(0, 0, width, height);
	
	aspect_ratio = (float) width / height;
	ProjectionMatrix = glm::perspective(45.0f*TO_RADIAN, aspect_ratio, 100.0f, 20000.0f);

	glutPostRedisplay();
}

void cleanup(void) {
	cleanup_axes();

	cleanup_floor();

	glDeleteTextures(N_TEXTURES_USED, texture_names);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutTimerFunc(100, timer_scene, 0);
	glutCloseFunc(cleanup);

	glutSpecialFunc(special_keyboard);
}

void prepare_shader_program(void) {
	int i;
	char string[256];
	ShaderInfo shader_info_simple[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};
	ShaderInfo shader_info_TXPS[3] = {
		{ GL_VERTEX_SHADER, "Shaders/Phong_Tx.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/Phong_Tx.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram_simple = LoadShaders(shader_info_simple);
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram_simple, "u_primitive_color");
	loc_ModelViewProjectionMatrix_simple = glGetUniformLocation(h_ShaderProgram_simple, "u_ModelViewProjectionMatrix");

	h_ShaderProgram_TXPS = LoadShaders(shader_info_TXPS);
	loc_ModelViewProjectionMatrix_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewMatrixInvTrans");

	loc_global_ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_global_ambient_color");
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		sprintf(string, "u_light[%d].light_on", i);
		loc_light[i].light_on = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].position", i);
		loc_light[i].position = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].ambient_color", i);
		loc_light[i].ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].diffuse_color", i);
		loc_light[i].diffuse_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].specular_color", i);
		loc_light[i].specular_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_direction", i);
		loc_light[i].spot_direction = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_exponent", i);
		loc_light[i].spot_exponent = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_cutoff_angle", i);
		loc_light[i].spot_cutoff_angle = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].light_attenuation_factors", i);
		loc_light[i].light_attenuation_factors = glGetUniformLocation(h_ShaderProgram_TXPS, string);
	}

	loc_material.ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.ambient_color");
	loc_material.diffuse_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.diffuse_color");
	loc_material.specular_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.specular_color");
	loc_material.emissive_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.emissive_color");
	loc_material.specular_exponent = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.specular_exponent");

	loc_texture = glGetUniformLocation(h_ShaderProgram_TXPS, "u_base_texture");

	loc_flag_texture_mapping = glGetUniformLocation(h_ShaderProgram_TXPS, "u_flag_texture_mapping");
	loc_flag_fog = glGetUniformLocation(h_ShaderProgram_TXPS, "u_flag_fog");
}

void initialize_lights_and_material(void) { // follow OpenGL conventions for initialization
	int i;

	glUseProgram(h_ShaderProgram_TXPS);

	glUniform4f(loc_global_ambient_color, 0.115f, 0.115f, 0.115f, 1.0f);
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		glUniform1i(loc_light[i].light_on, 0); // turn off all lights initially
		glUniform4f(loc_light[i].position, 0.0f, 0.0f, 1.0f, 0.0f);
		glUniform4f(loc_light[i].ambient_color, 0.0f, 0.0f, 0.0f, 1.0f);
		if (i == 0) {
			glUniform4f(loc_light[i].diffuse_color, 1.0f, 1.0f, 1.0f, 1.0f);
			glUniform4f(loc_light[i].specular_color, 1.0f, 1.0f, 1.0f, 1.0f);
		}
		else {
			glUniform4f(loc_light[i].diffuse_color, 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(loc_light[i].specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
		}
		glUniform3f(loc_light[i].spot_direction, 0.0f, 0.0f, -1.0f);
		glUniform1f(loc_light[i].spot_exponent, 0.0f); // [0.0, 128.0]
		glUniform1f(loc_light[i].spot_cutoff_angle, 180.0f); // [0.0, 90.0] or 180.0 (180.0 for no spot light effect)
		glUniform4f(loc_light[i].light_attenuation_factors, 1.0f, 0.0f, 0.0f, 0.0f); // .w != 0.0f for no ligth attenuation
	}

	glUniform4f(loc_material.ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);
	glUniform4f(loc_material.diffuse_color, 0.8f, 0.8f, 0.8f, 1.0f);
	glUniform4f(loc_material.specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform4f(loc_material.emissive_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform1f(loc_material.specular_exponent, 0.0f); // [0.0, 128.0]

	glUseProgram(0);
}

void initialize_flags(void) {
	is_time_running = 1;
	flag_polygon_fill = 1;
	flag_texture_mapping = 1;
	flag_fog = 0;

	glUseProgram(h_ShaderProgram_TXPS);
	glUniform1i(loc_flag_fog, flag_fog);
	glUniform1i(loc_flag_texture_mapping, flag_texture_mapping);
	glUseProgram(0);
}

void initialize_OpenGL(void) {

	glEnable(GL_MULTISAMPLE);


  	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//ViewMatrix = glm::lookAt(PRP_distance_scale[0] * glm::vec3(500.0f, 300.0f, 500.0f),
	//	glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ViewMatrix = glm::lookAt(4.0f/6.0f * glm::vec3(500.0f, 600.0f, 500.0f),
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	initialize_lights_and_material();
	initialize_flags();

	glGenTextures(N_TEXTURES_USED, texture_names);
}

void set_up_scene_lights(void) {
	// point_light_EC: use light 0
	light[0].light_on = 1;
	light[0].position[0] = 0.0f; light[0].position[1] = 100.0f; 	// point light position in EC
	light[0].position[2] = 0.0f; light[0].position[3] = 1.0f;

	light[0].ambient_color[0] = 0.13f; light[0].ambient_color[1] = 0.13f;
	light[0].ambient_color[2] = 0.13f; light[0].ambient_color[3] = 1.0f;

	light[0].diffuse_color[0] = 0.5f; light[0].diffuse_color[1] = 0.5f;
	light[0].diffuse_color[2] = 0.5f; light[0].diffuse_color[3] = 1.5f;

	light[0].specular_color[0] = 0.8f; light[0].specular_color[1] = 0.8f;
	light[0].specular_color[2] = 0.8f; light[0].specular_color[3] = 1.0f;

	// spot_light_WC: use light 1
	light[1].light_on = 1;
	light[1].position[0] = -200.0f; light[1].position[1] = 500.0f; // spot light position in WC
	light[1].position[2] = -200.0f; light[1].position[3] = 1.0f;

	light[1].ambient_color[0] = 0.152f; light[1].ambient_color[1] = 0.152f;
	light[1].ambient_color[2] = 0.152f; light[1].ambient_color[3] = 1.0f;

	light[1].diffuse_color[0] = 0.572f; light[1].diffuse_color[1] = 0.572f;
	light[1].diffuse_color[2] = 0.572f; light[1].diffuse_color[3] = 1.0f;

	light[1].specular_color[0] = 0.772f; light[1].specular_color[1] = 0.772f;
	light[1].specular_color[2] = 0.772f; light[1].specular_color[3] = 1.0f;

	light[1].spot_direction[0] = 0.0f; light[1].spot_direction[1] = -1.0f; // spot light direction in WC
	light[1].spot_direction[2] = 0.0f;
	light[1].spot_cutoff_angle = 20.0f;
	light[1].spot_exponent = 8.0f;

	glUseProgram(h_ShaderProgram_TXPS);
	glUniform1i(loc_light[0].light_on, light[0].light_on);
	glUniform4fv(loc_light[0].position, 1, light[0].position);
	glUniform4fv(loc_light[0].ambient_color, 1, light[0].ambient_color);
	glUniform4fv(loc_light[0].diffuse_color, 1, light[0].diffuse_color);
	glUniform4fv(loc_light[0].specular_color, 1, light[0].specular_color);

	glUniform1i(loc_light[1].light_on, light[1].light_on);
	// need to supply position in EC for shading
	glm::vec4 position_EC = ViewMatrix * glm::vec4(light[1].position[0], light[1].position[1],
												light[1].position[2], light[1].position[3]);
	glUniform4fv(loc_light[1].position, 1, &position_EC[0]); 
	glUniform4fv(loc_light[1].ambient_color, 1, light[1].ambient_color);
	glUniform4fv(loc_light[1].diffuse_color, 1, light[1].diffuse_color);
	glUniform4fv(loc_light[1].specular_color, 1, light[1].specular_color);
	// need to supply direction in EC for shading in this example shader
	// note that the viewing transform is a rigid body transform
	// thus transpose(inverse(mat3(ViewMatrix)) = mat3(ViewMatrix)
	glm::vec3 direction_EC = glm::mat3(ViewMatrix) * glm::vec3(light[1].spot_direction[0], light[1].spot_direction[1], 
																light[1].spot_direction[2]);
	glUniform3fv(loc_light[1].spot_direction, 1, &direction_EC[0]); 
	glUniform1f(loc_light[1].spot_cutoff_angle, light[1].spot_cutoff_angle);
	glUniform1f(loc_light[1].spot_exponent, light[1].spot_exponent);
	glUseProgram(0);
}

void init_objects(void) {
	tiger.original_dir = { 0, -1, 0 };
	//optimus.original_dir = { 1, 0, 0 };
	cow.original_dir = { 1, 0, 0 };
	cow.position.y += 22;
	tank.original_dir = { 0, -1, 0 };


	ben.scale = glm::vec3(100.0f, -100.0f, -100.0f);
	wolf.scale = glm::vec3(100.0f, 100.0f, 100.0f);
	spider.scale = glm::vec3(50.0f, -50.0f, 50.0f);
	optimus.scale = glm::vec3(0.1f, 0.1f, 0.1f);
	cow.scale = glm::vec3(80.0f, 80.0f, 80.0f);

	bike.scale = glm::vec3(20.0f, 20.0f, 20.0f);
	bus.scale = glm::vec3(3.0f, 3.0f, 3.0f);
	godzilla.scale = glm::vec3(0.5f, 0.5f, 0.5f);
	ironman.scale = glm::vec3(20.0f, 20.0f, 20.0f);
	tank.scale = glm::vec3(10.0f, 10.0f, 10.0f);
	tank.rotate = glm::vec3(-90.0f*TO_RADIAN, 0, 0);

	cow2.position = { 0, 200, 0 };
	cow2.scale = glm::vec3(100, 100, 100);
	cow2.type = TYPE_V;
	cow2.is_binary_file = false;
	

	objects.emplace_back(&tiger);
	objects.emplace_back(&ben);
	objects.emplace_back(&wolf);
	objects.emplace_back(&spider);

	objects.emplace_back(&optimus);
	objects.emplace_back(&cow);
	objects.emplace_back(&bike);
	objects.emplace_back(&bus);
	objects.emplace_back(&godzilla);
	objects.emplace_back(&ironman);
	objects.emplace_back(&tank);


	objects.emplace_back(&cow2);


	int obj_cnt = 0;
	for (auto& obj : objects) {
		obj->position.x = 300.0f * cos(TO_RADIAN * 30 * obj_cnt);
		obj->position.z = 300.0f * sin(TO_RADIAN * 30 * obj_cnt);
		obj_cnt++;
	}
}

void prepare_scene(void) {
	init_objects();

	prepare_axes();
	prepare_floor();

	for (auto& obj : objects)
	{
		obj->prepare();
	}

	set_up_scene_lights();
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	prepare_scene();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 1
void main(int argc, char *argv[]) {
	char program_name[64] = "Sogang CSE4170 3D Objects";
	char messages[N_MESSAGE_LINES][256] = { "    - Keys used: '0', '1', 'a', 't', 'f', 'c', 'd', 'y', 'u', 'i', 'o', 'ESC'"  };

	glutInit(&argc, argv);
  	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(800, 800);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}
