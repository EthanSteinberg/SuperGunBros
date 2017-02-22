#include "render.h"

#include <iostream>
#include <vector>

#include <lodepng.h>

#include <glad/glad.h>

const char* vertex_shader1_source = R"(
    #version 100
    attribute vec3 position;
    attribute vec2 a_texcoord;
    attribute vec2 a_texpos;
    attribute vec2 a_texsize;
    varying vec2 v_texcoord;
    varying vec2 v_texpos;
    varying vec2 v_texsize;
    uniform vec2 screen_size;
    void main()
    {
      v_texcoord = a_texcoord;
      v_texpos = a_texpos;
      v_texsize = a_texsize;
      vec2 temp = vec2(2, -2) * position.xy / screen_size + vec2(-1, 1);
      gl_Position = vec4(temp, -position.z/100.0, 1.0);
    }
  )";

const char* fragment_shader1_source = R"(
    #version 100
    precision mediump float;
    uniform sampler2D atlas;
    uniform vec2 atlas_size;
    varying vec2 v_texcoord;
    varying vec2 v_texpos;
    varying vec2 v_texsize;
    void main()
    {
      gl_FragColor = texture2D(atlas, clamp(v_texcoord, v_texpos + vec2(0.5, 0.5), v_texpos + v_texsize - vec2(0.5, 0.5)) / atlas_size);
    }
   )";


const char* vertex_shader2_source = R"(
    #version 100
    attribute vec3 position;
    attribute vec3 a_color;
    attribute vec2 a_center;
    attribute float a_radius;
    varying vec3 v_color;
    varying vec2 v_center;
    varying vec3 v_position;
    varying float v_radius;
    uniform vec2 screen_size;
    void main()
    {
      v_position = position;
      v_color = a_color;
      vec2 temp = vec2(2, -2) * position.xy / screen_size + vec2(-1, 1);
      gl_Position = vec4(temp, -position.z/100.0, 1.0);

      v_center = a_center;
      v_radius = a_radius;
    }
  )";

const char* fragment_shader2_source = R"(
    #version 100
    precision mediump float;
    varying vec3 v_color;
    varying vec2 v_center;
    varying vec3 v_position;
    varying float v_radius;
    void main()
    {
      vec2 pos = v_position.xy - v_center;

      float dist =  dot(pos, pos);

      float alpha = 1.0 - smoothstep(v_radius * v_radius, (v_radius + 1.0) * (v_radius + 1.0), dist);

      gl_FragColor = vec4(v_color * alpha, alpha);
    }
   )";


// Create and compile an OpenGL shader of the given type.
// Also checks for errors and prints as necessary.
GLint create_and_compile_shader(const char* source, GLenum type) {
    int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        std::cerr << "Could not compile the required shader " << source << std::endl;

        GLint logSize = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);

        std::string errorLog(logSize, 0);
        glGetShaderInfoLog(shader, logSize, &logSize, &errorLog[0]);
        std::cout<<errorLog<<std::endl;
        exit(-1);
    }

    return shader;
}

// Load the texture atlas for the program.
void load_texture_atlas(int program) {
    std::vector<unsigned char> image;

    unsigned int atlas_width;
    unsigned int atlas_height;

    unsigned decode_error = lodepng::decode(image, atlas_width, atlas_height, "../assets/img/pixelPacker.png");

    if (decode_error != 0) {
        std::cerr << "error while decoding the texture atlas: " << lodepng_error_text(decode_error) << std::endl;
    }

    unsigned int texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlas_width, atlas_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    int atlas_size_location = glGetUniformLocation(program, "atlas_size");
    glUniform2f(atlas_size_location, atlas_width, atlas_height);

    int sampler_location = glGetUniformLocation(program, "atlas");
    glUniform1i(sampler_location, 0);
}

// Update the size. This requires changing a uniform variable and the viewport.
void update_size(int program, int pixel_width, int pixel_height,
                 int screen_width, int screen_height) {
    glViewport(0, 0, pixel_width, pixel_height);

    int screen_size_location = glGetUniformLocation(program, "screen_size");
    glUniform2f(screen_size_location, screen_width, screen_height);
}

