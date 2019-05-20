#include <vector>
#include <iostream>
#include <algorithm>

#include "main.h"
#include "axes.h"
#include "floor.h"
#include "object.h"
#include "carmera.h"
#include "utility.h"
#include "car.h"
#include "keyboard.h"
//#include "light.h"

#define _CRT_SECURE_NO_WARNINGS
#define PI 3.141592

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

//std::vector<Light>lights;

// texture stuffs
GLuint texture_names[N_TEXTURES_USED];
int flag_texture_mapping;

// codes for the 'general' triangular-mesh object
//extern enum OBJ_TYPE { TYPE_V = 0, TYPE_VN, TYPE_VNT };
// GEOM_OBJ_TYPE_V: (x, y, z)
// GEOM_OBJ_TYPE_VN: (x, y, z, nx, ny, nz)
// GEOM_OBJ_TYPE_VNT: (x, y, z, nx, ny, nz, s, t)
//extern constexpr int elements_per_vertex[3] = { 3, 6, 8 };


// fog stuffs
// you could control the fog parameters interactively: FOG_COLOR, FOG_NEAR_DISTANCE, FOG_FAR_DISTANCE   
int flag_fog;

// for tiger animation
unsigned int timestamp_scene = 0; // the global clock in the scene
int is_time_running, flag_polygon_fill;
float rotation_angle_tiger = 0.0f;

// my object
std::vector<object *>objects;
//object optimus(1, "Data/static_objects/optimus_vnt.geom", TYPE_VNT);
//object cow(1, "Data/static_objects/cow_vn.geom", TYPE_VNT);
object bike(1, "Data/static_objects/bike_vnt.geom", TYPE_VNT);
//object bus(1, "Data/static_objects/bus_vnt.geom", TYPE_VNT);
//object godzilla(1, "Data/static_objects/godzilla_vnt.geom", TYPE_VNT);
object ironman(1, "Data/static_objects/ironman_vnt.geom", TYPE_VNT);
object tiger(12, "Data/dynamic_objects/tiger/Tiger_%02d_triangles_vnt.geom", TYPE_VNT);
object ben(30, "Data/dynamic_objects/ben/ben_vn%02d.geom", TYPE_VNT);
object wolf(17, "Data/dynamic_objects/wolf/wolf_%02d_vnt.geom", TYPE_VNT);
object spider(16, "Data/dynamic_objects/spider/spider_vnt_%02d.geom", TYPE_VNT);


object cow2(1, "Data/static_objects/txtdata/cow_triangles_v.txt", TYPE_V);
//object body(1, "Data/static_objects/txtdata/car_body_triangles_v.txt", TYPE_V);
car* car1;

object* slected;

std::vector<carmera>cams;
carmera* cur_cam;

//float fovy = 45.0f;
float aspect_ratio;
bool togle[3] = {true, true, true};

void display(carmera* cam) {
	//cams[1].move(slected->position + glm::vec3{ 0, 200, 0 } -glm::normalize(slected->velocity) * 100.0f);
	//cams[1].center = slected->position + glm::normalize(slected->velocity) * 100.0f;	

	ViewMatrix = cam->getView();
	ProjectionMatrix = cam->getProj();
	set_up_scene_lights();

	glClearColor(0x8C / 225.0, 0xC4 / 225.0, 0xCD / 225.0, 0.5);

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
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-1500.0f, 0.0f, 1500.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(3000.0f, 3000.0f, 3000.0f));
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

	
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	
	glClearColor(0 / 225.0, 0 / 225.0, 0 / 225.0, 0.5);
	glViewport(0, 0, 800, 800);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(&cams[0]);

	if (togle[0]) {
		glViewport(0, 800, 200, 200);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		display(&cams[1]);
	}

	cams[2].move(car1->body->position + glm::vec3(-3.0f, 0.5f, 2.5f));
	cams[2].center = cams[2].pos + glm::normalize(car1->body->velocity) * 100.0f;

	if (togle[1]) {
		glViewport(200, 800, 200, 200);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		display(&cams[2]);
	}

	cams[3].move(tiger.position + glm::vec3(0, 30, 0));
	cams[3].center = cams[3].pos + glm::normalize(tiger.velocity) * 10.0f;

	if (togle[2]) {
		glViewport(400, 800, 200, 200);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		display(&cams[3]);
	}
	
	glutSwapBuffers();
}

