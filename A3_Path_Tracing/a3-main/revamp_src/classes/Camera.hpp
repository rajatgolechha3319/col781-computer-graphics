#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <utility>
#include "Ray.hpp"

class Camera {
    private:
    float fov;
    glm::vec3 view_dir;
    glm::vec3 up;
    glm::vec3 right;
    std::pair<float, float> camera_plane_size;
    float tan_theta_by_2;
    void target_lock(glm::vec3& eye, float& fov);
    
    public:
    glm::vec3 eye;
    std::pair<int,int> buffer_size;
    void init_camera(int height, int width, float fov, glm::vec3 eye, glm::vec3 view_dir, glm::vec3 up);
    Ray make_ray(float x, float y) const;

    void rotate_gyro(float angle, glm::vec3 axis = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f));
    void translate_cam(glm::vec3 new_eye);
    std::pair<float, float> get_camera_plane_coords(int i, int j);
};

