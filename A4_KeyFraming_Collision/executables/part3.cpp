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

// PLANE
vec3 plane_vertices[4];
vec3 plane_normals[4];
ivec3 plane_triangles[2];
ivec2 plane_edges[4];

// SPHERE
vec3* sphere_vertices = nullptr;
vec3* sphere_normals = nullptr;
ivec3* sphere_triangles = nullptr;
ivec2* sphere_edges = nullptr;

// SPHERE2
vec3* sphere2_vertices = nullptr;
vec3* sphere2_normals = nullptr;
ivec3* sphere2_triangles = nullptr;
ivec2* sphere2_edges = nullptr;


GL::Object object;
GL::AttribBuf vertexBuf, normalBuf;
particle_matrix cloth;
sphere s;
sphere s2;
plane ground_plane;

GL::Object plane_object;
GL::AttribBuf plane_vertexBuf, plane_normalBuf;

GL::Object sphere_object;
GL::AttribBuf sphere_vertexBuf, sphere_normalBuf;

GL::Object sphere2_object;
GL::AttribBuf sphere2_vertexBuf, sphere2_normalBuf;

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


    cloth.initialize_particle_matrix(vec3(0.5f, -0.5f, 0.0f), 0.1f); // cloth mass is second term
    cloth.is_fixed[0][0] = true;
    // cloth.is_fixed[0][5] = true;
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

    // PLANE
    plane_object = r.createObject();

    ground_plane.init_plane(std::make_pair(vec3(0,-1,0),1.9f));
    std::vector<vec3> plane_boundaries = ground_plane.plane_boundaries(vec3(0,-1.9f,0), 50.0f);
    wh(i2, 4){
        plane_vertices[i2] = plane_boundaries[i2];
        plane_normals[i2] = ground_plane.get_plane_normal();
        i2 = i2 + 1;
    }
    plane_triangles[0] = ivec3(0,2,1);
    plane_triangles[1] = ivec3(0,1,3);
    plane_edges[0] = ivec2(0,3);
    plane_edges[1] = ivec2(3,1);
    plane_edges[2] = ivec2(1,2);
    plane_edges[3] = ivec2(2,0);

    r.draw_helper(plane_object, 4, 4, 2, plane_vertices, plane_normals, plane_triangles, plane_edges, plane_vertexBuf, plane_normalBuf);

    // SPHERE
    sphere_object = r.createObject();
    s.init_sphere(glm::vec3(-0.5f, -1.5f, -0.5f), 0.4f, glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(1.0f,0.5f,2.0f), 10.0f);
    mesh sphere_mesh = s.get_mesh();
    int sphere_nv = sphere_mesh.vertices.size();
    int sphere_nt = sphere_mesh.triangles.size();
    int sphere_ne = sphere_mesh.edges.size();
    sphere_vertices = new vec3[sphere_nv];
    sphere_normals = new vec3[sphere_nv];
    sphere_triangles = new ivec3[sphere_nt];
    sphere_edges = new ivec2[sphere_ne];
    int loop_iter2 = std::max(sphere_nv,std::max(sphere_nt,sphere_ne));
    wh(i3, loop_iter2){
        if(i3 < sphere_nv){
            sphere_vertices[i3] = sphere_mesh.vertices[i3];
            sphere_normals[i3] = sphere_mesh.normals[i3];
        }
        if(i3 < sphere_nt){
            sphere_triangles[i3] = sphere_mesh.triangles[i3];
        }
        if(i3 < sphere_ne){
            sphere_edges[i3] = sphere_mesh.edges[i3];
        }
        i3 = i3 + 1;
    }

    r.draw_helper(sphere_object, sphere_nv, sphere_nt, sphere_ne, sphere_vertices, sphere_normals, sphere_triangles, sphere_edges, sphere_vertexBuf, sphere_normalBuf);

    // SPHERE2
    sphere2_object = r.createObject();
    s2.init_sphere(glm::vec3(5.5f, -1.5f, 0.5f), 0.4f, glm::vec3(-2.0f, 0.0f, 0.0f), glm::vec3(-1.0f,-0.5f,-2.0f), 10.0f);
    mesh sphere2_mesh = s2.get_mesh();
    int sphere2_nv = sphere2_mesh.vertices.size();
    int sphere2_nt = sphere2_mesh.triangles.size();
    int sphere2_ne = sphere2_mesh.edges.size();
    sphere2_vertices = new vec3[sphere2_nv];
    sphere2_normals = new vec3[sphere2_nv];
    sphere2_triangles = new ivec3[sphere2_nt];
    sphere2_edges = new ivec2[sphere2_ne];
    int loop_iter3 = std::max(sphere2_nv,std::max(sphere2_nt,sphere2_ne));
    wh(i4, loop_iter3){
        if(i4 < sphere2_nv){
            sphere2_vertices[i4] = sphere2_mesh.vertices[i4];
            sphere2_normals[i4] = sphere2_mesh.normals[i4];
        }
        if(i4 < sphere2_nt){
            sphere2_triangles[i4] = sphere2_mesh.triangles[i4];
        }
        if(i4 < sphere2_ne){
            sphere2_edges[i4] = sphere2_mesh.edges[i4];
        }
        i4 = i4 + 1;
    }

    sphere2_vertexBuf = r.createVertexAttribs(sphere2_object, 0, sphere2_nv, sphere2_vertices);
    sphere2_normalBuf = r.createVertexAttribs(sphere2_object, 1, sphere2_nv, sphere2_normals);
    r.createTriangleIndices(sphere2_object, sphere2_nt, sphere2_triangles);
    r.createEdgeIndices(sphere2_object, sphere2_ne, sphere2_edges);

    r.draw_helper(sphere2_object, sphere2_nv, sphere2_nt, sphere2_ne, sphere2_vertices, sphere2_normals, sphere2_triangles, sphere2_edges, sphere2_vertexBuf, sphere2_normalBuf);

}

