//
//  Pyramid.hpp
//  project2B
//
//  Created by Andrew Exton on 10/28/18.
//

#ifndef Pyramid_hpp
#define Pyramid_hpp

#include <stdio.h>

#include <iostream>
#include <glad/glad.h>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

class Pyramid {
public:
    Pyramid();
    ~Pyramid();
    
    Pyramid(int BASE_POINTS);
    int BASE_POINTS;
    
    GLfloat *getVertexArray();
    GLushort *getIndexArray();
    
    int getVertexArraySize();
    int getIndexArraySize();
    int getFaceCount() {return 2 * BASE_POINTS;};
    
private:
    
    GLfloat *vertex_array;
    GLushort *index_array;
    
    int va_size;
    int ia_size;
    
    // returns buffer size in bytes
    int allocate_Pyramid_vertex_array(GLfloat **ptr, int slices);
    
    // returns buffer size in bytes
    int allocate_Pyramid_index_array(GLushort **ptr, int slices);
    
    // returns faces generated
    int generate_Pyramid(GLfloat *vertex_array, GLushort *index_array, int slices);
    
};
#endif /* Pyramid_hpp */