void timer_scene(int value) {
	timestamp_scene = (timestamp_scene + 1) % UINT_MAX;

	mykeyboard.action();
	

	if (glm::distance(ben.position, spider.position) < 50) {
		float e = 0.9;

		glm::vec3 v1 = ben.velocity;
		glm::vec3 v2 = spider.velocity;

		float m1 = 1;
		float m2 = 10;

		ben.velocity = v1 - m2 * (1 + e) / (m1 + m2) * (v1 - v2);
		spider.velocity = v2 + m1 * (1 + e) / (m1 + m2) * (v1 - v2);

		ben.velocity.y += 200;
	}



	if (timestamp_scene % 10 == 0) {
		for (auto& obj : objects)
		{
			obj->next_frame();
		}
	}
	else
	{
		ben.next_frame();
	}
	
	ben.updata_pos();

	auto ben_foward = ben.velocity;
	ben_foward.y = 0;
	if (ben.rotate.empty())
		ben.rotate.push_front(glm::mat4(1));

	ben.rotate.front() = align({ 0, 0, 1 }, ben_foward);

	if (mykeyboard.key_state['t']) {

		spider.acceleration = glm::normalize(ben.position - spider.position) * 1000.0f;
		spider.updata_pos();

		if (spider.rotate.empty())
			spider.rotate.push_front(glm::mat4(1));

		spider.rotate.front() = align({ 0, 0, 1 }, spider.velocity);
	}
	else
	{
		spider.timestamp_last = timestamp_scene;
	}


	ironman.updata_pos();
	if (ironman.rotate.empty())
		ironman.rotate.push_front(glm::mat4(1));
	auto ironman_foward = ironman.velocity;
	ironman_foward.y = 0;

	ironman.rotate.front() = align({ 0, 0, 1 }, ironman_foward);

	float radius_heart = 20.0f;
	float theta = timestamp_scene * 0.01;

	float i = theta;

	while (i > 21)
	{
		i -= 42;
	}
	tiger.position = glm::vec3(
		betman_x(i) * 25
		, sin(tiger.cur_frame/5.5*3.141592*2)*6 + 6
		, betman_y(i) * 25
	);

	tiger.velocity = glm::vec3(
		(betman_x(i + 0.01) - betman_x(i)) /0.01
		, 0
		, (betman_y(i + 0.01) - betman_y(i)) /0.01
	);

	tiger.rotate[0] = align({ 0, 0, 1 }, tiger.velocity);



	car1->body->position = glm::vec3(
		16 * pow(sin(theta), 3) * radius_heart
		, car1->body->position.y
		, (13 * cos(theta) - 5 * cos(2 * theta) - 2 * cos(3 * theta) - cos(4 * theta))*radius_heart
	);

	car1->body->velocity = glm::vec3(
		16 * 3 * pow(sin(theta), 2) * cos(theta) * radius_heart
		, 0
		, (-13 * sin(theta) + 10 * sin(2 * theta) + 6 * sin(3 * theta) + 4 * sin(4 * theta))*radius_heart
	);

	if (car1->body->rotate.size() != 1)
		car1->body->rotate.pop_front();
	car1->body->rotate.push_front(align({ 0, 0, 1 }, glm::normalize(car1->body->velocity)));



	for (auto& w : car1->wheels)
	{	
		w->rotate.pop_front();
		w->rotate.front() *= align({ 1, 0, 0 }, { cos(-5 * TO_RADIAN), sin(-5 * TO_RADIAN), 0 });
	}
	   	 
	auto v2 = glm::vec3(
		16 * 3 * pow(sin(theta + 0.1), 2) * cos(theta + 0.1)
		, 0
		, (-13 * sin(theta + 0.1) + 10 * sin(2 * (theta + 0.1)) + 6 * sin(3 * (theta + 0.1)) + 4 * sin(4 * (theta + 0.1)))
	);

	auto v1 = glm::vec3(
		16 * 3 * pow(sin(theta), 2) * cos(theta)
		, 0
		, (-13 * sin(theta) + 10 * sin(2 * theta) + 6 * sin(3 * theta) + 4 * sin(4 * theta))
	);

	float d = acos(glm::dot(v1, v2) / glm::length(v1) / glm::length(v2));
	for (auto& w : car1->wheels)
	{
		w->rotate.push_front(
			glm::rotate(glm::mat4(1), d*2, { 0, 1, 0 })
		);
	}

	glutPostRedisplay();
	if (is_time_running)
		glutTimerFunc(10, timer_scene, 0);
}



