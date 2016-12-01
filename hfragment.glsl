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
    
    //Initialize colour vector, intermediate vector
    vec4 colour;
    vec4 med;

    //med = nonzero values in the horizontal Sobel operator
    //colour = sum of the nonzero Sobel values
    
	   med = texture(tex, newCoords + vec2(-1,-1));
	   colour = colour + med;

   	   med = texture(tex, newCoords + vec2(1,1));
	   colour = colour - med;

   	   med = texture(tex, newCoords + vec2(0,1));
	   colour = colour - 2*med;

   	   med = texture(tex, newCoords + vec2(0,-1));
	   colour = colour + 2*med;

  	   med = texture(tex, newCoords + vec2(-1,1));
	   colour = colour - med;

  	   med = texture(tex, newCoords + vec2(1,-1));
	   colour = colour + med;

    //Take absolute value of the final output
    
	   colour = abs(colour);

    FragmentColour = colour;
}
