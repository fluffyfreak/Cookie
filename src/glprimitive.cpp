#include "glprimitive.h"
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

GLPrimitive::GLPrimitive() : vertexId_(0), indexId_(0) {

}

GLPrimitive::~GLPrimitive() {
    glDeleteBuffers(1, &vertexId_);
    glDeleteBuffers(1, &indexId_);
}

void GLPrimitive::draw() {
     glBindBuffer(GL_ARRAY_BUFFER, vertexId_);
     glEnableClientState(GL_VERTEX_ARRAY);
     glVertexPointer(3, GL_FLOAT, sizeof(GLVertex), BUFFER_OFFSET(vOffset_));   //The starting point of the VBO, for the vertices
     glEnableClientState(GL_NORMAL_ARRAY);
     glNormalPointer(GL_FLOAT, sizeof(GLVertex), BUFFER_OFFSET(nOffset_));   //The starting point of normals, 12 bytes away
     glClientActiveTexture(GL_TEXTURE0);
     glEnableClientState(GL_TEXTURE_COORD_ARRAY);
     glTexCoordPointer(3, GL_FLOAT, sizeof(GLVertex), BUFFER_OFFSET(tOffset_));   //The starting point of texcoords, 24 bytes away
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexId_);
     //To render, we can either use glDrawElements or glDrawRangeElements
     //The is the number of indices. 3 indices needed to make a single triangle
     glDrawElements(type_, idxCount_, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));   //The starting point of the IBO
     glDisableClientState(GL_VERTEX_ARRAY);
     glBindBuffer(GL_ARRAY_BUFFER, 0);
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

GLQuad::GLQuad(float3 tess, float3 translate, float3 scale) {
     this->tesselate(tess, translate, scale);
}


void GLQuad::tesselate(float3 tess, float3 translate, float3 scale) {

    if(vertexId_) glDeleteBuffers(1, &vertexId_);
    if(indexId_) glDeleteBuffers(1, &indexId_);

    type_ = GL_QUADS;
    idxCount_ = 4 * tess.x * tess.y;
    float3 delta = scale / tess;
    float3 tdelta = 1.0 / tess;
    delta.z = 0;
    GLVertex *pVertex = new GLVertex[(int)((tess.x + 1) * (tess.y + 1))];
    for(int y=0, i=0; y<=tess.y; y++) {
	for(int x=0; x<=tess.x; x++, i++) {
	    pVertex[i].p = (float3(-0.5, -0.5, 0.0) + translate + delta * float3(x, y, 0)) * scale;
	    pVertex[i].n = float3(0.0, 0.0, 1.0);
	    pVertex[i].t = float3(x, y, 0) * tdelta;
	}
    }
    glGenBuffers(1, &vertexId_);
    glBindBuffer(GL_ARRAY_BUFFER, vertexId_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLVertex)*((tess.x + 1) * (tess.y + 1)), &pVertex[0].p.x, GL_STATIC_DRAW);

    unsigned short *pIndices = new unsigned short[idxCount_];
    for(int y=0, i=0; y<tess.y; y++) {
	for(int x=0; x<tess.x; x++, i+=4) {
	   pIndices[i] = y*(tess.x+1)+x;
	   pIndices[i+1] = y*(tess.x+1)+x+1;
	   pIndices[i+2] = (y+1)*(tess.x+1)+x+1;
	   pIndices[i+3] = (y+1)*(tess.x+1)+x;

	}
    }

    glGenBuffers(1, &indexId_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexId_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)*idxCount_, &pIndices[0], GL_STATIC_DRAW);

    delete[] pVertex, delete[] pIndices;

    //byte offsets in the vertex struct...this may as well be hard coded for now.
    vOffset_ = 0;
    nOffset_ = 12;
    tOffset_ = 24;
}