void reshape(int width, int height) {
	

	glViewport(0, 0, width, height);
	
	aspect_ratio = (float) width / height;
	glutPostRedisplay();
}

void cleanup(void) {
	cleanup_axes();

	cleanup_floor();

	glDeleteTextures(N_TEXTURES_USED, texture_names);
}



void register_callbacks(void) {
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutTimerFunc(100, timer_scene, 0);
	glutCloseFunc(cleanup);


	glutIgnoreKeyRepeat(true);
	glutKeyboardFunc([](unsigned char key, int x, int y) { mykeyboard.key_down(key, x, y); });
	glutKeyboardUpFunc([](unsigned char key, int x, int y) { mykeyboard.key_up(key, x, y); });
	glutSpecialFunc([](int key, int x, int y) { mykeyboard.special_down(key, x, y); });
	glutSpecialUpFunc([](int key, int x, int y) { mykeyboard.special_up(key, x, y); });

	glutMotionFunc([](int x, int y) { 
		mykeyboard.motion(x, y, glutGetModifiers()); 
		//ProjectionMatrix = glm::perspective(fovy*TO_RADIAN, aspect_ratio, 100.0f, 20000.0f);
	});

	glutPassiveMotionFunc([](int x, int y) {mykeyboard.last_mouse_x = -1; mykeyboard.last_mouse_y = -1; });
	
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

	light[0].ambient_color[0] = 0.3f; light[0].ambient_color[1] = 0.3f;
	light[0].ambient_color[2] = 0.3f; light[0].ambient_color[3] = 1.0f;

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

	// spot_light_WC: use light 2
	auto f = ironman.velocity;
	f.y = 0;

	auto r = glm::cross(glm::normalize(f), { 0, 1, 0 });
	auto p1 = ironman.position + r * 35.0f + glm::vec3(0, 70, 0); 
	auto p2 = ironman.position - r * 35.0f + glm::vec3(0, 70, 0);

	auto l1 = glm::vec3(0, -1, 0) + glm::normalize(r)*0.3f;
	auto l2 = glm::vec3(0, -1, 0) + glm::normalize(r)*-0.3f;


	light[2].light_on = 1;
	light[2].position[0] = p1.x; light[2].position[1] = p1.y; // spot light position in WC
	light[2].position[2] = p1.z; light[2].position[3] = 1.0f;

	light[2].ambient_color[0] = 1; light[2].ambient_color[1] = 0;
	light[2].ambient_color[2] = 0; light[2].ambient_color[3] = 1.0f;

	light[2].diffuse_color[0] = 1; light[2].diffuse_color[1] = 0;
	light[2].diffuse_color[2] = 0; light[2].diffuse_color[3] = 1.0f;

	light[2].specular_color[0] = 1; light[2].specular_color[1] = 0;
	light[2].specular_color[2] = 0; light[2].specular_color[3] = 1.0f;

	light[2].spot_direction[0] = l1.x; light[2].spot_direction[1] = l1.y; // spot light direction in WC
	light[2].spot_direction[2] = l1.z;
	light[2].spot_cutoff_angle = 20.0f;
	light[2].spot_exponent = 8.0f;



	light[3].light_on = 1;
	light[3].position[0] = p2.x; light[3].position[1] = p2.y; // spot light position in WC
	light[3].position[2] = p2.z; light[3].position[3] = 1.0f;

	light[3].ambient_color[0] = 0; light[3].ambient_color[1] = 0;
	light[3].ambient_color[2] = 1; light[3].ambient_color[3] = 1.0f;

	light[3].diffuse_color[0] = 0; light[3].diffuse_color[1] = 0;
	light[3].diffuse_color[2] = 1; light[3].diffuse_color[3] = 1.0f;

	light[3].specular_color[0] = 0; light[3].specular_color[1] = 0;
	light[3].specular_color[2] = 1; light[3].specular_color[3] = 1.0f;

	light[3].spot_direction[0] = l2.x; light[3].spot_direction[1] = l2.y; // spot light direction in WC
	light[3].spot_direction[2] = l2.z;
	light[3].spot_cutoff_angle = 20.0f;
	light[3].spot_exponent = 8.0f;



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

	
	
	
	
	
	
	
	
	
	
	



	glUniform1i(loc_light[2].light_on, light[2].light_on);
	// need to supply position in EC for shading
	position_EC = ViewMatrix * glm::vec4(light[2].position[0], light[2].position[1],
		light[2].position[2], light[2].position[3]);
	glUniform4fv(loc_light[2].position, 1, &position_EC[0]);
	glUniform4fv(loc_light[2].ambient_color, 1, light[2].ambient_color);
	glUniform4fv(loc_light[2].diffuse_color, 1, light[2].diffuse_color);
	glUniform4fv(loc_light[2].specular_color, 1, light[2].specular_color);
	// need to supply direction in EC for shading in this example shader
	// note that the viewing transform is a rigid body transform
	// thus transpose(inverse(mat3(ViewMatrix)) = mat3(ViewMatrix)
	direction_EC = glm::mat3(ViewMatrix) * glm::vec3(light[2].spot_direction[0], light[2].spot_direction[1],
		light[2].spot_direction[2]);
	glUniform3fv(loc_light[2].spot_direction, 1, &direction_EC[0]);
	glUniform1f(loc_light[2].spot_cutoff_angle, light[2].spot_cutoff_angle);
	glUniform1f(loc_light[2].spot_exponent, light[2].spot_exponent);




	glUniform1i(loc_light[3].light_on, light[3].light_on);
	// need to supply position in EC for shading
	position_EC = ViewMatrix * glm::vec4(light[3].position[0], light[3].position[1],
		light[3].position[2], light[3].position[3]);
	glUniform4fv(loc_light[3].position, 1, &position_EC[0]);
	glUniform4fv(loc_light[3].ambient_color, 1, light[3].ambient_color);
	glUniform4fv(loc_light[3].diffuse_color, 1, light[3].diffuse_color);
	glUniform4fv(loc_light[3].specular_color, 1, light[3].specular_color);
	// need to supply direction in EC for shading in this example shader
	// note that the viewing transform is a rigid body transform
	// thus transpose(inverse(mat3(ViewMatrix)) = mat3(ViewMatrix)
	direction_EC = glm::mat3(ViewMatrix) * glm::vec3(light[3].spot_direction[0], light[3].spot_direction[1],
		light[3].spot_direction[2]);
	glUniform3fv(loc_light[3].spot_direction, 1, &direction_EC[0]);
	glUniform1f(loc_light[3].spot_cutoff_angle, light[3].spot_cutoff_angle);
	glUniform1f(loc_light[3].spot_exponent, light[3].spot_exponent);







	glUseProgram(0);
}

