#include "sw.hpp"

#include <iostream>
#include <vector>
#include <cassert>  // For assert
#include <cmath> // For sqrt
#define wh(i, n) int i = 0; while (i < (n))
#define f(i,a,b) for(int i = a; i <= b; i++)
float pi = M_PI; // 3.14159265358979323846 // Used for energy conservation
// TODO : function for lock / unlock framebuffer DONE
// Faster than bounding box algo
// Make a helper function wait until quit. DONE


namespace COL781 {
	namespace Software {

		// Extra Helper Functions
		void Rasterizer :: lock_frame_buffer(int bno) {
			// return;
			if (bno == 1){
				if (SDL_MUSTLOCK(framebuffer)) {
					if (SDL_LockSurface(framebuffer) < 0) {
						std::cerr << "Unable to lock surface! SDL_Error: " << SDL_GetError() << std::endl;
						return;
					}
				}
				return;
			}
			if (bno == 2){
				if (SDL_MUSTLOCK(framebuffer_2)) {
					if (SDL_LockSurface(framebuffer_2) < 0) {
						std::cerr << "Unable to lock surface 2! SDL_Error: " << SDL_GetError() << std::endl;
						return;
					}
				}
				return;
			}
		}
		
		void Rasterizer :: unlock_frame_buffer(int bno) {
			// return;
			if (bno == 1){
				if (SDL_MUSTLOCK(framebuffer)) {
					SDL_UnlockSurface(framebuffer);
				}
				return;
			}
			if (bno == 2){
				if (SDL_MUSTLOCK(framebuffer_2)) {
					SDL_UnlockSurface(framebuffer_2);
				}
				return;
			}
		}

		void Rasterizer :: wait_or_quit() {
			SDL_Event e;
			while (SDL_PollEvent(&e) != 0) {
				if (e.type == SDL_QUIT) {
					quit = true;
				}
			}
			return;
		}
		// Forward declarations

		template <> float Attribs::get(int index) const;
		template <> glm::vec2 Attribs::get(int index) const;
		template <> glm::vec3 Attribs::get(int index) const;
		template <> glm::vec4 Attribs::get(int index) const;

		template <> void Attribs::set(int index, float value);
		template <> void Attribs::set(int index, glm::vec2 value);
		template <> void Attribs::set(int index, glm::vec3 value);
		template <> void Attribs::set(int index, glm::vec4 value);

		// Built-in shaders

		VertexShader Rasterizer::vsIdentity() {
			return [](const Uniforms &uniforms, const Attribs &in, Attribs &out) {
				glm::vec4 vertex = in.get<glm::vec4>(0);
				return vertex;
			};
		}

		VertexShader Rasterizer::vsTransform() {
			return [](const Uniforms &uniforms, const Attribs &in, Attribs &out) {
				glm::vec4 vertex = in.get<glm::vec4>(0);
				glm::mat4 transform = uniforms.get<glm::mat4>("transform");
				return transform * vertex;
			};
		}

		VertexShader Rasterizer::vsColor() {
			return [](const Uniforms &uniforms, const Attribs &in, Attribs &out) {
				glm::vec4 vertex = in.get<glm::vec4>(0);
				glm::vec4 color = in.get<glm::vec4>(1);
				out.set<glm::vec4>(0, color);
				return vertex;
			};
		}

		VertexShader Rasterizer::vsNormalTransform() {
			return [](const Uniforms& uniforms, const Attribs& in, Attribs& out) {
				glm::vec4 vertex = in.get<glm::vec4>(0);
				glm::vec3 normal = in.get<glm::vec3>(1); 
				glm::mat4 transform = uniforms.get<glm::mat4>("transform");
				glm::mat4 wsTransform = uniforms.get<glm::mat4>("wsTransform");
				glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(wsTransform)));

				// Call to frag shader
				out.set<glm::vec3>(0, normalMatrix * normal);
				out.set<glm::vec4>(1, wsTransform * vertex);
				
