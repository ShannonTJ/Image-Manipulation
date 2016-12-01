// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Modifications by: Shannon TJ
// Date:    Fall 2016
// ==========================================================================
#version 410

// location indices for these attributes correspond to those specified in the
// InitializeGeometry() function of the main program
layout(location = 0) in vec2 VertexPosition;
layout(location = 1) in vec3 VertexColour;
layout(location = 2) in vec2 VertexTexture;

// output to be interpolated between vertices and passed to the fragment stage
out vec3 Colour;
out vec2 textureCoords;

//rotation matrix (in radians)
//mat2 rotate = mat2(
//     cos(3.14), -sin(3.14),
//     sin(3.14), cos(3.14)
//     );

//mouse position
vec2 mouse = vec2(0.0, 0.0);

void main()
{
    // assign vertex position without modification
    //gl_Position = vec4((rotate*VertexPosition)-mouse, 0.0, 1.0);

    gl_Position = vec4(VertexPosition, 0.0,1.0);

    // assign output colour to be interpolated
    Colour = VertexColour;
    textureCoords = VertexTexture;
}
