#include "../revamp_src/classes/scene.hpp"
#include <iostream>
#include <omp.h>
#include <vector>

int main() {
    // Window and rendering settings
    const int width = 900;
    const int height = 600;
    const std::string outputFilename = "final_out.png";

    // Create image and scene
    HDRImage image(width, height);
    Scene scene;

    float inital_intensity = 50.0f;
    // glm::vec3 global_light = glm::vec3(1.0f, 1.0f, 1.0f);

    // Add light sources in the vector scene.lights
    for(int i=0;i<6;i++){
        PointLight light;
        light.location = glm::vec3(-15.0f + 5.0f*i, 15.0f, -10.0f);
        light.intensity = glm::vec3(1.0f, 1.0f, 1.0f) * inital_intensity;
        scene.lights.push_back(light);
    }
    scene.lights[0].intensity = glm::vec3(1.0f, 0.0f, 0.0f) * inital_intensity;
    scene.lights[2].intensity = glm::vec3(0.0f, 0.707f, 0.0f) * inital_intensity;
    scene.lights[2].intensity = glm::vec3(0.0f, 0.707f, 0.0f) * inital_intensity;
    scene.lights[5].intensity = glm::vec3(0.0f, 0.0f, 1.0f) * inital_intensity;

    // Initialize the camera
    scene.start_camera(height, width, 90.0f,
        glm::vec3(0, 12.5f, -20),  // Moved camera 
        glm::vec3(0, 0, 1),    // Looking direction
        glm::vec3(0, 1, 0)     // Up vector
    );

    // Create the scene objects (spheres)
    // Sphere sphere_1(glm::vec3(0, 0, -2), 1.0);      // Small sphere
    // Sphere sphere_2(glm::vec3(0, -101, -2), 100.0); // Large ground sphere
    // AA_BB box_1(5, 10, 5, 10, 5, 10); // Small box
    // AA_BB box_2(-10, -5, 5, 10, 5, 10); // Small box 2
    // Inf_Plane sphere_2(glm::vec3(0, 1, 0), -1); // Large ground plane

    // std::vector<Object> shapes;

    // Ground plane (unchanged)
    // Inf_Plane* pl1_shape = new Inf_Plane(glm::vec3(0, 1, 0), -1);
    // Material* pl1_mat = nullptr;
    // Object pl1(pl1_shape, pl1_mat);
    // shapes.push_back(pl1);

    // Ellipse centered at origin
    // Three_Aces* ta1_shape = new Three_Aces(
    //     glm::vec3(0.25f, 0.36f, 0.16f),
    //     -1.0f  // c parameter
    // );
    // Material* ta1_mat = nullptr;
    // Object ta1(ta1_shape, ta1_mat);
    // shapes.push_back(ta1);

    // // // Sphere centered at (4.5 , 0 , 0) and radius 1.25
    // Sphere* sp1_shape = new Sphere(glm::vec3(4.5f, 0.0f, 0.0f), 1.25f);
    // Material* sp1_mat = nullptr;
    // Object sp1(sp1_shape, sp1_mat);
    // shapes.push_back(sp1);


    // // Sphere centered at (-4.5 , 0 , 0) and radius 1.25
    // Sphere* sp2_shape = new Sphere(glm::vec3(-4.5f, 0.0f, 0.0f), 1.25f);
    // Material* sp2_mat = nullptr;
    // Object sp2(sp2_shape, sp2_mat);
    // shapes.push_back(sp2);

    // PointLight light;
    // light.location = glm::vec3(0.0f, 14.50f, -10.50f);
    // light.intensity = glm::vec3(1.0f, 1.0f, 1.0f) * inital_intensity;
    // scene.lights.push_back(light);

    // PointLight light2;
    // light2.location = glm::vec3(5.0f, 4.50f, -10.50f);
    // light2.intensity = glm::vec3(1.0f, 1.0f, 1.0f) * inital_intensity;
    // scene.lights.push_back(light2);




    // // Inf_plane y =  - 1.5
    int shape_id = 0;
    Inf_Plane* pl1_shape = new Inf_Plane(glm::vec3(0, 1, 0), -1.5, shape_id);
    Material* pl1_mat = new Lambertian(glm::vec3(1.0,1.0,1.0));
    Object* pl1 = new Object(pl1_shape, pl1_mat);
    scene.objects.push_back(pl1);
    scene.objects[0]->mat->set_reflectivity(glm::vec3(0.75f));
    shape_id++;

    // // METALLIC SPHERE
    // Sphere* sp1_shape = new Sphere(glm::vec3(0.0f, 0.5f, 4.0f), 2.0f, shape_id);
    // Material* sp1_mat = new Lambertian(glm::vec3(0.7, 0.2, 0.5));
    // Object* sp1 = new Object(sp1_shape, sp1_mat);
    // scene.objects.push_back(sp1);
    // scene.objects[1]->mat->set_metallic_factor(0.7f);
    // shape_id++;

    // // A COPPER SPHERE
    // Sphere* sp2_shape = new Sphere(glm::vec3(0.0f, 0.5f, 4.0f), 2.0f, shape_id);
    // Material* sp2_mat = new Lambertian(glm::vec3(0.72, 0.45, 0.2));
    // Object* sp2 = new Object(sp2_shape, sp2_mat);
    // scene.objects.push_back(sp2);
    // scene.objects[2]->mat->set_metallic_factor(0.9f);
    // shape_id++;

    // // A GOLD SPHERE
    // Sphere* sp3_shape = new Sphere(glm::vec3(0.0f, 0.5f, 4.0f), 2.0f, shape_id);
    // Material* sp3_mat = new Lambertian(glm::vec3(0.83, 0.69, 0.22));
    // Object* sp3 = new Object(sp3_shape, sp3_mat);
    // scene.objects.push_back(sp3);
    // scene.objects[3]->mat->set_metallic_factor(0.9f);
    // shape_id++;


    // // Translate the 2nd sphere by (3,0,3)
    // glm::mat4 M = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, -3.0f));
    // scene.objects[2]->set_transform(M);

    // M = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 0.0f, -3.0f));
    // scene.objects[3]->set_transform(M);


    // AA_BB* box_shape = new AA_BB(-2,2,2,2.1,1,3, shape_id);
    // Material* box_mat = new Lambertian(glm::vec3(0.5, 0.5, 0.5));
    // Object* box = new Object(box_shape, box_mat);
    // scene.objects.push_back(box);
    // shape_id++;

    // // SET LIGHT
    // AA_BB* box_shape2 = new AA_BB(-2,2,4,4.1,1,3, shape_id);
    // Material* box_mat2 = new Lambertian(glm::vec3(0.5, 0.5, 0.5));
    // Object* box2 = new Object(box_shape2, box_mat2);
    // scene.objects.push_back(box2);
    // scene.objects[2]->mat->set_luminosity(glm::vec3(0.333f, 0.333f, 0.333f) * 50.0f);
    // shape_id++;


    
    // AA_BB* box_shape = new AA_BB(-20.0, 20.0, 11.5, 12.0 , 20.0 , 60.0, shape_id);
    // Material* box_mat = new Lambertian(glm::vec3(0.5, 0.5, 0.5));
    // Object* box = new Object(box_shape, box_mat);
    // scene.objects.push_back(box);
    // // scene.objects[shape_id]->mat->set_luminosity(glm::vec3(0.333f, 0.333f, 0.333f) * inital_intensity);
    // shape_id++;

    // A Sphere below the box (0,0.5,4) and radius 1
    // Sphere* sp1_shape = new Sphere(glm::vec3(0.0f, 0.5f, 4.0f), 1.0f, shape_id);
    // Material* sp1_mat = new Lambertian(glm::vec3(0.7, 0.2, 0.5));
    // Object* sp1 = new Object(sp1_shape, sp1_mat);
    // scene.objects.push_back(sp1);
    // shape_id++;



    // Buildings we create a set of apartments parallel to x axis of height -1.5 to 11.5 and width in z -1.5 to 1.5
    // The width in x direction is also 3.0 but we keep a gap of 2.0 between the buildings
    // First building starts at x = -15.0
    // for(int i=0; i<6; i++){
    //     AA_BB* box_shape = new AA_BB(-15.0 + 5.0*i, -12.0 + 5.0*i, -1.5, 11.5, -1.5, 1.5, shape_id);
    //     Material* box_mat = new Lambertian(glm::vec3(0.5, 0.5, 0.5));
    //     Object* box = new Object(box_shape, box_mat);
    //     scene.objects.push_back(box);
    //     shape_id++;
    // }

    // glm::mat4 M = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 1.0f, 1.0f));
    // scene.objects[3]->set_transform(M);


    // AA_BB* box_shape = new AA_BB(-15.0, 15.0, 12, 18, -12, -10, shape_id);
    // Material* box_mat = new Lambertian(glm::vec3(0.5, 0.5, 0.5));
    // Object* box = new Object(box_shape, box_mat);
    // scene.objects.push_back(box);
    // scene.objects[0]->mat->set_metallic_factor(0.5f);
    // scene.objects[4]->mat->set_luminosity(glm::vec3(0.1333f, 0.1333f, 0.1333f) * inital_intensity);
    // shape_id++;



    // scene.objects[3]->mat->set_luminosity(glm::vec3(0.0f, 0.0f, 0.1f) * inital_intensity);

    // Sphere* sp1_shape = new Sphere(glm::vec3(0.0f, 4.0f, 0.0f), 6.0f, shape_id);
    // Material* sp1_mat = new Lambertian(glm::vec3(0.5, 0.5, 0.5));
    // Object* sp1 = new Object(sp1_shape, sp1_mat);
    // scene.objects.push_back(sp1);
    // shape_id++;


    // Triangular_Mesh* tm1_shape = new Triangular_Mesh("/Users/darkelixir/Desktop/COL781/A3/graphics_a3_col781/a3-main/executables/bunny_1k.obj", shape_id);
    Sphere* tm1_shape = new Sphere(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, shape_id);
    // AA_BB* tm1_shape = new AA_BB(-1.0f, 1.0f, -1.5f, 9.0f, -1.0f, 1.0f, shape_id);
    Material* tm1_mat = new Lambertian(glm::vec3(0.5, 0.5, 0.5));
    Object* tm1 = new Object(tm1_shape, tm1_mat);
    scene.objects.push_back(tm1);
    glm::mat4 M = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 1.0f));
    scene.objects[shape_id]->set_transform(M);
    shape_id++;

    // glm::mat4 M = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    // glm::mat4 M = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 0.0f, 0.0f));
    // Scale in x by 2
    // glm::mat4 M = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 2.0f));
    // scene.objects[shape_id]->set_transform(M);

    // Scale by 5x in all directions
    // glm::mat4 M = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f));
    // scene.objects[1]->set_transform(M);


    // Render loop
    scene.render_scene_raytracer(image, true);
    std::cout << "Rendering complete" << std::endl;
    // Save the image to a file
    SDL_Surface *out = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    tonemap(image, out, 1, 2.2f);
    IMG_SavePNG(out, outputFilename.c_str());
    SDL_FreeSurface(out);
    std::cout << "Image saved to " << outputFilename << std::endl;


    // Affine transformations
    // Make a translation matrix to translate the object by (0,0,2)
    // glm::mat4 M = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    // scene.objects[3]->set_transform(M);
    // scene.render_scene(image, true);
    // std::cout << "Rendering complete" << std::endl;
    // out = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    // tonemap(image, out, 1, 2.2f);
    // IMG_SavePNG(out, "out_affine.png");
    // SDL_FreeSurface(out);

    // M = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    // scene.objects[3]->set_transform(M,false);
    // scene.render_scene(image, true);
    // std::cout << "Rendering complete" << std::endl;
    // out = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    // tonemap(image, out, 1, 2.2f);
    // IMG_SavePNG(out, "out_affine_2.png");
    // SDL_FreeSurface(out);

    // Rotate camera by 15 degrees about x axis
    // scene.camera->rotate_gyro(15.0f, glm::vec3(0.0f, 1.0f, 0.0f), scene.camera->eye);
    // scene.render_scene(image, true);
    // std::cout << "Rendering complete" << std::endl;
    // std::string new_op_file = "out_rotated.png";
    // out = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    // tonemap(image, out, 1, 2.2f);
    // IMG_SavePNG(out, new_op_file.c_str());
    // SDL_FreeSurface(out);
    // std::cout << "Image saved to " << new_op_file << std::endl;





//    // Move camera ahead in z by 2 units
//     for(int i=0;i<5;i++){
//         scene.camera->translate_cam(glm::vec3(0, 8.5f, -20 + 2.0f*(i+1)));
//         scene.render_scene(image, true);
//         std::cout << "Rendering complete" << std::endl;
//         std::string new_op_file = "out" + std::to_string(i + 1) + ".png";
//         out = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
//         tonemap(image, out, 1, 2.2f);
//         IMG_SavePNG(out, new_op_file.c_str());
//         SDL_FreeSurface(out);
//         std::cout << "Image saved to " << new_op_file << std::endl;
//     }


}