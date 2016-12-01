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
    vec4 colour = vec4(0,0,0,0);
    vec4 med = vec4(0,0,0,0);
    
    //Initialize 1D 3x3 Gaussian array
    float kernel[3] = float[3](0.2,0.6,0.2);

    //For every height pixel in Gaussian kernel
    for(int j=0; j<3; j++)
    {
	//For every width pixel in Gaussian kernel
	for(int i=0; i<3; i++)
    	{
	   //med = get neighbouring pixel values (x = i-2, y = j-2)
   	   med = texture(tex, newCoords + vec2(i-1,j-1));

	   //med = pixel location * 1D array values
	   med = med*kernel[i];
	   med = med*kernel[j];
	   
	   //colour = sum values of med
	   colour = colour + med;
    	}
     }

    FragmentColour = colour;
}
