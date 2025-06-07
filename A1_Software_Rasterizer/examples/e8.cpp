#include "../src/a1.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace R = COL781::Hardware;
// namespace R = COL781::Software;

using namespace glm;

int main() {
    R::Rasterizer r;

    int width = 800, height = 640;

    if (!r.initialize("Example 8", width, height))
        return EXIT_FAILURE;

    R::ShaderProgram program = r.createShaderProgram(
        r.vsNormalTransform(),
        r.fsSpecularLighting()
    );

    // Platform vertices (XZ plane, Y=0)
    float vertices[] = {
        -2.0f, 0.0f, -2.0f, 1.0f,
        2.0f, 0.0f, -2.0f, 1.0f,
        -2.0f, 0.0f, 2.0f, 1.0f,
        2.0f, 0.0f, 2.0f, 1.0f
    };

    // All normals point upward (Y+)
    float normals[] = {
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };

    // Two triangles forming a quad
    int triangles[] = {
        0, 1, 2,
        1, 3, 2
    };

    // Cuboid rod vertices (X: ±0.2, Y: 0-2.5, Z: ±0.2)
    float vertices2[] = {
        // Front face (Z=-0.2)
        -0.05f, 0.0f, -0.05f, 1.0f,
        0.05f, 0.0f, -0.05f, 1.0f,
        -0.05f, 2.5f, -0.05f, 1.0f,
        0.05f, 2.5f, -0.05f, 1.0f,

        // Back face (Z=0.2)
        -0.05f, 0.0f, 0.05f, 1.0f,
        0.05f, 0.0f, 0.05f, 1.0f,
        -0.05f, 2.5f, 0.05f, 1.0f,
        0.05f, 2.5f, 0.05f, 1.0f,

        // Left face (X=-0.2)
        -0.05f, 0.0f, -0.05f, 1.0f,
        -0.05f, 0.0f, 0.05f, 1.0f,
        -0.05f, 2.5f, -0.05f, 1.0f,
        -0.05f, 2.5f, 0.05f, 1.0f,

        // Right face (X=0.2)
        0.05f, 0.0f, -0.05f, 1.0f,
        0.05f, 0.0f, 0.05f, 1.0f,
        0.05f, 2.5f, -0.05f, 1.0f,
        0.05f, 2.5f, 0.05f, 1.0f,

        // Bottom face (Y=0)
        -0.05f, 0.0f, -0.05f, 1.0f,
        0.05f, 0.0f, -0.05f, 1.0f,
        -0.05f, 0.0f, 0.05f, 1.0f,
        0.05f, 0.0f, 0.05f, 1.0f,

        // Top face (Y=2.5)
        -0.05f, 2.5f, -0.05f, 1.0f,
        0.05f, 2.5f, -0.05f, 1.0f,
        -0.05f, 2.5f, 0.05f, 1.0f,
        0.05f, 2.5f, 0.05f, 1.0f
    };

    float normals2[] = {
        // Front face normals (Z-)
        0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f,

        // Back face normals (Z+)
        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,

        // Left face normals (X-)
        -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        // Right face normals (X+)
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        // Bottom face normals (Y-)
        0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,

        // Top face normals (Y+)
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f
    };

    int triangles2[] = {
        // Front face
        0, 1, 2,
        1, 3, 2,
        
        // Back face
        4, 5, 6,
        5, 7, 6,
        
        // Left face
        8, 9, 10,
        9, 11, 10,
        
        // Right face
        12, 13, 14,
        13, 15, 14,
        
        // Bottom face
        16, 17, 18,
        17, 19, 18,
        
        // Top face
        20, 21, 22,
        21, 23, 22
    };
    
    R::Object platform = r.createObject();
    r.setVertexAttribs(platform, 0, 4, 4, vertices);
    r.setVertexAttribs(platform, 1, 4, 3, normals);
    r.setTriangleIndices(platform, 2, triangles);

    R::Object windmill_rod = r.createObject();
    r.setVertexAttribs(windmill_rod, 0, 24, 4, vertices2);
    r.setVertexAttribs(windmill_rod, 1, 24, 3, normals2);
    r.setTriangleIndices(windmill_rod, 12, triangles2);

    r.enableDepthTest();

    // Lighting parameters
    vec3 ambientColor(0.25f, 0.25f, 0.3f);    // Soft blue-tinted ambient light
    vec3 specularColor(0.9f, 0.9f, 0.5f);      // ylowish specular highlights
    vec3 lightColor(1.0f, 0.98f, 0.92f);       // Slightly warm sunlight
    vec3 lightDir;

    vec3 light_color(0.85f, 0.8f, 0.7f);  //
    vec3 dark_color(0.45f, 0.35f, 0.25f);   // Dark taupe
    vec3 rod_color(0.9f, 0.9f, 0.85f);  

    // Create view and projection matrices
    mat4 view = lookAt(vec3(4.5f, 5.5f, 4.5f), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
    mat4 projection = perspective(radians(45.0f), (float)width/height, 0.1f, 100.0f);

    // Declare transformation matrices outside the loop
    mat4 rotation, transform, blade_rotation;

    while (!r.shouldQuit()) {
        float currentTime = SDL_GetTicks64() * 1e-3f;
        float blade_ticker = SDL_GetTicks64() * 2e-3f;
            r.clear(vec4(0.1f, 0.1f, 0.1f, 1.0f));
            r.useShaderProgram(program);
            
            // Animate light direction
            lightDir = normalize(vec3(sin(currentTime * 0.5f), 0.8f, cos(currentTime * 0.5f)));
            r.setUniform(program, "lightColor", lightColor);
            r.setUniform(program, "lightDir", lightDir);
            r.setUniform(program, "ambientColor", ambientColor);
            r.setUniform(program, "specularColor", specularColor);
            r.setUniform(program, "viewPos", vec3(3.0f, 2.5f, 3.0f));
            r.setUniform(program, "blinnpow", 32);
            
            // Common rotation for all pieces
            rotation = rotate(mat4(1.0f), radians(currentTime * 10.0f), vec3(0.0f, 1.0f, 0.0f));
            blade_rotation = rotate(mat4(1.0f), radians(-1.0f * blade_ticker * 15.0f), vec3(0.0f, 1.0f, 0.0f));
            // Create 16 smaller squares
            int row = 0;
            while (row < 4) {
                int col = 0;
                while (col < 4) {
                    // Calculate position
                    float x = -1.5f + col * 1.0f;
                    float z = -1.5f + row * 1.0f;
                    
                    // Alternate colors
                    vec3 tileColor = ((row + col) % 2 == 0) ? light_color : dark_color;
                    
                    // Create transformation matrix
                    transform = rotation *
                                translate(mat4(1.0f), vec3(x, 0.0f, z)) *
                                scale(mat4(1.0f), vec3(0.25f, 1.0f, 0.25f));
                    
                    // Set uniforms and draw
                    r.setUniform(program, "transform", projection * view * transform);
                    r.setUniform(program, "wsTransform", transform);
                    r.setUniform(program, "objectColor", tileColor);
                    r.drawObject(platform);
                    
                    col++;
                }
                row++;
            }
            
            // Draw windmill base
            transform = rotation * scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));
            // Set color to white
            r.setUniform(program, "objectColor", rod_color);
            r.setUniform(program, "transform", projection * view * transform);
            r.setUniform(program, "wsTransform", transform);
            r.drawObject(windmill_rod);

            // Now transform this to turbine
            transform = blade_rotation * rotation * translate(mat4(1.0f), vec3(0.0f, 2.5f, 0.0f)) * scale(mat4(1.0f), vec3(1.0f,0.04f,4.0f)) ;
            r.setUniform(program, "transform", projection * view * transform);
            r.setUniform(program, "wsTransform", transform);
            r.drawObject(windmill_rod);

            // Rotate turbine 90 degrees for next blade
            transform = rotate(mat4(1.0f), radians(90.0f), vec3(0.0f, 1.0f, 0.0f)) * transform;
            r.setUniform(program, "transform", projection * view * transform);
            r.setUniform(program, "wsTransform", transform);
            r.drawObject(windmill_rod);

            r.show();
    }

    r.deleteShaderProgram(program);
    return EXIT_SUCCESS;
}