				// return
				return transform * vertex;
			};
		}

		VertexShader Rasterizer::vsColorTransform() {
			// Combined vscolor and vstransform
			return [](const Uniforms &uniforms, const Attribs &in, Attribs &out) {
				glm::vec4 vertex = in.get<glm::vec4>(0);
				glm::vec4 color = in.get<glm::vec4>(1);
				glm::mat4 transform = uniforms.get<glm::mat4>("transform");
				out.set<glm::vec4>(0, color);
				return transform * vertex;
			};
		}

		FragmentShader Rasterizer::fsConstant() {
			return [](const Uniforms &uniforms, const Attribs &in) {
				glm::vec4 color = uniforms.get<glm::vec4>("color");
				return color;
			};
		}

		FragmentShader Rasterizer::fsIdentity() {
			return [](const Uniforms &uniforms, const Attribs &in) {
				glm::vec4 color = in.get<glm::vec4>(0);
				return color;
			};
		}

		FragmentShader Rasterizer::fsDiffuseLighting() {
			return [](const Uniforms &uniforms, const Attribs &in) {
				glm::vec3 new_normal = in.get<glm::vec3>(0);
				glm::vec3 ambientColor = uniforms.get<glm::vec3>("ambientColor");
				glm::vec3 lightColor = uniforms.get<glm::vec3>("lightColor");
				glm::vec3 lightDir = uniforms.get<glm::vec3>("lightDir");
				glm::vec3 objectColor = uniforms.get<glm::vec3>("objectColor");

				// apply gamma correction
				ambientColor = glm::pow(ambientColor, glm::vec3(2.2f));
				lightColor = glm::pow(lightColor, glm::vec3(2.2f));
				objectColor = glm::pow(objectColor, glm::vec3(2.2f));

				float diff = glm::max(glm::dot(glm::normalize(new_normal), glm::normalize(lightDir)), 0.0f);

				glm::vec3 diffuse = diff * lightColor * objectColor;
				glm::vec3 linColor = ambientColor + diffuse;

				// apply gamma correction and return
				return glm::vec4(glm::pow(linColor, glm::vec3(1.0f / 2.2f)), 1.0f);

			};
		}

		FragmentShader Rasterizer::fsSpecularLighting() {
			return [](const Uniforms &uniforms, const Attribs &in) {
				glm::vec3 new_normal = glm::normalize(in.get<glm::vec3>(0));
				glm::vec4 vertex_pos = in.get<glm::vec4>(1);
				glm::vec3 ambientColor = uniforms.get<glm::vec3>("ambientColor");
				glm::vec3 lightColor = uniforms.get<glm::vec3>("lightColor");
				glm::vec3 lightDir = glm::normalize(uniforms.get<glm::vec3>("lightDir"));
				glm::vec3 objectColor = uniforms.get<glm::vec3>("objectColor");
				glm::vec3 viewPos = uniforms.get<glm::vec3>("viewPos");
				glm::vec3 specularColor = uniforms.get<glm::vec3>("specularColor");
				int blinnpow = uniforms.get<int>("blinnpow");
				// std::cout << blinnpow << std::endl;
				// apply gamma correction
				ambientColor = glm::pow(ambientColor, glm::vec3(2.2f));
				lightColor = glm::pow(lightColor, glm::vec3(2.2f));
				objectColor = glm::pow(objectColor, glm::vec3(2.2f));
				specularColor = glm::pow(specularColor, glm::vec3(2.2f));

				glm::vec3 viewDir = glm::normalize(viewPos - glm::vec3(vertex_pos));
				glm::vec3 bisector = glm::normalize(lightDir + viewDir);

				float diff = glm::max(glm::dot(new_normal, lightDir), 0.0f);
				float spec = glm::pow(glm::max(glm::dot(new_normal, bisector), 0.0f), blinnpow);

				glm::vec3 diffuse = diff * lightColor * objectColor;
				glm::vec3 specular = spec * lightColor * specularColor;

				glm::vec3 linColor = (ambientColor * pi + diffuse + specular) / pi;

				return glm::vec4(glm::pow(linColor, glm::vec3(1.0f / 2.2f)), 1.0f);
			};
		}

		// Implementation of Attribs and Uniforms classes

		void checkDimension(int index, int actual, int requested) {
			if (actual != requested) {
				std::cout << "Warning: attribute " << index << " has dimension " << actual << " but accessed as dimension " << requested << std::endl;
			}
		}

		template <> float Attribs::get(int index) const {
			checkDimension(index, dims[index], 1);
			return values[index].x;
		}

		template <> glm::vec2 Attribs::get(int index) const {
			checkDimension(index, dims[index], 2);
			return glm::vec2(values[index].x, values[index].y);
		}

		template <> glm::vec3 Attribs::get(int index) const {
			checkDimension(index, dims[index], 3);
			return glm::vec3(values[index].x, values[index].y, values[index].z);
		}

		template <> glm::vec4 Attribs::get(int index) const {
			checkDimension(index, dims[index], 4);
			return values[index];
		}

		int Attribs::get_num_dims() const {
			return dims.size();
		}

		int Attribs::get_dimension_i(int index) const {
			if (index < 0 || index >= dims.size() ) {
				throw std::out_of_range("Index out of bounds");
			}
			return dims[index];
		}

		void expand(std::vector<int> &dims, std::vector<glm::vec4> &values, int index) {
			if (dims.size() < index+1)
				dims.resize(index+1);
			if (values.size() < index+1)
				values.resize(index+1);
		}

		template <> void Attribs::set(int index, float value) {
			expand(dims, values, index);
			dims[index] = 1;
			values[index].x = value;
		}

		template <> void Attribs::set(int index, glm::vec2 value) {
			expand(dims, values, index);
			dims[index] = 2;
			values[index].x = value.x;
			values[index].y = value.y;
		}

		template <> void Attribs::set(int index, glm::vec3 value) {
			expand(dims, values, index);
			dims[index] = 3;
			values[index].x = value.x;
			values[index].y = value.y;
			values[index].z = value.z;
		}

		template <> void Attribs::set(int index, glm::vec4 value) {
			expand(dims, values, index);
			dims[index] = 4;
			values[index] = value;
		}

		template <typename T> T Uniforms::get(const std::string &name) const {
			return *(T*)values.at(name);
		}

		template <typename T> void Uniforms::set(const std::string &name, T value) {
			auto it = values.find(name);
			if (it != values.end()) {
				delete it->second;
			}
			values[name] = (void*)(new T(value));
		}

		template <typename T>
		void Rasterizer::setUniform(ShaderProgram &program, const std::string &name, T value) {
			program.uniforms.set(name, value);
			// Call use shader program here
			useShaderProgram(program);
			// DO NOT REMOVE THE ABOVE LINE 
			// EVERYTHING WILL BREAK
		}

		// Explicitly instantiate template for allowed types
		template void Rasterizer::setUniform<float>(ShaderProgram&, const std::string&, float);
		template void Rasterizer::setUniform<int>(ShaderProgram&, const std::string&, int);
		template void Rasterizer::setUniform<glm::vec2>(ShaderProgram&, const std::string&, glm::vec2);
		template void Rasterizer::setUniform<glm::vec3>(ShaderProgram&, const std::string&, glm::vec3);
		template void Rasterizer::setUniform<glm::vec4>(ShaderProgram&, const std::string&, glm::vec4);
		template void Rasterizer::setUniform<glm::mat2>(ShaderProgram&, const std::string&, glm::mat2);
		template void Rasterizer::setUniform<glm::mat3>(ShaderProgram&, const std::string&, glm::mat3);
		template void Rasterizer::setUniform<glm::mat4>(ShaderProgram&, const std::string&, glm::mat4);
		

		// Initialize the rasterizer
		bool Rasterizer::initialize(const std::string &title, int width, int height, int spp) {
			if (SDL_Init(SDL_INIT_VIDEO) < 0) {
				std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
				return false;
			}
			int win_ht = height;
			int win_wd = width;

			window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, win_wd, win_ht, SDL_WINDOW_SHOWN);

			if (!window) {
				std::cerr << "Could not create window: " << SDL_GetError() << std::endl;
				return false;
			}
			// Assign variables here
			window_wd = width;
			window_ht = height;
			quit = false;
			window_spp = int(std::sqrt(spp));
			std::cout << " Window spp is "<< window_spp << std::endl;
			depth_flag = false;


			// Create the framebuffer
			framebuffer = SDL_CreateRGBSurface(0, window_wd, window_ht, 32, 0, 0, 0, 0);
			if(spp != 1){
				int window_wd_spp = window_wd * window_spp;
				int window_ht_spp = window_ht * window_spp;
				framebuffer_2 = SDL_CreateRGBSurface(0, window_wd_spp, window_ht_spp, 32, 0, 0, 0, 0);
				if (framebuffer_2 == NULL) {
					std::cerr << "Could not create framebuffer_2: " << SDL_GetError() << std::endl;
					return false;
				}
				window_wd = window_wd_spp;
				window_ht = window_ht_spp;
			}
			if (framebuffer == NULL) {
        		std::cerr << "Could not create framebuffer: " << SDL_GetError() << std::endl;
        		return false;
    		}
			
			std::cout << " Init done " << std::endl;

			return true;
			
		}

		bool Rasterizer::shouldQuit() {
			return quit;
		}

		void Rasterizer::useShaderProgram(const ShaderProgram &program) {
			shader_for_rasterizer = program;
		}


		void Rasterizer::deleteShaderProgram(ShaderProgram &program) {
			// Delete the program
			program.vs = NULL;
			program.fs = NULL;
			program.uniforms = Uniforms(); // NULL wasn't working here !!
			return;
		}

		ShaderProgram Rasterizer::createShaderProgram(const VertexShader &vs, const FragmentShader &fs) {
			// Instantiate a new ShaderProgram object
			ShaderProgram new_shader;
			new_shader.vs = vs;
			new_shader.fs = fs;
			new_shader.uniforms = Uniforms();
			return new_shader;
		}

		Object Rasterizer::createObject() {
			// Instantiate a new Object
			Object new_object;
			return new_object;
		}

		// Sets the data for the i'th vertex attribute.
		// data is only allowed to be float array only. For different dimensions vectors glm::vec2, glm::vec3, or glm::vec4 enter corresponding dimensions as d.
		void Rasterizer::setVertexAttribs(Object &object, int attribIndex, int n, int d, const float* data){
			// Resize if smaller
			// std::cout << "Setting Vertex Attributes" << std::endl;
			if (object.vertexAttributes.size() < n) {
				object.vertexAttributes.resize(n);
			}
			// Set the data 
			// Can later change to while loop instead of for loop
			// Make if else according to dimensions or case switch
			// d=1:float, dim=2:vec2, dim=3:vec3, dim=4:vec4
			if(d <= 2){
				if(d == 1){
					wh(i, n){
						object.vertexAttributes[i].set<float>(attribIndex, data[i]);
						i++;
					}
				}
				else{
					const glm::vec2* vec2_data = reinterpret_cast<const glm::vec2*>(data);
					wh(i,n) {
						object.vertexAttributes[i].set<glm::vec2>(attribIndex, vec2_data[i]);
						i++;
					}
				}
			}
			else{
				if(d == 3){
					const glm::vec3* vec3_data = reinterpret_cast<const glm::vec3*>(data);
					wh(i,n) {
						object.vertexAttributes[i].set<glm::vec3>(attribIndex, vec3_data[i]);
						i++;
					}
				}
				else{
					const glm::vec4* vec4_data = reinterpret_cast<const glm::vec4*>(data);
					wh(i,n) {
						object.vertexAttributes[i].set<glm::vec4>(attribIndex, vec4_data[i]);
						i++;
					}
				}
			}
		}

		void Rasterizer::setTriangleIndices(Object &object, int n, int* indices){
			// std::cout << "Setting Triangle Indices" << std::endl;
			// Resize if smaller
			if (object.indices.size() < n) {
				object.indices.resize(n);
			}
			// Set the data
			// Can later change to while loop instead of for loop for universality of macro // DONE
			wh(i, n){
				object.indices[i] = glm::ivec3(indices[3*i], indices[3*i+1], indices[3*i+2]);
				i++;
			}
			
		}

		void Rasterizer::enableDepthTest() {
			depth_flag = true;
			return;
		}

		Mvec<uint8_t> color_from_glm(glm::vec4 color) {
			Mvec<uint8_t> color_vec(4);
			color_vec[0] = (uint8_t)(color[0] * 255);
			color_vec[1] = (uint8_t)(color[1] * 255);
			color_vec[2] = (uint8_t)(color[2] * 255);
			color_vec[3] = (uint8_t)(color[3] * 255);
			return color_vec;
		}
			
		
		void Rasterizer::clear(glm::vec4 color) {
			// std::cout << "Clearing" << std::endl;
			Mvec<uint8_t> color_vec = color_from_glm(color);

			// SDL_FillRect(framebuffer, NULL, SDL_MapRGBA(framebuffer->format, r, g, b, a));

			Uint32 sdl_color = SDL_MapRGBA(framebuffer->format, color_vec[0], color_vec[1], color_vec[2], color_vec[3]);

			// Lock the surface
			lock_frame_buffer(1);

			// Fill the surface
			SDL_FillRect(framebuffer, NULL, sdl_color);

			// Unlock the surface
			unlock_frame_buffer(1);

			// clear the zbuffer
			if(depth_flag){
				z_buffer.clear();
			}

			if (window_spp != 1){
				sdl_color = SDL_MapRGBA(framebuffer_2->format, color_vec[0], color_vec[1], color_vec[2], color_vec[3]);
				lock_frame_buffer(2);
				SDL_FillRect(framebuffer_2, NULL, sdl_color);
				unlock_frame_buffer(2);
			}

		}


		// Function to get the area of a triangle in two dimensions given the 3 vertices as input
		float area_triangle(const glm::ivec2 &v1, const glm::ivec2 &v2, const glm::ivec2 &v3) {
			return 0.5f * std::abs(
				v1.x * (v2.y - v3.y) +
				v2.x * (v3.y - v1.y) +
				v3.x * (v1.y - v2.y)
			);
		}


		bool in_triangle(const glm::ivec2 &v1, const glm::ivec2 &v2, const glm::ivec2 &v3, const glm::vec2 &pt){
			// Check whether v1 and pt are on the same side or not
			float s1 = (v1.x - v2.x) * (v3.y - v2.y) - (v1.y - v2.y) * (v3.x - v2.x);
			float s2 = (pt.x - v2.x) * (v3.y - v2.y) - (pt.y - v2.y) * (v3.x - v2.x);
			if(s1 * s2 < 0){
				return false;
			}
			// Check whether v2 and pt are on the same side or not
			s1 = (v2.x - v1.x) * (v3.y - v1.y) - (v2.y - v1.y) * (v3.x - v1.x);
			s2 = (pt.x - v1.x) * (v3.y - v1.y) - (pt.y - v1.y) * (v3.x - v1.x);
			if(s1 * s2 < 0){
				return false;
			}
			// Check whether v3 and pt are on the same side or not
			s1 = (v3.x - v1.x) * (v2.y - v1.y) - (v3.y - v1.y) * (v2.x - v1.x);
			s2 = (pt.x - v1.x) * (v2.y - v1.y) - (pt.y - v1.y) * (v2.x - v1.x);
			if(s1 * s2 < 0){
				return false;
			}
			return true;
		}

		Mvec<glm::ivec2> Rasterizer::bounding_box(const glm::ivec2 &v1, const glm::ivec2 &v2, const glm::ivec2 &v3){
			// Get the bounding box
			// First let's get the x boundaries
			glm::ivec2 x_bounds = glm::ivec2(
				std::max(0, std::min(v1.x, std::min(v2.x, v3.x))),
				std::min(window_wd-1, std::max(v1.x, std::max(v2.x, v3.x)))
			);
			// Now let's get the y boundaries
			glm::ivec2 y_bounds = glm::ivec2(
				std::max(0, std::min(v1.y, std::min(v2.y, v3.y))),
				std::min(window_ht-1, std::max(v1.y, std::max(v2.y, v3.y)))
			);
			// Now we have the bounding box
			return Mvec<glm::ivec2>{x_bounds, y_bounds};
		}

		void Rasterizer::draw_helper(const glm::ivec2 &v1, const Object &obj, ShaderProgram &shader_pg, Uint32 *pixels){

			// TODO improve the code using the algo taught in class
			// the v1 contains the index and the pitch
			Mvec<int> vertices;
			vertices.push_back(obj.indices[v1.x].x);
			vertices.push_back(obj.indices[v1.x].y);
			vertices.push_back(obj.indices[v1.x].z);

			// Get the attributes for the 3 vertices
			Mvec<Attribs> vertex_attributes(3);
			Mvec<glm::vec4> vertex_coords(3);
			wh(i1,3){
				vertex_coords[i1] = shader_pg.vs(shader_pg.uniforms, obj.vertexAttributes[vertices[i1]], vertex_attributes[i1]);
				i1++;
			}

			// Now convert to screen space the coordinates
			Mvec<glm::vec3> vertex_coords_screen(3);
			wh(i2,3){
				vertex_coords_screen[i2] = glm::vec3(vertex_coords[i2]) / vertex_coords[i2].w;
				i2++;
			}

			// Now get x,y coordinates in the framebuffer
			Mvec<glm::ivec2> vertex_coords_framebuffer(3);
			wh(i3,3){
				vertex_coords_framebuffer[i3] = glm::ivec2(
					static_cast<int>((vertex_coords_screen[i3].x + 1.0f) * 0.5f * window_wd),
					static_cast<int>((1.0f - vertex_coords_screen[i3].y) * 0.5f * window_ht)
				);
				i3++;
			}

			// Now let's get the bounding box
			Mvec<glm::ivec2> bound_box = bounding_box(vertex_coords_framebuffer[0], vertex_coords_framebuffer[1], vertex_coords_framebuffer[2]);

			// Let's iterate over the bounding box and check if the point is inside the triangle or not
			f(pt_x,bound_box[0].x,bound_box[0].y){
				f(pt_y,bound_box[1].x,bound_box[1].y){
					const glm::vec2 pt(pt_x + 0.5f, pt_y + 0.5f);
					bool inside = in_triangle(vertex_coords_framebuffer[0], vertex_coords_framebuffer[1], vertex_coords_framebuffer[2], pt);
					if(!inside){
						continue;
					}
					// Compute area of triangle
					float area_total = area_triangle(vertex_coords_framebuffer[0], vertex_coords_framebuffer[1], vertex_coords_framebuffer[2]);
					if(area_total <= 1.42e-6){
						continue;
					}
					// Compute areas of the 3 sub-triangles
					Mvec<float> areas(3);
					wh(i4,3){
						areas[i4] = area_triangle(vertex_coords_framebuffer[(i4+1)%3], vertex_coords_framebuffer[(i4+2)%3], pt);
						i4++;
					}
					// Compute the barycentric coordinates
					Mvec<float> phi(3);
					wh(i5,3){
						phi[i5] = areas[i5] / area_total;
						i5++;
					}
					// Store the w coords from the vertices
					Mvec<float> w(3);
					wh(i6,3){
						w[i6] = vertex_coords[i6].w;
						i6++;
					}
					// Compute the w_inv
					float weighted_w = 0.0f;
					wh(i7,3){
						weighted_w += phi[i7] / w[i7];
						i7++;
					}
					// Compute the z coordinate of the point
					float z_pt = 0.0f;
					wh(i8,3){
						z_pt += phi[i8] * vertex_coords_screen[i8].z;
						i8++;
					}
					// Check depth
					if(depth_flag){
						int pt_x_y_map = pt_y * v1.y + pt_x;
						auto it = z_buffer.find(pt_x_y_map);
						if(it != z_buffer.end()){
							if(it->second < z_pt){
								continue;
							}
						}
						z_buffer[pt_x_y_map] = z_pt;
					}
					// Compute the interpolated attributes
					Attribs interpolated;
					int nDim = vertex_attributes[0].get_num_dims();
					// Sanity check : check if dim for all 3 vertices is same using assert
					assert(nDim == vertex_attributes[1].get_num_dims() && nDim == vertex_attributes[2].get_num_dims() && "Error: Vertex dimensions do not match!");
					wh(i,nDim){
						int dim = vertex_attributes[0].get_dimension_i(i);
						switch(dim){
							case 1:{
								Mvec<float> vals(3);
								vals[0] = vertex_attributes[0].get<float>(i);
								vals[1] = vertex_attributes[1].get<float>(i);
								vals[2] = vertex_attributes[2].get<float>(i);
								if(w[0] == 0.0f || w[1] == 0.0f || w[2] == 0.0f){
									interpolated.set(i, phi[0]*vals[0] + phi[1]*vals[1] + phi[2]*vals[2]);
								}
								else{
									float q_by_w = phi[0] * vals[0] / w[0] + phi[1] * vals[1] / w[1] + phi[2] * vals[2] / w[2];
									interpolated.set(i, q_by_w / weighted_w );
								}
								break;
							}
							case 4:{
								Mvec<glm::vec4> vals(3);
								vals[0] = vertex_attributes[0].get<glm::vec4>(i);
								vals[1] = vertex_attributes[1].get<glm::vec4>(i);
								vals[2] = vertex_attributes[2].get<glm::vec4>(i);
								if(w[0] == 0.0f || w[1] == 0.0f || w[2] == 0.0f){
									interpolated.set(i, phi[0]*vals[0] + phi[1]*vals[1] + phi[2]*vals[2]);
								}
								else{
									glm::vec4 q_by_w = phi[0] * vals[0] / w[0] + phi[1] * vals[1] / w[1] + phi[2] * vals[2] / w[2];
									interpolated.set(i, q_by_w / weighted_w );
								}
								break;
							}
							case 2:{
								Mvec<glm::vec2> vals(3);
								vals[0] = vertex_attributes[0].get<glm::vec2>(i);
								vals[1] = vertex_attributes[1].get<glm::vec2>(i);
								vals[2] = vertex_attributes[2].get<glm::vec2>(i);
								if(w[0] == 0.0f || w[1] == 0.0f || w[2] == 0.0f){
									interpolated.set(i, phi[0]*vals[0] + phi[1]*vals[1] + phi[2]*vals[2]);
								}
								else{
									glm::vec2 q_by_w = phi[0] * vals[0] / w[0] + phi[1] * vals[1] / w[1] + phi[2] * vals[2] / w[2];
									interpolated.set(i, q_by_w / weighted_w );
								}
								break;
							}
							case 3:{
								Mvec<glm::vec3> vals(3);
								vals[0] = vertex_attributes[0].get<glm::vec3>(i);
								vals[1] = vertex_attributes[1].get<glm::vec3>(i);
								vals[2] = vertex_attributes[2].get<glm::vec3>(i);
								if(w[0] == 0.0f || w[1] == 0.0f || w[2] == 0.0f){
									interpolated.set(i, phi[0]*vals[0] + phi[1]*vals[1] + phi[2]*vals[2]);
								}
								else{
									glm::vec3 q_by_w = phi[0] * vals[0] / w[0] + phi[1] * vals[1] / w[1] + phi[2] * vals[2] / w[2];
									interpolated.set(i, q_by_w / weighted_w );
								}
								break;
							}
						}
						i++;
					}
					
					glm::vec4 color = shader_pg.fs(shader_pg.uniforms, interpolated);
					Mvec<uint8_t> color_vec = color_from_glm(color);
					if (window_spp == 1){
						pixels[pt_y * v1.y + pt_x] = SDL_MapRGBA(framebuffer->format, color_vec[0], color_vec[1], color_vec[2], color_vec[3]);
					}
					else{
						pixels[pt_y * v1.y + pt_x] = SDL_MapRGBA(framebuffer_2->format, color_vec[0], color_vec[1], color_vec[2], color_vec[3]);
					}
				}
			} 
			return;
		}

		void Rasterizer::drawObject(const Object &object) {
			// Draw the object
			// TODO
			// Lock the surface
			// std::cout << "Drawing Object" << std::endl;
			// Lock the surface
			lock_frame_buffer(1);
			// Draw the object
			// if (window_spp == 1){
			Uint32 *pixels = (Uint32*)framebuffer->pixels;
			int pitch = framebuffer->pitch / sizeof(Uint32);
			ShaderProgram &shader_pg = shader_for_rasterizer;
			// }
			if(window_spp != 1){
				lock_frame_buffer(2);
				pixels = (Uint32*)framebuffer_2->pixels;
				pitch = framebuffer_2->pitch / sizeof(Uint32);
			}
			// Now render each triangle
			int nTris = object.indices.size();
			wh(i,nTris){
				draw_helper(glm::ivec2(i, pitch), object, shader_pg, pixels);
				i++;
			}
			
			// Draw from framebuffer_2 to framebuffer_1 using supersampling if window_spp > 1
			if(window_spp != 1){
				// Every window_spp * window_spp cells in framebuffer_2 correspond to 1 cell in framebuffer_1
				Uint32 *pixels_1 = (Uint32*)framebuffer->pixels;
				Uint32 *pixels_2 = (Uint32*)framebuffer_2->pixels;
				for(int i_x = 0; i_x < window_wd; i_x += window_spp){
					for(int j_y = 0; j_y < window_ht; j_y += window_spp){
						int f1_x = i_x / window_spp;
						int f1_y = j_y / window_spp;
						// Average the pixels here
						// Framebuffers are already locked
						glm::vec4 color = glm::vec4(0.0f);

						for(int k=0; k<window_spp; k++){
							for(int l=0; l<window_spp; l++){
								int f2_x = i_x + k;
								int f2_y = j_y + l;
								// Take the average of the colors
								Uint32 pixel = pixels_2[f2_y * window_wd + f2_x];
								Mvec<uint8_t> color_vec = {0, 0, 0, 0};
								SDL_GetRGBA(pixel, framebuffer_2->format, &color_vec[0], &color_vec[1], &color_vec[2], &color_vec[3]);
								color += glm::vec4(color_vec[0], color_vec[1], color_vec[2], color_vec[3]);
							}
						}
						color /= (window_spp * window_spp);
						pixels_1[f1_y * (pitch / window_spp) + f1_x] = SDL_MapRGBA(framebuffer->format, color[0], color[1], color[2], color[3]);
					}
				}


				unlock_frame_buffer(2);
			}
			unlock_frame_buffer(1);

		}

		void Rasterizer::show() {
			// std::cout << " In function show " << std::endl;
			SDL_Surface* windowSurface = SDL_GetWindowSurface(window);
			SDL_BlitScaled(framebuffer, NULL, windowSurface, NULL);
			SDL_UpdateWindowSurface(window);

			wait_or_quit();

		}



	}
}
