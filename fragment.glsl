// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Modifications by: Shannon TJ
// Date:    Fall 2016
// ==========================================================================
#version 410


// interpolated colour received from vertex stage
in vec3 Colour;
in vec2 textureCoords;

// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;

uniform sampler2DRect tex;

void main(void)
{  
    
    vec2 newCoords;
    newCoords.x = textureCoords.x;
    newCoords.y = textureCoords.y;
    
    // write colour output without modification
    vec4 colour = texture(tex, newCoords);


    FragmentColour = colour;
}
