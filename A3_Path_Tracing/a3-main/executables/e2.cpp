#include "../revamp_src/classes/scene.hpp"
#include <iostream>
#include <omp.h>
#include <vector>

int main() {
    // Window and rendering settings
    const int width = 900;
    const int height = 600;
    const std::string outputFilename = "e2.png";

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

    // Ground plane
    Inf_Plane* ground_plane = new Inf_Plane(glm::vec3(0, 1, 0), -1, shape_id);
    Material* ground_plane_mat = new Lambertian(glm::vec3(1.0,1.0,1.0));
    Object* ground_plane_obj = new Object(ground_plane, ground_plane_mat);
    scene.objects.push_back(ground_plane_obj);
    shape_id++;

    // Boxes 
    AA_BB* box_1 = new AA_BB(-1.5, -1.25, -1, 0.5, -2, -1.75, shape_id);
    Material* box_1_mat = new Lambertian(glm::vec3(1.0,1.0,1.0));
    Object* box_1_obj = new Object(box_1, box_1_mat);
    scene.objects.push_back(box_1_obj);
    shape_id++;

    AA_BB* box_2 = new AA_BB(1.25, 1.7, -1, 0.8, -2, -1.75, shape_id);
    Material* box_2_mat = new Lambertian(glm::vec3(1.0,1.0,1.0));
    Object* box_2_obj = new Object(box_2, box_2_mat);
    scene.objects.push_back(box_2_obj);
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