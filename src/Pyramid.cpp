//
//  Pyramid.cpp
//  project2B
//
//  Created by Andrew Exton on 10/28/18.
//

#include "Pyramid.hpp"

#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"

using namespace glm;
using namespace std;

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

Pyramid::Pyramid() {
    Pyramid(3);
}

Pyramid::~Pyramid() {
    //    free(this->vertex_array);
    //    free(this->index_array);
}

Pyramid::Pyramid(int BASE_POINTS) {
    this->BASE_POINTS = BASE_POINTS;
    
    this->va_size = allocate_Pyramid_vertex_array(&vertex_array, this->BASE_POINTS);
    this->ia_size = allocate_Pyramid_index_array(&index_array, this->BASE_POINTS);
    generate_Pyramid(vertex_array, index_array, this->BASE_POINTS);
}



// returns buffer size in bytes
int Pyramid::allocate_Pyramid_vertex_array(GLfloat **ptr, int slices) {
    int size = (slices + 1) * 3 * sizeof(GLfloat);
    //    cout << "ALLOCATING VA SIZE: " << size << endl;
    *ptr = (GLfloat *) malloc(size);
    return size;
}

// returns buffer size in bytes
int Pyramid::allocate_Pyramid_index_array(GLushort **ptr, int slices) {
    int size = 2 * slices * 3 * sizeof(GLushort);
    //    cout << "ALLOCATING IA SIZE: " << size << endl;
    *ptr =  (GLushort *) malloc(size);
    return size;
}

// returns faces generated
int Pyramid::generate_Pyramid(GLfloat *vertex_array, GLushort *index_array, int slices) {
    if (slices < 3) cout << "CANNOT GENERATE Pyramid WITH LESS THAN 3 POINTS PER LEVEL" << endl;
    
    float alpha = 2 * M_PI / slices; // angle for point in circle
    
    // Set Pyramid Top Vertex
    vertex_array[0] = 0.0f; //x
    vertex_array[1] = 1.0f; //y
    vertex_array[2] = 0.0f; //z
    
    // Build Pyramid Base Verticies
    for (int p = 1; p <= slices; p++) {
        int px = p*3;
        int py = px + 1;
        int pz = px + 2;
        vertex_array[px] = cos(alpha * p);
        vertex_array[py] = 0.0f;
        vertex_array[pz] = sin(alpha * p);
    }
    
    int idx = 0;
    
    for (int squareFace = 1; squareFace <= slices; squareFace++) {
        // c -- d
        // | \  |
        // |  \ |
        // a -- b
        // Get 4 important point's indices
        short a, b;
        a = squareFace;
        if (a >= slices) {
            b = ((a + 1) % slices);
        } else {
            b = (a + 1);
        }
        
        index_array[idx++] = 0; // Set top vertex
        index_array[idx++] = a;
        index_array[idx++] = b;
    }
    
    return slices * 2;
}

int Pyramid::getVertexArraySize() {
    return va_size;
}
int Pyramid::getIndexArraySize() {
    return ia_size;
}

GLfloat* Pyramid::getVertexArray() {
    return vertex_array;
}

GLushort* Pyramid::getIndexArray() {
    return index_array;
}
