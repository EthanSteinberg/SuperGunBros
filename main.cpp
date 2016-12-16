#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <vector>

// const char* vertex_shader_source = R"(
//     attribute vec3 position;
//     attribute vec2 a_texcoord;
//     attribute vec2 a_texpos;
//     attribute vec2 a_texsize;
//     varying vec2 v_texcoord;
//     varying vec2 v_texpos;
//     varying vec2 v_texsize;
//     uniform vec2 screen_size;
//     void main()
//     {
//       v_texcoord = a_texcoord;
//       v_texpos = a_texpos;
//       v_texsize = a_texsize;
//       vec2 temp = vec2(2, -2) * position.xy / screen_size + vec2(-1, 1);
//       gl_Position = vec4(temp, position.z, 1.0);
//     }
//   )";

// const char* fragment_shader_source = R"(
//     precision mediump float;
//     uniform sampler2D testImage;
//     uniform vec2 atlas_size;
//     varying vec2 v_texcoord;
//     varying vec2 v_texpos;
//     varying vec2 v_texsize;
//     void main()
//     {
//       gl_FragColor = texture2D(testImage, clamp(v_texcoord, v_texpos + vec2(0.5, 0.5), v_texpos + v_texsize - vec2(0.5, 0.5)) / atlas_size);
//     }
//    )";

const char* vertex_shader_source = R"(
    attribute vec2 position;
    void main()
    {
      gl_Position = vec4(position, 0.0, 1.0);
    }
  )";

const char* fragment_shader_source = R"(

    void main()
    {
      gl_FragColor = vec4(0.5, 0.5, 0, 1.0);
    }
   )";

static void error_callback(int /*error*/, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
static void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}


static void opengl_error(GLenum /*source*/,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei /*length*/,
    const GLchar* message,
    const void* /*userParam*/) {
    fprintf(stderr, "Opengl Error: %s, %d %d %d\n", message, type, id, severity);
}

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

int main(void)
{
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    window = glfwCreateWindow(640, 480, "Simple example", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);

    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    glDebugMessageCallback(opengl_error, nullptr);

    glfwSwapInterval(1);
    // NOTE: OpenGL error checks have been omitted for brevity

    int program = glCreateProgram();

    int vertex_shader = create_and_compile_shader(vertex_shader_source, GL_VERTEX_SHADER);
    int fragment_shader = create_and_compile_shader(fragment_shader_source, GL_FRAGMENT_SHADER);

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    glUseProgram(program);

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    int positionLocation = glGetAttribLocation(program, "position");

    glEnableVertexAttribArray(positionLocation);

    glVertexAttribPointer(positionLocation, 2, GL_FLOAT, false, 4 * 2, 0);

    float testing[2 * 3] = {};

    testing[0] = 0.0;
    testing[1] = 0.0;

    testing[2] = 0.0;
    testing[3] = 1.0;

    testing[4] = 1.0;
    testing[5] = 1.0;

    glBufferData(GL_ARRAY_BUFFER, 2 * 4 * 3, testing, GL_STREAM_DRAW);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(1.0, 0, 0, 1.0);

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);  
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

