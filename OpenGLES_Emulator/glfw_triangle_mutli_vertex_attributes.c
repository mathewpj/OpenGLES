/************************************************************** 
 *
 *  Running OpenglEs on desktop requires an emulator.
 *  You could use an emulator like Mali OpenGL ES 
 *  emulator however the minimum requirements are 
 *  - OpenGL 3.2 – when OpenGL ES 2.0 contexts are used
 *  - OpenGL 3.3 – when OpenGL ES 3.0 contexts are used
 *  - OpenGL 4.3 – when OpenGL ES 3.1 or 3.2 contexts are used
 *  and my system only supports OpenGL 3.0. 
 *  Hence I've had to use glfw and modify the shaders below 
 *  to get a OpenGL ES 3.0 equivalent "Hello World" example 
 *  working.
 *
 *  sudo apt-get install libglfw3-dev libgles2-mesa-dev
 *  gcc glfw_triangle_mutli_vertex_attributes.c -lGLESv2 -lglfw
 *
 *  Appropriator : mathew.p.joseph@gmail.com
 *
 *************************************************************/

#include <stdio.h>
#include <stdlib.h>

#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

static const GLuint WIDTH = 800;
static const GLuint HEIGHT = 600;
static const GLchar* vertex_shader_source =
    "#version 300 es\n"
    "layout(location = 0) in vec3 position;\n"
    "layout(location = 1) in vec3 color;\n"
    "out vec3 vertex_color;\n"	
    "void main() {\n"
    "   gl_Position = vec4(position, 1.0);\n"
    "	vertex_color = color;\n"
    "}\n";
    /* 	The Vertex shader gets called three times since
	only single triangle is being drawn. however the 
	fragment shader will be call more than 3 times since
	multiple fragments will be generated. The color 
	for each pixel alone a line gets interpolated 
	i.e., if A = [0.0f,  0.5f, 0.0f] & its color is Red
	and B = [0.5f, -0.5f, 0.0f] and its color is Green
	then a point between A and B will be a color thats 
	interpolated between Red and Green */
static const GLchar* fragment_shader_source =
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec3 vertex_color;\n"	
    "out vec4 fragColor;\n"	
    "void main() {\n"
    "   fragColor = vec4(vertex_color, 1.0);\n"
    "}\n";

	/* Interleaving vertices and color */
static const GLfloat vertices_and_color[] = {
        0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
       -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
};

GLint common_get_shader_program(const char *vertex_shader_source, const char *fragment_shader_source) {
    enum Consts {INFOLOG_LEN = 512};
    GLchar infoLog[INFOLOG_LEN];
    GLint fragment_shader;
    GLint shader_program;
    GLint success;
    GLint vertex_shader;

    /* Vertex shader */
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, INFOLOG_LEN, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    }

    /* Fragment shader */
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, INFOLOG_LEN, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
    }

    /* Link shaders */
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, INFOLOG_LEN, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return shader_program;
}

int main(void) {
    GLuint shader_program, vbo;
    GLint pos, col;
    GLFWwindow* window;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    window = glfwCreateWindow(WIDTH, HEIGHT, __FILE__, NULL, NULL);
    glfwMakeContextCurrent(window);

    printf("GL_VERSION  : %s\n", glGetString(GL_VERSION) );
    printf("GL_RENDERER : %s\n", glGetString(GL_RENDERER) );

    shader_program = common_get_shader_program(vertex_shader_source, fragment_shader_source);
    pos = glGetAttribLocation(shader_program, "position");
    col = glGetAttribLocation(shader_program, "color");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, WIDTH, HEIGHT);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_and_color), vertices_and_color, GL_STATIC_DRAW);
    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (GLvoid*)0);
    glVertexAttribPointer(col, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(pos);
    glEnableVertexAttribArray(col);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader_program);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
    }
    glDeleteBuffers(1, &vbo);
    glfwTerminate();
    return EXIT_SUCCESS;
}