// Create and initialize the OpenGL program.
std::array<std::function<void(void)>, 2>  create_and_use_program(int pixel_width, int pixel_height, int screen_width, int screen_height) {
    int main_program = glCreateProgram();
    {
        int vertex_shader = create_and_compile_shader(vertex_shader1_source, GL_VERTEX_SHADER);
        int fragment_shader = create_and_compile_shader(fragment_shader1_source, GL_FRAGMENT_SHADER);

        glAttachShader(main_program, vertex_shader);
        glAttachShader(main_program, fragment_shader);
        glLinkProgram(main_program);
    }

    int flame_program = glCreateProgram();
    {
        int vertex_shader = create_and_compile_shader(vertex_shader2_source, GL_VERTEX_SHADER);
       int fragment_shader = create_and_compile_shader(fragment_shader2_source, GL_FRAGMENT_SHADER);

       glAttachShader(flame_program, vertex_shader);
       glAttachShader(flame_program, fragment_shader);
       glLinkProgram(flame_program);
    }

    std::array<unsigned int, 2> buffers;
    glGenBuffers(2, buffers.data());

    std::array<unsigned int, 2> vao;
    glGenVertexArrays(2, vao.data());

    {
        glUseProgram(main_program);
        glBindVertexArray(vao[0]);
        glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);

        int position_location = glGetAttribLocation(main_program, "position");
        int tex_location = glGetAttribLocation(main_program, "a_texcoord");
        int tex_pos = glGetAttribLocation(main_program, "a_texpos");
        int tex_size = glGetAttribLocation(main_program, "a_texsize");

        glEnableVertexAttribArray(position_location);
        glVertexAttribPointer(position_location, 3, GL_FLOAT, false, 4 * 9, 0);

        glEnableVertexAttribArray(tex_location);
        glVertexAttribPointer(tex_location, 2, GL_FLOAT, false, 4 * 9, (void *)(4 * 3));

        glEnableVertexAttribArray(tex_pos);
        glVertexAttribPointer(tex_pos, 2, GL_FLOAT, false, 4 * 9, (void *)(4 * 5));

        glEnableVertexAttribArray(tex_size);
        glVertexAttribPointer(tex_size, 2, GL_FLOAT, false, 4 * 9, (void *)(4 * 7));

        load_texture_atlas(main_program);

        update_size(main_program, pixel_width, pixel_height, screen_width, screen_height);
    }

    {
        glUseProgram(flame_program);
        glBindVertexArray(vao[1]);
        glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);

        int position_location = glGetAttribLocation(flame_program, "position");
        int color_location = glGetAttribLocation(flame_program, "a_color");
        int center_location = glGetAttribLocation(flame_program, "a_center");
        int radius_location = glGetAttribLocation(flame_program, "a_radius");

        glEnableVertexAttribArray(position_location);
        glVertexAttribPointer(position_location, 3, GL_FLOAT, false, 4 * 9, 0);

        glEnableVertexAttribArray(color_location);
        glVertexAttribPointer(color_location, 3, GL_FLOAT, false, 4 * 9, (void *)(4 * 3));

        glEnableVertexAttribArray(center_location);
        glVertexAttribPointer(center_location, 2, GL_FLOAT, false, 4 * 9, (void *)(4 * 6));

        glEnableVertexAttribArray(radius_location);
        glVertexAttribPointer(radius_location, 1, GL_FLOAT, false, 4 * 9, (void *)(4 * 8));

        update_size(flame_program, pixel_width, pixel_height, screen_width, screen_height);
    }


    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(1.0, 1, 1, 1.0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    auto main_setup = [=]() {
        glUseProgram(main_program);
        glBindVertexArray(vao[0]);
        glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    };

    auto flame_setup = [=]() {
        glUseProgram(flame_program);
        glBindVertexArray(vao[1]);
        glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    };

    return {main_setup, flame_setup};
}
