/************************************************************** 
 *
 *  Function to load shaders.
 *
 *  Author : Mathew P Joseph
 *  Email  : mathew.p.joseph@gmail.com
 *
 *************************************************************/
#include <iostream>
#include <string.h>
#include <math.h>

#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

using namespace std;

GLint common_get_shader_program(const char *vertex_shader_source, const char *fragment_shader_source) 
{

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
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED " << infoLog << endl;
    }

    /* Fragment shader */
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, INFOLOG_LEN, NULL, infoLog);
        cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED " << infoLog <<endl;
    }

    /* Link shaders */
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, INFOLOG_LEN, NULL, infoLog);
        cout <<  "ERROR::SHADER::PROGRAM::LINKING_FAILED" << infoLog << endl;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return shader_program;
}