void updateScene(float t) {
    bool flag = false;
    cloth.invocare_forciam(flag);
    cloth.compute_call(0.0001f,flag, &ground_plane, &s, &s2);
    // cloth.debug_();
    mesh temp_mesh = cloth.get_mesh();
    wh(i,nv){
        vertices[i] = temp_mesh.vertices[i];
        normals[i] = temp_mesh.normals[i];
        i = i + 1;
    }

    r.update_helper(vertexBuf, normalBuf, nv, vertices, normals);

    s.compute_call();
    mesh sphere_mesh = s.get_mesh();

    int sphere_nv = sphere_mesh.vertices.size();

    wh(i2,sphere_nv){
        sphere_vertices[i2] = sphere_mesh.vertices[i2];
        sphere_normals[i2] = sphere_mesh.normals[i2];
        i2 = i2 + 1;
    }

    r.update_helper(sphere_vertexBuf,sphere_normalBuf, sphere_nv, sphere_vertices, sphere_normals);

    s2.compute_call();
    mesh sphere2_mesh = s2.get_mesh();

    int sphere2_nv = sphere2_mesh.vertices.size();

    wh(i3,sphere2_nv){
        sphere2_vertices[i3] = sphere2_mesh.vertices[i3];
        sphere2_normals[i3] = sphere2_mesh.normals[i3];
        i3 = i3 + 1;
    }

    r.update_helper(sphere2_vertexBuf, sphere2_normalBuf, sphere2_nv, sphere2_vertices, sphere2_normals);


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
        glm::vec3 white(1.0f, 1.0f, 1.0f);
        glm::vec3 blue(0.1f, 0.1f, 0.9f);
        glm::vec3 red(0.9f, 0.1f, 0.1f);
        glm::vec3 grey(0.5f, 0.5f, 0.5f);
        glm::vec3 green(0.1f, 0.9f, 0.1f);
        r.setUniform(program, "ambientColor", 0.2f*white);
        r.setUniform(program, "extdiffuseColor", 0.9f*green);
        r.setUniform(program, "intdiffuseColor", 0.4f*green);
        r.setUniform(program, "specularColor", 0.6f*white);
        r.setUniform(program, "phongExponent", 20.f);
        r.drawTriangles(plane_object);
        r.setUniform(program, "extdiffuseColor", 0.9f*grey);
        r.setUniform(program, "intdiffuseColor", 0.4f*grey);
        r.drawTriangles(sphere_object);
        r.setUniform(program, "extdiffuseColor", 0.9f*blue);
        r.setUniform(program, "intdiffuseColor", 0.4f*blue);
		r.drawTriangles(object);
        r.setUniform(program, "extdiffuseColor", 0.9f*red);
        r.setUniform(program, "intdiffuseColor", 0.4f*red);
        r.drawTriangles(sphere2_object);

		r.setupWireFrame();
        glm::vec3 black(0.0f, 0.0f, 0.0f);
        r.setUniform(program, "ambientColor", black);
        r.setUniform(program, "extdiffuseColor", black);
        r.setUniform(program, "intdiffuseColor", black);
        r.setUniform(program, "specularColor", black);
        r.setUniform(program, "phongExponent", 0.f);
		r.drawEdges(object);
        r.drawEdges(plane_object);
        r.drawEdges(sphere_object);
        r.drawEdges(sphere2_object);

		r.show();
	}

    delete[] vertices;
    delete[] normals;
    delete[] triangles;
    delete[] edges;

    delete[] sphere_vertices;
    delete[] sphere_normals;
    delete[] sphere_triangles;
    delete[] sphere_edges;

    delete[] sphere2_vertices;
    delete[] sphere2_normals;
    delete[] sphere2_triangles;
    delete[] sphere2_edges;
}
