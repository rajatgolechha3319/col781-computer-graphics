#ifndef SW_HPP
#define SW_HPP

#include <glm/glm.hpp>
#include <map>
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#define Mvec std::vector

namespace COL781 {
	namespace Software {

		class Attribs {
			// A class to contain the attributes of ONE vertex
		public:
			// only float, glm::vec2, glm::vec3, glm::vec4 allowed
			template <typename T> T get(int attribIndex) const;
			template <typename T> void set(int attribIndex, T value);
			int get_num_dims() const; // Get the total number of dimensions
			int get_dimension_i(int index) const ; // Get the ith value of dims array

		private:
			std::vector<int> dims;
			std::vector<glm::vec4> values;
		};

		class Uniforms {
			// A class to contain all the uniform variables
		public:
			// any type allowed
			template <typename T> T get(const std::string &name) const;
			template <typename T> void set(const std::string &name, T value);
		private:
			std::map<std::string,void*> values;
		};

		/* A vertex shader is a function that:
		   reads the uniform variables and one vertex's input attributes,
		   writes the output attributes for interpolation to fragments,
		   and returns the vertex position in NDC as a homogeneous vec4. */
		using VertexShader = glm::vec4(*)(const Uniforms &uniforms, const Attribs &in, Attribs &out);

		/* A fragment shader is a function that:
		   reads the uniform variables and one fragment's interpolated attributes,
		   and returns the colour of the fragment as an RGBA value. */
		using FragmentShader = glm::vec4(*)(const Uniforms &uniforms, const Attribs &in);

		struct ShaderProgram {
			VertexShader vs;
			FragmentShader fs;
			Uniforms uniforms;
		};

		struct Object {
			std::vector<Attribs> vertexAttributes;
			std::vector<glm::ivec3> indices;
		};

		class Rasterizer {
		public:
#include "api.inc"
FragmentShader fsGradientSpecular();
		private:
			// My helpers
			void lock_frame_buffer(int bno);
			void unlock_frame_buffer(int bno);
			void wait_or_quit();
			Mvec<glm::ivec2> bounding_box(const glm::ivec2 &v1, const glm::ivec2 &v2, const glm::ivec2 &v3);
			void draw_helper(const glm::ivec2 &v1, const Object &obj, ShaderProgram &shader_pg, Uint32 *pixels);

			//
			SDL_Window *window;
			bool quit;
			// Create window and rasterizer attributes here
			SDL_Surface *framebuffer;
			SDL_Surface *framebuffer_2;
			ShaderProgram shader_for_rasterizer;
			// std::map<int, float> z_buffer; // z buffer
			std::map<int,float> z_buffer;
			bool depth_flag = false;
			int window_ht;
			int window_wd; 
			int window_spp;
			int BUFF_SZ;
		};

	}
}

#endif
