#include "shader.h"
#include <string.h>
#include <iostream>
#include <fstream>

#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

using namespace std;

static char* textFileRead(const char *fileName) {
	char* text = NULL;
	FILE *file; 
    	printf ("\ntextFileRead filename = %s\n", fileName);
	if (fileName != NULL) {
        file = fopen(fileName, "r");
        
		if (file != NULL) {
            fseek(file, 0, SEEK_END);
            int count = ftell(file);
            rewind(file);
            if (count > 0) {
				text = (char*)malloc(sizeof(char) * (count + 1));
				count = fread(text, sizeof(char), count, file);
				text[count] = '\0';
			}
			fclose(file);
		}else{printf("file == NULL\n");}
	}
	return text;
}

Shader::Shader() {
    
}

Shader::Shader(const char *vsFile, const char *fsFile) {
    init(vsFile, fsFile);
}

void Shader::init(const char *vsFile, const char *fsFile) {

 GLint status = GL_FALSE;
        char compileStatus[100];
        GLint g_color = 0,compileStat = 0, ProgramObjectAttrib = 0;
        GLsizei numActiveUserUniform = 0, strLenght = 0, size = 0, length = 0;
        GLenum type;
        GLchar ActiveUserUniform[40];

        shader_vp = glCreateShader(GL_VERTEX_SHADER);
        shader_fp = glCreateShader(GL_FRAGMENT_SHADER);

        const char* vsText = textFileRead(vsFile);
        const char* fsText = textFileRead(fsFile);

    if (vsText == NULL || fsText == NULL) {
        cerr << "Either vertex shader or fragment shader file not found." << endl;
        return;
    }

        glShaderSource(shader_vp, 1, &vsText, 0);
        glShaderSource(shader_fp, 1, &fsText, 0);
#if DEBUG_ENABLE
        glGetShaderiv(shader_vp, GL_SHADER_SOURCE_LENGTH, &status);
                printf("\nVertex Shader length(GL_SHADER_SOURCE_LENGTH): %d\n", status);
        glGetShaderiv(shader_fp, GL_SHADER_SOURCE_LENGTH, &status);
                printf("\nFragment Shader length(GL_SHADER_SOURCE_LENGTH): %d\n", status);
#endif

        glCompileShader(shader_vp);
#if DEBUG_ENABLE
        // Printing some Shader info after compilation)
        glGetShaderInfoLog(shader_vp, 100, &compileStat, compileStatus);
        printf("\nVertex Shader Compile Status: %s\n",compileStatus);
#endif

        glCompileShader(shader_fp);
#if DEBUG_ENABLE
        // Printing some Shader info after compilation)
        glGetShaderInfoLog(shader_fp, 100, &compileStat, compileStatus);
                printf("\nFragment Shader Compile Status: %s\n",compileStatus);
#endif
        shader_id = glCreateProgram();

        glAttachShader(shader_id, shader_fp);
        glAttachShader(shader_id, shader_vp);

        glLinkProgram(shader_id);
        glGetProgramiv(shader_id, GL_ACTIVE_UNIFORMS, &ProgramObjectAttrib);
#if DEBUG_ENABLE
        printf("\nProgram Object Attribute(GL_ACTIVE_UNIFORMS) : %d\n", ProgramObjectAttrib);
        //Printing the Active User defined Uniform variables
        for(int j = 0; j < ProgramObjectAttrib; j++){
                glGetActiveUniform(shader_id, j, 40, &strLenght, &size, &type, ActiveUserUniform);
                ActiveUserUniform[strLenght] = '\0';
                printf("\n ActiveUserUniform %d : %s\n",j, ActiveUserUniform);
                printf("\n glGetUniformLocation(shader_id, %s) : %d\n",
		ActiveUserUniform, glGetUniformLocation(shader_id, ActiveUserUniform));
        }
#endif

}

Shader::~Shader() {
	glDetachShader(shader_id, shader_fp);
	glDetachShader(shader_id, shader_vp);
    
	glDeleteShader(shader_fp);
	glDeleteShader(shader_vp);
	glDeleteProgram(shader_id);
}

unsigned int Shader::id() {
	return shader_id;
}

void Shader::bind() {
	glUseProgram(shader_id);
}

void Shader::unbind() {
	glUseProgram(0);
}

