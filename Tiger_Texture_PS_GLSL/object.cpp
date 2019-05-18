#include "object.h"


#include "Shaders/LoadShaders.h"
#include "My_Shading.h"

#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

#define LOC_VERTEX 0
#define LOC_NORMAL 1
#define LOC_TEXCOORD 2

// texture stuffs
#define N_TEXTURES_USED 2
#define TEXTURE_ID_FLOOR 0
#define TEXTURE_ID_TIGER 1




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

object::object
(
	int num_frames
	, std::string fv
	, OBJ_TYPE type
)
	: num_frames(num_frames)
	, filename_vertices(fv)
	, type(type)
	, filename_texture("Data/dynamic_objects/tiger/tiger_tex2.jpg")
	, position(0)
	, material(tiger_material)
	, velocity({ 0, 0, 1e-7 })
	, acceleration(0)
	, scale(1)
	, rotate(0)
	, num_triangles(num_frames)
	, vertex_offset(num_frames)
	, vertices(num_frames)
	, cur_frame(0)
	, is_texture_on(true)
	, is_binary_file(true)
	, timestamp_last(timestamp_scene)
	, original_dir(0, 0, 1)
	, parent(nullptr)
	, additional(1)
{

}


object::~object()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void object::prepare(void)
{
	auto f = [](glm::mat4 m) { return (glm::mat4)(glm::translate(m, glm::vec3(0, 0, 0))); };


	int num_total_triangles = 0;
	char filename[512];

	int bytes_per_vertex = elements_per_vertex[type] * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	int bytes_per_triangle = 3 * bytes_per_vertex;

	for (int i = 0; i < num_frames; i++) {
		sprintf(filename, filename_vertices.c_str(), i);

		if (is_binary_file) {
			num_triangles[i] = read_geometry_file_binary(&vertices[i], type, filename);
		}
		else
		{
			num_triangles[i] = read_geometry_file_txt(&vertices[i], type, filename);
		}


		// assume all geometry files are effective
		num_total_triangles += num_triangles[i];

		if (i == 0)
			vertex_offset[i] = 0;
		else
			vertex_offset[i] = vertex_offset[i - 1] + 3 * num_triangles[i - 1];
	}

	// initialize vertex array object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// initialize vertex buffer object
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, num_total_triangles*bytes_per_triangle, NULL, GL_STATIC_DRAW);

	for (int i = 0; i < num_frames; i++)
		glBufferSubData(GL_ARRAY_BUFFER, vertex_offset[i] * bytes_per_vertex,
			num_triangles[i] * bytes_per_triangle, vertices[i]);

	// as the geometry data exists now in graphics memory, ...
	for (int i = 0; i < num_frames; i++)
		free(vertices[i]);

	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	if (type >= TYPE_VN) {
		glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}
	if (type >= TYPE_VNT) {
		glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	if (true) {
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
}

void object::draw(const glm::mat4& ViewMatrix, const glm::mat4& ProjectionMatrix) {

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
	glDrawArrays(GL_TRIANGLES, vertex_offset[cur_frame], 3 * num_triangles[cur_frame]);
	glBindVertexArray(0);
}

glm::mat4 object::getModelMatrix()
{
	glm::mat4 ModelMatrix(1.0f);
	if (parent != nullptr) {
		ModelMatrix = ModelMatrix * parent->getModelMatrix();
	}

	ModelMatrix = glm::translate(
		ModelMatrix,
		this->position
	);

	if (glm::distance(glm::normalize(original_dir), glm::normalize(velocity))) {
		auto axis = glm::cross(original_dir, glm::normalize(velocity));
		float rad = acos(glm::dot(original_dir, velocity) / glm::length(original_dir) / glm::length(velocity));
		ModelMatrix = glm::rotate(
			ModelMatrix,
			rad,
			axis
		);
	}

	ModelMatrix = glm::scale(
		ModelMatrix,
		this->scale
	);


	return ModelMatrix;
}

void object::updata_pos() {
	float dtime = timestamp_scene - timestamp_last;

	position += velocity * dtime + 0.5f * acceleration * dtime * dtime;
	velocity += (acceleration)* dtime;

	timestamp_last = timestamp_scene;
}

void object::move_forward(float s)
{
	position += s * normalize(velocity);
}

void object::turn_left(float rad)
{
	glm::vec3 forward = glm::normalize(velocity);
	auto up = glm::vec3(0, 1, 0);

	auto right = glm::cross(forward, up);
	up = glm::cross(right, forward);

	auto m_rotate = glm::rotate(glm::mat4(1), rad, up);


	velocity = vec4_to_3(m_rotate * vec3_to_4(velocity, 0));
}
