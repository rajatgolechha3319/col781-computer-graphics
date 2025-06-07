#include "Camera.hpp"
#include <cmath>
#include <iostream>
void cam_vec_set(glm::vec3& eye, glm::vec3& view_dir, glm::vec3& up, glm::vec3& view_dir_norm, glm::vec3& right_norm, glm::vec3& up_norm) {
    view_dir_norm = glm::normalize(view_dir);
    right_norm = glm::normalize(glm::cross(view_dir, up));
    up_norm = glm::normalize(glm::cross(right_norm, view_dir));
}

void Camera::target_lock(glm::vec3& eye, float& fov){
    // First compute angle in degrees
    float theta = fov *  3.14159265f / 180.0f;
    this->tan_theta_by_2 = tan(theta / 2.0f);
    this->fov = fov;
    this->eye = eye;
}

void Camera::init_camera(int height, int width, float fov, glm::vec3 eye, glm::vec3 view_dir, glm::vec3 up) {

    target_lock(eye, fov);    
    cam_vec_set(eye, view_dir, up, this->view_dir, this->right, this->up);
    
    this->buffer_size = std::make_pair(height, width);
    this->camera_plane_size = std::make_pair(this->tan_theta_by_2,  this->tan_theta_by_2 * ((float)width / (float)height) );
}

Ray Camera::make_ray(float x, float y) const {
    
    glm::vec3 ray_dir = glm::normalize(view_dir + x * camera_plane_size.second * right + y * camera_plane_size.first * up);
    return Ray(eye, ray_dir);
}

void Camera::rotate_gyro(float angle, glm::vec3 axis,glm::vec3 center){
    glm::mat4 Identity = glm::mat4(1.0f);
    float rad_angle = angle * M_PI / 180.0f;
    glm::mat4 rot_mat = glm::rotate(Identity, rad_angle, axis);
    glm::vec3 view_dir_ = glm::vec3(rot_mat * glm::vec4(view_dir, 0.0f));
    glm::vec3 up_ = glm::vec3(rot_mat * glm::vec4(up, 0.0f));

    // Eye center distance translation
    glm::vec3 eye_center = eye - center;
    if(glm::length(eye_center) > 1.23e-3){
        //Center is not at eye
        eye_center = glm::vec3(rot_mat * glm::vec4(eye_center, 0.0f));
        eye = eye_center + center;
    }

    view_dir = glm::normalize(view_dir_);
    right = glm::normalize(glm::cross(view_dir_, up_));
    up = glm::normalize(glm::cross(right, view_dir_));
}

void Camera::translate_cam(glm::vec3 new_eye){
    // Translate eye to new position
    eye = new_eye;
}

std::pair<float, float> Camera::get_camera_plane_coords(int i, int j){
    float x = 2 * (i + 0.5f) / buffer_size.second - 1;
    float y = 1 - 2 * (j + 0.5f) / buffer_size.first;
    return std::make_pair(x, y);
}