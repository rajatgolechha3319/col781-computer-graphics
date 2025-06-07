#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <utility>
#include <iostream>
#include <vector>
#define  wh2(i, n) while (i < (n))

class Axis{
    public:
        glm::vec3 point;
        glm::vec3 direction;
        float angle = 0.0f;
        Axis() : point(glm::vec3(0.0f)), direction(glm::vec3(0.0f, 1.0f, 0.0f)), angle(0.0f) {}
        Axis(glm::vec3 point, glm::vec3 direction, float angle) : point(point), direction(direction), angle(angle) {}
};

class Quaternion{
    public:
        float s;
        glm::vec3 v;
        Quaternion(float s, glm::vec3 v) : s(s), v(v) {}
        void set(float s, glm::vec3 v) {
            this->s = s;
            this->v = v;
        }
        Quaternion mult(Quaternion a, Quaternion b);
};

class Skeletal_Box {
    private:
        glm::vec3 translation_vector = glm::vec3(0.0f);
        Quaternion rotation_quaternion = Quaternion(1.0f,glm::vec3(0.0f));
    public:
        // Variables
        Skeletal_Box* parent = nullptr; // Parent bone // Only root can have transformations
        Axis hinge_joint; // Connection to the parent using hinge joint ( In parent's system )
        glm::vec3 center; // In parent's system
        std::pair<std::pair<float,float>, float> lengths; // ((x,y), z)

        // MESH
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::ivec3> triangles;
        std::vector<glm::ivec2> edges;

        // Functions
        void move_object(glm::vec3 translation_vector);
        void rotate_object(Quaternion rotation_quaternion);
        void init();
        void compute_call(bool flag = false);

        // INITIALIZE
        void initialize_Box(Skeletal_Box* parent, std::pair<std::pair<float,float>, float> lengths, glm::vec3 center, Axis hinge_joint){
            this->parent = parent;
            if(parent == nullptr){
                hinge_joint.angle = 0.0f;
                hinge_joint.direction = glm::vec3(0.0f, 1.0f, 0.0f);
                hinge_joint.point = center;
            }
            this->lengths = lengths;
            this->center = center;
            this->hinge_joint = hinge_joint;
            this->init();
            if(parent == nullptr){
                this->compute_call(); // ROOT
            }
            else{
                if(parent->parent == nullptr){
                    this->compute_call(); // CHILD
                }
                else{
                    this->compute_call(true); // GRANDCHILD
                }
            }
        }

};