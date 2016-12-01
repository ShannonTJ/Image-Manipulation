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
    int res = 1;
    vec2 newCoords;
    newCoords.x = res * (int(textureCoords.x)/res);
    newCoords.y = res * (int(textureCoords.y)/res);

    // write colour output without modification
    vec4 colour = texture(tex, newCoords);

    float saveR = colour.r;
    float saveG = colour.g;

//GRAYSCALE 3
    colour.r = (colour.r*0.213f) + (colour.g*0.715f) + (colour.b*0.072f);
    colour.g = (saveR*0.213f) + (colour.g*0.715f) + (colour.b*0.072f);
    colour.b = (saveR*0.213f) + (saveG*0.715f) + (colour.b*0.072f);

    FragmentColour = colour;
}
