#include "../revamp_src/classes/scene.hpp"
#include <iostream>
#include <omp.h>
#include <vector>

int main() {
    // Window and rendering settings
    const int width = 900;
    const int height = 600;
    const std::string outputFilename = "e1.png";

    // Create image and scene
    HDRImage image(width, height);
    Scene scene;

    // Camera
    // Initialize the camera
    scene.start_camera(height, width, 70.0f,
        glm::vec3(0, 0, 0),  // Moved camera 
        glm::vec3(0, 0, -1),    // Looking direction
        glm::vec3(0, 1, 0)     // Up vector
    );

    int shape_id = 0;
    // Unit sphere at 0,0,-2

    Sphere* sphere_1 = new Sphere(glm::vec3(0, 0, -2), 1.0, shape_id);
    Material* sphere_1_mat = new Lambertian(glm::vec3(1.0,1.0,1.0));
    Object* sphere_1_obj = new Object(sphere_1, sphere_1_mat);
    scene.objects.push_back(sphere_1_obj);
    shape_id++;

    // Large sphere at 0,-101,-2
    Sphere* sphere_2 = new Sphere(glm::vec3(0, -101, -2), 100.0,shape_id);
    Material* sphere = new Lambertian(glm::vec3(1.0,1.0,1.0));
    Object* sphere_2_obj = new Object(sphere_2, sphere);
    scene.objects.push_back(sphere_2_obj);
    shape_id++;

    // Render
    scene.render_normal_visualization(image, false);

    // Save

    std::cout << "Rendering complete" << std::endl;
    SDL_Surface *out = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    tonemap(image, out, 1, 2.2f);
    IMG_SavePNG(out, outputFilename.c_str());
    SDL_FreeSurface(out);
    std::cout << "Image saved to " << outputFilename << std::endl;

}