void init_objects(void) {
	tiger.rotate.push_front(align({ 0, -1, 0 }, { 0, 0, 1 }));
	tiger.rotate.push_front(glm::mat4(1));		//tiger.velocity = glm::vec3(0, 0, 0);


	ben.scale = glm::vec3(100.0f, -100.0f, -100.0f);
	spider.scale = glm::vec3(50.0f, -50.0f, 50.0f);
	wolf.scale = glm::vec3(200.0f, 100.0f, 100.0f);

	bike.scale = glm::vec3(20.0f, 20.0f, 20.0f);
	//tank.rotate.push_front( align(glm::vec3(-90.0f*TO_RADIAN, 0, 0), { 0, 0, 1 }) );
	ironman.scale = glm::vec3(20.0f, 20.0f, 20.0f);

	ben.position = { -300, 0, -300 };
	ben.acceleration = { 0, -1000, 0 };
	spider.position = { -500, 0, 0 };
	spider.acceleration = { 0, -500, 0 };
	wolf.position = { 0, 0, -500 };
	ironman.position = { 0, 0, 0 };
	ironman.acceleration = { 0, -100, 0 }; 
	ironman.velocity = { 0, 0, 1 };

	slected = &ben;

	//ironman.filename_texture = "Data/dynamic_objects/tiger/red.jpg";
	//spider.filename_texture = "Data/dynamic_objects/tiger/black.jpg";
	
	/*
	tiger.original_dir = { 0, -1, 0 };
	//optimus.original_dir = { 1, 0, 0 };
	cow.original_dir = { 1, 0, 0 };
	cow.position.y += 22;
	tank.original_dir = { 0, -1, 0 };


	optimus.scale = glm::vec3(0.1f, 0.1f, 0.1f);
	cow.scale = glm::vec3(80.0f, 80.0f, 80.0f);

	bus.scale = glm::vec3(3.0f, 3.0f, 3.0f);
	godzilla.scale = glm::vec3(0.5f, 0.5f, 0.5f);
	tank.rotate = glm::vec3(-90.0f*TO_RADIAN, 0, 0);
	*/

	cow2.position = { 0, 200, 0 };
	cow2.scale = glm::vec3(100, 100, 100);
	cow2.is_binary_file = false;

	car1 = new car();
	car1->body->rotate.push_front(align({ 1, 0, 0 }, { 0, 0, 1 }));

	for (auto& w : car1->wheels)
	{
		w->rotate.resize(2, glm::mat4(1));
	}
	
	cams.emplace_back(1000, 1000, 1000);
	cams.emplace_back(700, 700, 700);
	//cams.back().up = glm::vec3(0, 0, 1);
	cams.emplace_back(car1->body->position + glm::vec3(0, 100, 0));
	cams.emplace_back(tiger.position + glm::vec3(0, -88, 62));

	cur_cam = &cams[0];



	objects.emplace_back(&tiger);


	objects.emplace_back(&ben);
	objects.emplace_back(&wolf);
	objects.emplace_back(&spider);
	objects.emplace_back(&ironman);
	objects.emplace_back(&bike);
	/*
	objects.emplace_back(&optimus);
	objects.emplace_back(&cow);
	objects.emplace_back(&bike);
	objects.emplace_back(&bus);
	objects.emplace_back(&godzilla);
	objects.emplace_back(&cow2);
	*/
	
	for (float i = 0; i < 2 * 3.141592; i += 0.1)
	{
		objects.push_back(new object(1, "Data/static_objects/bus_vnt.geom", TYPE_VNT));
		

		objects.back()->position = glm::vec3(
			16 * pow(sin(i), 3) * 20
			, 0
			, (13 * cos(i) - 5 * cos(2 * i) - 2 * cos(3 * i) - cos(4 * i))* 20
		);

		objects.back()->velocity = glm::vec3(
			16 * 3 * pow(sin(i + 0.1), 2) * cos(i + 0.1)
			, 0
			, (-13 * sin(i + 0.1) + 10 * sin(2 * (i + 0.1)) + 6 * sin(3 * (i + 0.1)) + 4 * sin(4 * (i + 0.1)))
		);

		objects.back()->rotate.push_front(align({ 0, 0, 1 }, objects.back()->velocity));
	}
	

	for (float i = -21; i < 21; i += 0.2)
	{
		objects.push_back(new object(1, "Data/static_objects/txtdata/car_body_triangles_v.txt", TYPE_V));
		objects.back()->is_binary_file = false;

		objects.back()->material.ambient_color[0] = 0;
		objects.back()->material.ambient_color[1] = 0;
		objects.back()->material.ambient_color[2] = 0;
		objects.back()->material.ambient_color[3] = 1;

		float x = betman_x(i);
		float y = betman_y(i);

		float dr = 2 * sin(1.2 * i) * cos(1.2 * i) * 1.2 
			+ 3 * pow(cos(6 * i), 2) * -sin(6*i) * 6;


		objects.back()->position = glm::vec3(
			betman_x(i) * 25
			, 0
			, betman_y(i) * 25
		);

		objects.back()->velocity = glm::vec3(
			(betman_x(i+1) - betman_x(i)) * 25
			, 0
			, (betman_y(i + 1) - betman_y(i)) * 25
		);

		objects.back()->rotate.push_front(align({ 0, 0, 1 }, objects.back()->velocity));
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

void greetings(char *program_name) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	fprintf(stdout, "      KEYS : only works with small letter\n\n");

	fprintf(stdout, "      1, 2, 3 : togle/untogle 1st, 2nd, 3rd subwindow respectively\n\n");

	fprintf(stdout, "MAIN WINDOW\n");
	fprintf(stdout, "	shift + pressed mouse move right : zoom in\n");
	fprintf(stdout, "	shift + pressed mouse move left : zoom out\n");
	fprintf(stdout, "	right arrow key : main camera move right\n");
	fprintf(stdout, "	left arrow key : main camera move left\n");
	fprintf(stdout, "	up arrow key : main camera move up\n");
	fprintf(stdout, "	down arrow key : main camera move down\n");




	fprintf(stdout, "\n");
	fprintf(stdout, "SUB-WINDOW 1\n");
	fprintf(stdout, "	if first subwindow togled :\n");
	fprintf(stdout, "		q : subcamera1 to go up\n");
	fprintf(stdout, "		e : subcamera1 to go down\n");
	fprintf(stdout, "		w : subcamera1 to go forward\n");
	fprintf(stdout, "		s : subcamera1 to go backward\n");
	fprintf(stdout, "		z : subcamera1 to rotate right\n");
	fprintf(stdout, "		c : subcamera1 to roate left\n");
	fprintf(stdout, "		ALT + pressed mouse move right : zoom in\n");
	fprintf(stdout, "		ALT + pressed mouse move left : zoom out\n");


	fprintf(stdout, "\n");
	fprintf(stdout, "	if first subwindow is not togled :\n");
	fprintf(stdout, "		w : ben run forward\n");
	fprintf(stdout, "		s : ben run backward\n");
	fprintf(stdout, "		d : ben rotate right\n");
	fprintf(stdout, "		s : ben rotate left\n");

	fprintf(stdout, "\n");
	fprintf(stdout, "	t : spider follow ben\n");

	fprintf(stdout, "\n");
	fprintf(stdout, "	u : ironman go up\n");
	fprintf(stdout, "	i : ironman rotate\n");
	fprintf(stdout, "	o : ironman stop to rotate\n");


	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 1
void main(int argc, char *argv[]) {
	char program_name[64] = "Sogang CSE4170 3D Objects";

	glutInit(&argc, argv);
  	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(1000, 1000);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}