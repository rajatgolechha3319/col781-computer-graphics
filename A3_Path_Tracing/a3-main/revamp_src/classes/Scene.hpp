#pragma once
#include <vector>
#include "Camera.hpp"
#include "Object.hpp"
#include "PointLight.hpp"
#include "../image/image.hpp"
#include <omp.h>
#include <cstdlib>
#include <ctime>
#define MAX_REC_DEPTH 25


class Scene {
private:
    glm::vec3 direct_helper(Ray direction, HitRecord& rec);
    int reflection_depth = 3;
    int avg_depth = 3;
    int sample_rate = 25;
    int save_rate = 5;
public:
    Camera* camera;
    std::vector<Object*> objects;
    std::vector<PointLight> lights;
    void update_reflection_depth(int depth) {reflection_depth = depth;}
    void update_avg_depth(int depth) {avg_depth = depth;}
    void update_sample_rate(int rate) {sample_rate = rate;}
    void update_save_rate(int rate) {save_rate = rate;}

    Scene() : camera(nullptr) {std::srand(static_cast<unsigned int>(std::time(0)));}
    ~Scene();

    void start_camera(int height, int width, float fov, glm::vec3 eye, glm::vec3 view_dir, glm::vec3 up);
    void render_scene_pathtracer(HDRImage &image, bool parallel=true);
    void render_scene_raytracer(HDRImage &image, bool parallel=true);
    void render_normal_visualization(HDRImage &image, bool parallel=true);
    // glm::vec3 compute_direct_illumination( Ray direction);
    glm::vec3 compute_intensity(Ray direction, int reflections);

    // Part 6
    glm::vec3 compute_intensity_sampling(Ray direction, const float cont, int recursion_depth);
};