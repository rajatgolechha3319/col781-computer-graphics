#include "../revamp_src/classes/scene.hpp"
#include <iostream>
#include <omp.h>
#include <vector>

int main() {
    // Window and rendering settings
    const int width = 900;
    const int height = 600;
    const std::string outputFilename = "e7.png";
    glm::vec3 off_white = glm::vec3(0.980f, 0.976f, 0.9647f);
    glm::vec3 new_yellow = glm::vec3(0.9411f,0.988f,0.012f);
    glm::vec3 light_blue = glm::vec3(0.529f, 0.808f, 0.922f);
    // Create image and scene
    HDRImage image(width, height);
    Scene scene;


    // Add a small point light for global illumination
    PointLight light_1;
    light_1.location = glm::vec3(0.0f, 23.9f, 0.0f);
    light_1.intensity = glm::vec3(1.0f, 1.0f, 1.0f) * 8.0f;
    scene.lights.push_back(light_1);



    // Camera
    // Initialize the camera
    scene.start_camera(height, width, 90.0f,
        glm::vec3(0, 12.5f, 22.68034f),  // Moved camera 
        glm::vec3(0, 0, -1),    // Looking direction
        glm::vec3(0, 1, 0)     // Up vector
    );

    int shape_id = 0;

    // Left Wall
    Inf_Plane* left_wall = new Inf_Plane(glm::vec3(1, 0, 0), -15.f, shape_id);
    Material* left_wall_mat = new Lambertian(glm::vec3(0.66, 1.0, 0.0));
    left_wall_mat->set_reflectivity(glm::vec3(0.0, 0.0, 0.0));
    Object* left_wall_obj = new Object(left_wall, left_wall_mat);
    scene.objects.push_back(left_wall_obj);
    shape_id++;

    // Right Wall
    Inf_Plane* right_wall = new Inf_Plane(glm::vec3(1, 0, 0), 15.f, shape_id);
    Material* right_wall_mat = new Lambertian(glm::vec3(0.933, 0.294, 0.1686));
    right_wall_mat->set_reflectivity(glm::vec3(0.0, 0.0, 0.0));
    Object* right_wall_obj = new Object(right_wall, right_wall_mat);
    scene.objects.push_back(right_wall_obj);
    shape_id++;

    // Back Wall
    Inf_Plane* back_wall = new Inf_Plane(glm::vec3(0, 0, 1), -12.5f, shape_id);
    Material* back_wall_mat = new Lambertian(off_white);
    back_wall_mat->set_reflectivity(glm::vec3(0.0, 0.0, 0.0));
    Object* back_wall_obj = new Object(back_wall, back_wall_mat);
    scene.objects.push_back(back_wall_obj);
    shape_id++;

    // Floor
    Inf_Plane* floor = new Inf_Plane(glm::vec3(0, 1, 0), 0.0f, shape_id);
    Material* floor_mat = new Lambertian(off_white);
    floor_mat->set_reflectivity(glm::vec3(0.0, 0.0, 0.0));
    Object* floor_obj = new Object(floor, floor_mat);
    scene.objects.push_back(floor_obj);
    shape_id++;

    // Ceiling
    Inf_Plane* ceiling = new Inf_Plane(glm::vec3(0, 1, 0), 25.f, shape_id);
    Material* ceiling_mat = new Lambertian(off_white);
    ceiling_mat->set_reflectivity(glm::vec3(0.0, 0.0, 0.0));
    Object* ceiling_obj = new Object(ceiling, ceiling_mat);
    scene.objects.push_back(ceiling_obj);
    shape_id++;


    // Light Source on Ceiling centre in a 5*5*1 box
    AA_BB* light_box = new AA_BB(-2.5f, 2.5f, 24.0f, 25.0f, -2.5f, 2.5f, shape_id);
    Material* light_box_mat = new Lambertian(glm::vec3(1.0f, 1.0f, 1.0f));
    light_box_mat->set_luminosity(glm::vec3(1.0f, 1.0f, 1.0f) * 5.0f);
    Object* light_box_obj = new Object(light_box, light_box_mat);
    scene.objects.push_back(light_box_obj);
    shape_id++;

    // Sphere on the floor near left back corner
    Sphere* sphere_1 = new Sphere(glm::vec3(-5.0f, 5.f, -4.0f), 5.f, shape_id);
    Material* sphere_1_mat = new Lambertian(light_blue);
    sphere_1_mat->set_reflectivity(glm::vec3(0.3, 0.3, 0.3));
    sphere_1_mat->set_metallic_factor(glm::vec3(0.9, 0.9, 0.9));
    Object* sphere_1_obj = new Object(sphere_1, sphere_1_mat);
    scene.objects.push_back(sphere_1_obj);
    shape_id++;

    // Add a second sphere on the floor near right front mid
    Sphere* sphere_2 = new Sphere(glm::vec3(5.0f, 5.f, 4.0f), 5.f, shape_id);
    Material* sphere_2_mat = new Lambertian(new_yellow);
    sphere_2_mat->set_reflectivity(glm::vec3(0.3, 0.3, 0.3));
    sphere_2_mat->set_metallic_factor(glm::vec3(0.9, 0.9, 0.9));
    Object* sphere_2_obj = new Object(sphere_2, sphere_2_mat);
    scene.objects.push_back(sphere_2_obj);
    shape_id++;



    // Render
    scene.update_avg_depth(5);
    scene.update_sample_rate(1000);
    scene.update_save_rate(50);
    scene.render_scene_pathtracer(image, true);

    // Save

    std::cout << "Rendering complete" << std::endl;
    SDL_Surface *out = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    tonemap(image, out, 1, 2.2f);
    IMG_SavePNG(out, outputFilename.c_str());
    SDL_FreeSurface(out);
    std::cout << "Image saved to " << outputFilename << std::endl;

}