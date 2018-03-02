#ifndef __LIGHT_H
#define __LIGHT_H

#include <iostream>
#include <glm/glm.hpp>

#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

class light_source {
    public:
    glm::vec3 lPosition;
    glm::vec3 lAmbient;
    glm::vec3 lDiffuse;
    glm::vec3 lSpecular;
    GLint  light_Loc;
    GLint  l_ambientLoc;
    GLint  l_diffuseLoc;
    GLint  l_specularLoc;
    // Methods	
    void setLightParams(void);
    void getUniformLocations(GLuint);
};

class light_material {
    public:
    glm::vec3 mAmbient;
    glm::vec3 mDiffuse;
    glm::vec3 mSpecular;
    GLfloat Shine;
    GLint  m_ambientLoc;
    GLint  m_diffuseLoc;
    GLint  m_specularLoc;
    GLint  shineLoc;
    // Methods	
    void setMaterialParams(void);
    void getUniformLocations(GLuint);
};

#endif
