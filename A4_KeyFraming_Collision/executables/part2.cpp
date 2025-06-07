#include "base/camera.hpp"
#include "frame/cloth.hpp"


using namespace COL781;
namespace GL = COL781::OpenGL;
using namespace glm;

GL::Rasterizer r;
GL::ShaderProgram program;

int nv,nt,ne;


vec3* vertices = nullptr;
vec3* normals = nullptr;
ivec3* triangles = nullptr;
ivec2* edges = nullptr;

GL::Object object;
GL::AttribBuf vertexBuf, normalBuf;
particle_matrix cloth;

CameraControl camCtl;

void initializeScene() {
    nv = cloth_division.first * cloth_division.second;
    nt = (cloth_division.first - 1) * (cloth_division.second - 1) * 2;
    ne = (cloth_division.first * (cloth_division.second - 1)) + (cloth_division.second * (cloth_division.first - 1));
	object = r.createObject();
    vertices = new vec3[nv];
    normals = new vec3[nv];
    triangles = new ivec3[nt];
    edges = new ivec2[ne];


    cloth.initialize_particle_matrix(vec3(0.5f, -0.5f, 0.0f), 1.0f);
    cloth.is_fixed[0][0] = true;
    cloth.is_fixed[0][9] = true;
    mesh temp_mesh = cloth.get_mesh();
    int loop_iter = std::max(nv,std::max(nt,ne));
    wh(i, loop_iter){
        if(i < nv){
            vertices[i] = temp_mesh.vertices[i];
            normals[i] = temp_mesh.normals[i];
        }
        if(i < nt){
            triangles[i] = temp_mesh.triangles[i];
        }
        if(i < ne){
            edges[i] = temp_mesh.edges[i];
        }
        i = i + 1;
    }

    r.draw_helper(object,nv,nt,ne,vertices,normals,triangles,edges,vertexBuf,normalBuf);



}

void updateScene(float t) {
    bool flag = false; // CHANGE HERE FOR PBD ON AND OFF ( False , True )
    cloth.invocare_forciam(flag);
    cloth.compute_call(0.0001f,flag);
    mesh temp_mesh = cloth.get_mesh();
    wh(i,nv){
        vertices[i] = temp_mesh.vertices[i];
        normals[i] = temp_mesh.normals[i];
        i = i + 1;
    }

    r.update_helper(vertexBuf, normalBuf, nv, vertices, normals);
}

int main() {
	int width = 640, height = 480;
	if (!r.initialize("Animation", width, height)) {
		return EXIT_FAILURE;
	}
	camCtl.initialize(width, height);
	camCtl.camera.setCameraView(vec3(4.5, -0.5, 0.0), vec3(0.5, -1.5, 0.0), vec3(0.0, 1.0, 0.0));
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
        glm::vec3 teal(0.0f, 0.6f, 0.6f);
        glm::vec3 white(1.0f, 1.0f, 1.0f);
        r.setUniform(program, "ambientColor", 0.2f*white);
        r.setUniform(program, "extdiffuseColor", 0.9f*teal);
        r.setUniform(program, "intdiffuseColor", 0.4f*teal);
        r.setUniform(program, "specularColor", 0.6f*white);
        r.setUniform(program, "phongExponent", 20.f);
		r.drawTriangles(object);

		r.setupWireFrame();
        glm::vec3 black(0.0f, 0.0f, 0.0f);
        r.setUniform(program, "ambientColor", black);
        r.setUniform(program, "extdiffuseColor", black);
        r.setUniform(program, "intdiffuseColor", black);
        r.setUniform(program, "specularColor", black);
        r.setUniform(program, "phongExponent", 0.f);
		r.drawEdges(object);

		r.show();
	}

    delete[] vertices;
    delete[] normals;
    delete[] triangles;
    delete[] edges;
}
