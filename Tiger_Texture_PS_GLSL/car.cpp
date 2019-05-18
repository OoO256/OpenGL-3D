#include "car.h"
#include "main.h"
#include "utility.h"

extern std::vector<object *>objects;

car::car(void)
{
	body = new object(1, "Data/static_objects/txtdata/car_body_triangles_v.txt", TYPE_V);

	body->position = { 0, 50, 0 };
	body->scale = glm::vec3(10, 10, 10);
	body->is_binary_file = false;
	body->original_dir = glm::vec3(1, 0, 0);
	body->velocity = { 1e-7, 0, 0 };

	objects.emplace_back(body);
	nuts.resize(5);
	for (int i = 0; i < 4; i++)
	{
		wheels.push_back(new object(1, "Data/static_objects/txtdata/car_wheel_triangles_v.txt", TYPE_V));
		wheels[i]->parent = body;

		wheels[i]->is_binary_file = false;
		wheels[i]->velocity = { 1e-7, 0, 0 };
		wheels[i]->original_dir = glm::vec3(1, 0, 0);
		objects.push_back(wheels[i]);

		for (int j = 0; j < 5; j++)
		{
			nuts[i].push_back(new object(1, "Data/static_objects/txtdata/car_nut_triangles_v.txt", TYPE_V));
			nuts[i][j]->parent = wheels[i];


			nuts[i][j]->is_binary_file = false;
			nuts[i][j]->velocity = { 1e-7, 0, 0 };
			nuts[i][j]->original_dir = glm::vec3(1, 0, 0);


			if (i < 2) {
				nuts[i][j]->position = glm::vec3(1.2f * cos(360 / 5 * TO_RADIAN * j), 1.2f * sin(360 / 5 * TO_RADIAN * j), 1.0);
			}
			else {
				nuts[i][j]->position = glm::vec3(1.2f * cos(360 / 5 * TO_RADIAN * j), 1.2f * sin(360 / 5 * TO_RADIAN * j), -1.0);
			}



			objects.push_back(nuts[i][j]);
		}
	}

	wheels[0]->position = glm::vec3(-3.9, -3.5, 4.5);
	wheels[1]->position = glm::vec3(3.9, -3.5, 4.5);
	wheels[2]->position = glm::vec3(-3.9, -3.5, -4.5);
	wheels[3]->position = glm::vec3(3.9, -3.5, -4.5);


	body->material.ambient_color[0] = 1.0f;
	body->material.ambient_color[1] = 1.0f;
	body->material.ambient_color[2] = 1.0f;
	body->material.ambient_color[3] = 1.0f;

	for (auto& w : wheels)
	{
		w->material.ambient_color[0] = 0;
		w->material.ambient_color[1] = 0;
		w->material.ambient_color[2] = 0;
		w->material.ambient_color[3] = 1;
	}


	for (auto& i : nuts)
	{
		for (auto& j : i) {
			j->material.ambient_color[0] = 1.0f;
			j->material.ambient_color[1] = 0.0f;
			j->material.ambient_color[2] = 0.0f;
			j->material.ambient_color[3] = 1.0f;
		}
	}


	//nuts[1][0]->position = glm::vec3(1.2f, 0.0f, 1.0);
}

car::~car()
{
}

void car::move_forward(float s)
{
	body->move_forward(s);


	auto f = glm::normalize(glm::vec3(1, 0, 0));
	auto up = glm::vec3(0, 1, 0);
	auto r = glm::cross(f, up);
	up = glm::cross(r, f);

	for (auto& w : wheels)
	{
		auto m_rotate = glm::rotate(glm::mat4(1), -s * 2 * TO_RADIAN, r);
		w->velocity = vec4_to_3(m_rotate * vec3_to_4(w->velocity, 0));
	}


}
