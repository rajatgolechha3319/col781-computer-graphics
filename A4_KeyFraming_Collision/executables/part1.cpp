#include "base/camera.hpp"
#include "frame/rectangle.hpp"
#include "frame/catmull_rom.hpp"

#include <iostream>

using namespace COL781;
namespace GL = COL781::OpenGL;
using namespace glm;

GL::Rasterizer r;
GL::ShaderProgram program;

const int n_obj = 5;
const int nv = 8, nt = 12, ne = 12;

// Arrays for all objects
vec3 vertices[n_obj][nv];
vec3 normals[n_obj][nv];
ivec3 triangles[n_obj][nt];
ivec2 edges[n_obj][ne];

// OpenGL objects and buffers
GL::Object objects[n_obj];
GL::AttribBuf vertexBufs[n_obj];
GL::AttribBuf normalBufs[n_obj];


CameraControl camCtl;
Skeletal_Box* root = new Skeletal_Box();
Skeletal_Box* child = new Skeletal_Box();
Skeletal_Box* grandchild = new Skeletal_Box();
Skeletal_Box* child2 = new Skeletal_Box();
Skeletal_Box* grandchild2 = new Skeletal_Box();

void initializeScene() {
	wh(i_0,n_obj){
		objects[i_0] = r.createObject();
		i_0 = i_0 + 1;
	}

	// We will make a cube 
	Axis root_axis = Axis(vec3(0.5f, -0.5f, 0.0f), vec3(0.0f, 1.0f, 0.0f), 0.0f);
	Axis child_axis = Axis(vec3(0.0f, -0.5f, 0.2f), vec3(1.0f, 0.0f, 0.0f), 0.0f);
	Axis child2_axis = Axis(vec3(1.0f, -0.5f, 0.2f), vec3(1.0f, 0.0f, 0.0f), 0.0f);
	Axis grandchild_axis = Axis(vec3(0.0f, -0.5f, 0.9f), vec3(1.0f, 0.0f, 0.0f), 0.0f);
	Axis grandchild2_axis = Axis(vec3(1.0f, -0.5f, 0.9f), vec3(1.0f, 0.0f, 0.0f), 0.0f);

	root->initialize_Box(nullptr, std::make_pair(std::make_pair(0.75, 1.5), 1.5), vec3(0.5f, -0.5f, 0.0f), root_axis);
	child->initialize_Box(root, std::make_pair(std::make_pair(0.25, 0.3), 0.8), vec3(-0.5f, 0.0f, 0.5f), child_axis);
	child2->initialize_Box(root, std::make_pair(std::make_pair(0.25, 0.3), 0.8), vec3(0.5f, 0.0f, 0.5f), child2_axis);
	grandchild->initialize_Box(child, std::make_pair(std::make_pair(0.25, 0.3), 0.8), vec3(0.0f, 0.0f, 0.7f), grandchild_axis);
	grandchild2->initialize_Box(child2, std::make_pair(std::make_pair(0.25, 0.3), 0.8), vec3(0.0f, 0.0f, 0.7f), grandchild2_axis);

	wh(i_1,8){
		for(int j=0; j < n_obj; j++){
			vertices[j][i_1] = root->vertices[i_1];
			normals[j][i_1] = root->normals[i_1];
		}
		i_1 = i_1 + 1;
	}

	wh(i_2,12){
		for(int j=0; j < n_obj; j++){
			triangles[j][i_2] = root->triangles[i_2];
			edges[j][i_2] = root->edges[i_2];
		}
		i_2 = i_2 + 1;
	}

	wh(i_3,n_obj){
		r.draw_helper(objects[i_3], nv, nt, ne, vertices[i_3], normals[i_3], triangles[i_3], edges[i_3], vertexBufs[i_3], normalBufs[i_3]);
		i_3 = i_3 + 1;
	}

}

