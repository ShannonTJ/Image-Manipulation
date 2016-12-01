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

//SEPIA
//Sepia formula referenced from: http://stackoverflow.com/questions/1061093/how-is-a-sepia-tone-created
    colour.r = colour.r*0.393f + colour.g*0.769f + colour.b*0.189f;
    colour.g = saveR*0.349f + colour.g*0.686f + colour.b*0.168f;
    colour.b = saveR*0.272f + saveG*0.534f + colour.b*0.131f;

    FragmentColour = colour;
}
