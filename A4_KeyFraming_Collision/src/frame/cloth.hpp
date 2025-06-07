#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <utility>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <set>

using std::string;
#define EPSILON 0.00000001
#define SPHERE_SLICES 15
#define SPHERE_STACKS 20
#define COEF_RESTITUTION 0.5f
#define COEF_FRICTION 0.7f
#define DEFAULT_TIME_STEP 0.000125f // 125 mics

namespace constants {
    const glm::vec3 zeroVec3(0.0f, 0.0f, 0.0f);
}

// CLOTH PARAMS
extern std::pair<float,float> cloth_size;
extern std::pair<int,int> cloth_division;

// SPRING PARAMS
extern glm::vec3 spring_k; // SPRING - STRUCTURAL - SHEAR - BENDING
extern glm::vec3 spring_k_damp; // SPRING -> DAMPING CONSTANTS

// GLOBAL PARAMAS
extern glm::vec3 global_acceleration; // GLOBAL ACCELERATION
extern int PBD_iters; // PBD iterations

struct spring{
    std::pair<std::pair<int,int>,std::pair<int,int>> particles_pair;
    float rest_length;
    std::pair<float,float> spring_constants_pair;

    spring(std::pair<std::pair<int,int>,std::pair<int,int>>  particles_pair, float rest_length, std::pair<float,float> spring_constants_pair){
        this->particles_pair = particles_pair;
        this->rest_length = rest_length;
        this->spring_constants_pair = spring_constants_pair;
    }
};

struct mesh{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::ivec3> triangles;
    std::vector<glm::ivec2> edges;
};

class sphere;
class plane;
class particle_matrix{
    private:
        std::vector<std::vector<glm::vec3>> particle_location_matrix;
        std::vector<std::vector<glm::vec3>> particle_velocity_matrix;
        std::vector<std::vector<glm::vec3>> particle_force_matrix;
        void init_matrix();
        std::vector<spring> springs_vec;
        public:
        std::vector<std::vector<bool>> is_fixed;
        float mass_per_particle;
        // GETTERS
        glm::vec3 get_particle_location(int i, int j) const {
            return particle_location_matrix[i][j];
        }
        glm::vec3 get_particle_velocity(int i,int j) const {
            return particle_velocity_matrix[i][j];
        }
        glm::vec3 get_particle_force_matrix(int i,int j) const {
            return particle_force_matrix[i][j];
        }
        // SETTERS
        void set_value(int i,int j, string mat_name, glm::vec3 value);
        void clear_force_matrix();
        void initialize_particle_matrix(glm::vec3 initial_cloth_center, float total_cloth_mass);

        // DEBUG
        void debug_();

        // MAIN FUNCTIONS
        void invocare_forciam(bool pbd_flag = false);
        void compute_call(float time_step = DEFAULT_TIME_STEP, bool pbd_flag = false,plane *plane1 = nullptr, sphere *sphere1 = nullptr, sphere *sphere2 = nullptr);
        mesh get_mesh();
        std::vector<std::vector<glm::vec3>> sphere_cloth_collision(sphere &sphere1); // A impulse matrice for cloth, and updated omega and velocity for the sphere
        std::vector<std::vector<glm::vec3>> plane_cloth_collision(plane &plane1); // A impulse matrice for cloth, and updated omega and velocity for the sphere

};

class plane{
    public:
        std::pair<glm::vec3,float> plane_desc; // A normal and a float for a plane n.v = d
        void init_plane(std::pair<glm::vec3,float> plane_desc){
            this->plane_desc = plane_desc;
        }
        std::vector<glm::vec3> plane_boundaries(glm::vec3 plane_center, float plane_size);
        // GETTERS
        glm::vec3 get_plane_normal() const {
            return glm::normalize(plane_desc.first);
        }

};

class sphere{
    private:
        bool setup_sphere = false;
        std::vector<glm::vec3> vertices_buf;
        std::vector<glm::vec3> normals_buf;
        std::vector<glm::ivec3> triangles_buf;
        std::vector<glm::ivec2> edges_buf;
        std::pair<glm::vec3,glm::vec3> changes = std::make_pair(glm::vec3(0,0,0),glm::vec3(0,0,0));
    public:
        glm::vec3 center;
        float radius;
        glm::vec3 velocity;
        glm::vec3 omega;
        float mass;
        void init_sphere(glm::vec3 center, float radius, glm::vec3 velocity, glm::vec3 omega, float mass = 5.0f){
            this->center = center;
            this->radius = radius;
            this->velocity = velocity;
            this->omega = omega;
            this->mass = mass;
        }
        mesh get_mesh();
        void compute_call(float time_step = DEFAULT_TIME_STEP){
            glm::vec3 center_movement = velocity*time_step;
            glm::vec3 angular_movement = omega*time_step;
            changes = std::make_pair(center_movement,angular_movement);
        }
};
