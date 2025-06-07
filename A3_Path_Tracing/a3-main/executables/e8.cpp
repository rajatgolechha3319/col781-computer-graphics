#include "../revamp_src/classes/scene.hpp"
#include <iostream>
#include <omp.h>
#include <vector>

int main() {
    // Window and rendering settings
    const int width = 900;
    const int height = 600;
    const std::string outputFilename = "e8.png";
    glm::vec3 light_gray = glm::vec3(0.7f, 0.7f, 0.7f);
    glm::vec3 sun_set = glm::vec3(0.855f, 0.796f, 0.490f);

    // Create image and scene
    HDRImage image(width, height);
    Scene scene;

    // Set up camera
    scene.start_camera(height, width, 90.0f,
        glm::vec3(-3, 5, 15.68034f),  // Camera position
        glm::vec3(0, 0, -1),             // Look-at point
        glm::vec3(0, 1, 0)               // Up vector
    );

    int shape_id = 0;

    // Add ground plane
    Inf_Plane* base = new Inf_Plane(glm::vec3(0, 1, 0), 0, shape_id++);
    Material* base_material = new Lambertian(light_gray);
    base_material->set_reflectivity(glm::vec3(0.0f, 0.0f, 0.0f));
    Object* base_object = new Object(base, base_material);
    scene.objects.push_back(base_object);

    // Define cube colors
    glm::vec3 colors[6] = {
        glm::vec3(1, 0, 0), // Red
        glm::vec3(0, 1, 0), // Green
        glm::vec3(0, 0, 1), // Blue
        glm::vec3(1, 1, 0), // Yellow
        glm::vec3(0, 1, 1), // Cyan
        glm::vec3(1, 0, 1)  // Magenta
    };

    // Add base layer cubes
    AA_BB* cube1 = new AA_BB(-6, -2, 0, 4, -2, 2, shape_id++);
    Material* cube1_material = new Lambertian(colors[0]);
    cube1_material->set_reflectivity(glm::vec3(0, 0, 0));
    Object* cube1_object = new Object(cube1, cube1_material);
    scene.objects.push_back(cube1_object);
    glm::mat4 cube1_transform = glm::rotate(glm::mat4(1.0f), glm::radians(15.0f), glm::vec3(0, 1, 0));
    scene.objects.back()->set_transform(cube1_transform);

    AA_BB* cube2 = new AA_BB(-2, 2, 0, 4, -2, 2, shape_id++);
    Material* cube2_material = new Lambertian(colors[1]);
    cube2_material->set_reflectivity(glm::vec3(0, 0, 0));
    Object* cube2_object = new Object(cube2, cube2_material);
    scene.objects.push_back(cube2_object);
    glm::mat4 cube2_transform = glm::rotate(glm::mat4(1.0f), glm::radians(-15.0f), glm::vec3(0, 1, 0));
    scene.objects.back()->set_transform(cube2_transform);

    AA_BB* cube3 = new AA_BB(2, 6, 0, 4, -2, 2, shape_id++);
    Material* cube3_material = new Lambertian(colors[2]);
    cube3_material->set_reflectivity(glm::vec3(0, 0, 0));
    Object* cube3_object = new Object(cube3, cube3_material);
    scene.objects.push_back(cube3_object);


    // Add middle layer cubes
    AA_BB* cube4 = new AA_BB(-4, 0, 4, 8, -2, 2, shape_id++);
    Material* cube4_material = new Lambertian(colors[3]);
    cube4_material->set_reflectivity(glm::vec3(0, 0, 0));
    Object* cube4_object = new Object(cube4, cube4_material);
    scene.objects.push_back(cube4_object);
    glm::mat4 cube4_transform = glm::rotate(glm::mat4(1.0f), glm::radians(15.0f), glm::vec3(0, 1, 0));
    scene.objects.back()->set_transform(cube4_transform);

    AA_BB* cube5 = new AA_BB(0, 4, 4, 8, -2, 2, shape_id++);
    Material* cube5_material = new Lambertian(colors[4]);
    cube5_material->set_reflectivity(glm::vec3(0, 0, 0));
    Object* cube5_object = new Object(cube5, cube5_material);
    scene.objects.push_back(cube5_object);
    glm::mat4 cube5_transform = glm::rotate(glm::mat4(1.0f), glm::radians(-15.0f), glm::vec3(0, 1, 0));
    scene.objects.back()->set_transform(cube5_transform);

    // Add top layer cube
    AA_BB* cube6 = new AA_BB(-2, 2, 8, 12, -2, 2, shape_id++);
    Material* cube6_material = new Lambertian(colors[5]);
    cube6_material->set_reflectivity(glm::vec3(0, 0, 0));
    Object* cube6_object = new Object(cube6, cube6_material);
    scene.objects.push_back(cube6_object);

    // Add metallic sphere
    Sphere* sphere = new Sphere(glm::vec3(15, 7, 7), 7, shape_id++);
    Material* sphere_material = new Lambertian(glm::vec3(0.9, 0.9, 0.9)); 
    sphere_material->set_reflectivity(glm::vec3(0.8, 0.8, 0.8));
    sphere_material->set_metallic_factor(glm::vec3(1, 1, 1));
    Object* sphere_object = new Object(sphere, sphere_material);
    scene.objects.push_back(sphere_object);

    // Add single point light source
    scene.lights.clear(); // Remove any existing lights
    PointLight light;
    light.location = glm::vec3(-5, 10, 10);
    light.intensity = sun_set * 500.0f;
    scene.lights.push_back(light);

    // Add a small spherical light in the background of colour yellow
    // Sphere* small_sphere = new Sphere(glm::vec3(-10, 10 , 10), 1, shape_id++);
    // Material* small_sphere_material = new Lambertian(glm::vec3(1, 1, 0));
    // small_sphere_material->set_reflectivity(glm::vec3(0, 0, 0));
    // small_sphere_material->set_metallic_factor(glm::vec3(0, 0, 0));
    // small_sphere_material->set_luminosity(glm::vec3(0.9, 0.9, 0.3) * 1000.0f);
    // Object* small_sphere_object = new Object(small_sphere, small_sphere_material);
    // scene.objects.push_back(small_sphere_object);

    scene.camera->rotate_gyro(-25.0f, glm::vec3(0, 1, 0), scene.camera->eye);

    // Render the scene
    std::cout << "Rendering..." << std::endl;
    scene.update_avg_depth(10);
    scene.update_sample_rate(60);
    scene.update_save_rate(12);
    scene.render_scene_pathtracer(image, true);

    // Save the output
    std::cout << "Rendering complete" << std::endl;
    SDL_Surface* out = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    tonemap(image, out, 1, 2.2f);
    IMG_SavePNG(out, outputFilename.c_str());
    SDL_FreeSurface(out);
    std::cout << "Image saved to " << outputFilename << std::endl;

    return 0;
}