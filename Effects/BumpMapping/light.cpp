#include "light.h"


void light_source::setLightParams(void){

    // Load the Defaults 
    // Light Position in Model space
    lPosition = glm::vec3(1.0, 1.0, 0.0);
    glUniform3fv (light_Loc , 1, ( GLfloat * ) &lPosition);
    
    // light ambient properties
    lAmbient = glm::vec3(1.0, 1.0, 1.0);
    glUniform3fv (l_ambientLoc , 1, ( GLfloat * ) &lAmbient);

    // light diffuse properties
    lDiffuse = glm::vec3(1.0, 1.0, 1.0);
    glUniform3fv (l_diffuseLoc , 1, ( GLfloat * ) &lDiffuse);
    
    // light specular properties
    lSpecular = glm::vec3(1.0, 1.0, 1.0);
    glUniform3fv (l_specularLoc , 1, ( GLfloat * ) &lSpecular);
}

void light_source::getUniformLocations(GLuint shader_id){
    light_Loc = glGetUniformLocation (shader_id, "lightPosition" );
    std::cout << "light_Loc " << light_Loc << std::endl;
    l_ambientLoc = glGetUniformLocation (shader_id, "lambient" );
    std::cout << "l_ambientLoc " << l_ambientLoc << std::endl;
    l_diffuseLoc = glGetUniformLocation (shader_id, "ldiffuse" );
    l_specularLoc = glGetUniformLocation (shader_id, "lspecular" );
}

void light_material::setMaterialParams(void){

    // Load the defaults 
    
    // Load the material ambient properties
    glm::vec3 mAmbient = glm::vec3(0.17, 0.01, 0.017);
    glUniform3fv (m_ambientLoc , 1, ( GLfloat * ) &mAmbient);
    
    // Load the material diffuse properties
    glm::vec3 mDiffuse = glm::vec3(0.61, 0.04, 0.046);
    glUniform3fv (m_diffuseLoc , 1, ( GLfloat * ) &mDiffuse);
    
    // Load the material specular properties
    glm::vec3 mSpecular = glm::vec3(0.72, 0.62, 0.62);
    glUniform3fv (m_specularLoc , 1, ( GLfloat * ) &mSpecular);

    // Load the material shininess properties
    Shine = 76.0;
    glUniform1f (shineLoc , Shine);

}

void light_material::getUniformLocations(GLuint shader_id){

    m_ambientLoc = glGetUniformLocation (shader_id, "mambient" );
    m_diffuseLoc = glGetUniformLocation (shader_id, "mdiffuse" );
    m_specularLoc = glGetUniformLocation (shader_id, "mspecular" );
    shineLoc = glGetUniformLocation (shader_id, "shininess" );
}