void updateScene(float t_curr) {
	// float freq = 2, amp = 1;
	// float phase0 = 0, phase1 = 0.5;
	// float theta0 = amp*cos(freq*t + phase0), theta1 = amp*cos(freq*t + phase1);
	part1 p1,p2,p3;
	float x_perturb_mag = 0.85f;
	float y_perturb_mag = 0.5f;
	float z_perturb_mag = 0.5f;

	std::vector<float> non_uniform_spaced_points = {
		0.0f,
		0.1f,
		0.18f,
		0.25f,
		0.39f,
		1.25f,
		2.16f,
		2.5f,
		3.0f,
		3.1f,
		3.2f
	};




	std::vector<time_position> time_positions;
	// use sine wave for x, cos wave for y and sin wave at 30 degree phase for sampling
	int sample_count = non_uniform_spaced_points.size();
	double freq = 2.0f; // Change this to change the motion
	wh(sample_iter, sample_count){
		float t_ = non_uniform_spaced_points[sample_iter];
		float x = x_perturb_mag * cos(freq * t_);
		float y = y_perturb_mag * sin(freq * t_);
		float z = z_perturb_mag * sin(freq * t_ + M_PI/6.0f);
		time_positions.push_back({t_, vec3(x, y, z)});
		sample_iter++;
	}


	std::vector<time_position> time_position_2 = {
		{0.0000f, vec3(0.0000f, 0.0f, 0.0f)},
		{0.1156f, vec3(0.1519f, 0.0f, 0.0f)},
		{0.1812f, vec3(0.2486f, 0.0f, 0.0f)},
		{0.3856f, vec3(0.5247f, 0.0f, 0.0f)},
		{0.6857f, vec3(0.9432f, 0.0f, 0.0f)},
		{1.1112f,vec3(1.5433f, 0.0f, 0.0f)},
		{1.5623f, vec3(2.1611f, 0.0f, 0.0f)},
		{2.1254f, vec3(2.9276f, 0.0f, 0.0f)},
		{3.0111f, vec3(4.1593f, 0.0f, 0.0f)},
		{4.5589f, vec3(6.2832f, 0.0f, 0.0f)}
	};
	
	std::vector<time_position> time_position_3 = {
		{0.0000f, vec3(3.1416f, 0.0f, 0.0f)},
		{0.1119f, vec3(3.2935f, 0.0f, 0.0f)},
		{0.1843f, vec3(3.3902f, 0.0f, 0.0f)},
		{0.3834f, vec3(3.6663f, 0.0f, 0.0f)},
		{0.6857f, vec3(4.0848f, 0.0f, 0.0f)},
		{1.1182f,vec3(4.6849f, 0.0f, 0.0f)},
		{1.5657f, vec3(5.3027f, 0.0f, 0.0f)},
		{2.1219f, vec3(6.0691f, 0.0f, 0.0f)},
		{3.0157f, vec3(7.3009f, 0.0f, 0.0f)},
		{4.5538f, vec3(9.4248f, 0.0f, 0.0f)}
	};
	

	p1.initialize(time_positions);
	p2.initialize(time_position_2);
	p3.initialize(time_position_3);

	float theta0 = p2.func(t_curr).x;
	float theta1 = p3.func(t_curr).x;
	child->hinge_joint.angle = theta0;
	child2->hinge_joint.angle = theta0 + 3.14f;
	child->compute_call();
	child2->compute_call();
	wh(child_update_iter, 8){
		vertices[1][child_update_iter] = child->vertices[child_update_iter];
		normals[1][child_update_iter] = child->normals[child_update_iter];
		vertices[3][child_update_iter] = child2->vertices[child_update_iter];
		normals[3][child_update_iter] = child2->normals[child_update_iter];
		child_update_iter = child_update_iter + 1;
	}

	// Update grandchild
	grandchild->hinge_joint.angle = theta1;
	grandchild2->hinge_joint.angle = theta1;
	grandchild->compute_call(true);
	grandchild2->compute_call(true);

	wh(grandchild_update_iter, 8){
		vertices[2][grandchild_update_iter] = grandchild->vertices[grandchild_update_iter];
		normals[2][grandchild_update_iter] = grandchild->normals[grandchild_update_iter];
		vertices[4][grandchild_update_iter] = grandchild2->vertices[grandchild_update_iter];
		normals[4][grandchild_update_iter] = grandchild2->normals[grandchild_update_iter];
		grandchild_update_iter = grandchild_update_iter + 1;
	}

	
	glm::vec3 translation_vector = p1.func(t_curr);
	root->move_object(translation_vector);


	root->compute_call();

	wh(root_update_iter, 8){
		vertices[0][root_update_iter] = root->vertices[root_update_iter];
		normals[0][root_update_iter] = root->normals[root_update_iter];
		root_update_iter = root_update_iter + 1;
	}

	wh(i_4,n_obj){
		r.update_helper(vertexBufs[i_4], normalBufs[i_4], nv, vertices[i_4], normals[i_4]);
		i_4 = i_4 + 1;
	}

}

int main() {
	int width = 640, height = 480;
	if (!r.initialize("Animation", width, height)) {
		return EXIT_FAILURE;
	}
	camCtl.initialize(width, height);
	camCtl.camera.setCameraView(vec3(2.0, -0.5, 6.0), vec3(2.0, -0.5, 0.0), vec3(0.0, 1.0, 0.0));
	program = r.createShaderProgram(
		r.vsBlinnPhong(),
		r.fsBlinnPhong()
	);

	initializeScene();

	while (!r.shouldQuit()) {
        float t = SDL_GetTicks64()*1e-3;
		updateScene(t);

		camCtl.update();
		Camera &camera = camCtl.camera;

		r.clear(vec4(0.4, 0.4, 0.4, 1.0));
		r.enableDepthTest();
		r.useShaderProgram(program);

		r.setUniform(program, "model", glm::mat4(1.0));
		r.setUniform(program, "view", camera.getViewMatrix());
		r.setUniform(program, "projection", camera.getProjectionMatrix());
		r.setUniform(program, "lightPos", camera.position);
		r.setUniform(program, "viewPos", camera.position);
		r.setUniform(program, "lightColor", vec3(1.0f, 1.0f, 1.0f));

		r.setupFilledFaces();
		glm::vec3 skyBlue(135.0f/255.0f, 206.0f/255.0f, 235.0f/255.0f);
        glm::vec3 orange(1.0f, 0.6f, 0.2f);
        glm::vec3 white(1.0f, 1.0f, 1.0f);
        r.setUniform(program, "ambientColor", 0.1f*skyBlue);
        r.setUniform(program, "extdiffuseColor", 1.0f*skyBlue);
        r.setUniform(program, "intdiffuseColor", 1.0f*skyBlue);
        r.setUniform(program, "specularColor", 0.8f*white);
        r.setUniform(program, "phongExponent", 32.0f);
		wh(i_1,n_obj){
			r.drawTriangles(objects[i_1]);
			i_1 = i_1 + 1;
		}

		r.setupWireFrame();
        glm::vec3 black(0.0f, 0.0f, 0.0f);
        r.setUniform(program, "ambientColor", black);
        r.setUniform(program, "extdiffuseColor", black);
        r.setUniform(program, "intdiffuseColor", black);
        r.setUniform(program, "specularColor", black);
        r.setUniform(program, "phongExponent", 1.0f);
		wh(i_2,n_obj){
			r.drawEdges(objects[i_2]);
			i_2 = i_2 + 1;
		}

		r.show();
	}
}
