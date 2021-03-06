// ==========================================================================
// Barebones OpenGL Core Profile Boilerplate
//    using the GLFW windowing system (http://www.glfw.org)
//
// Loosely based on
//  - Chris Wellons' example (https://github.com/skeeto/opengl-demo) and
//  - Camilla Berglund's example (http://www.glfw.org/docs/latest/quick.html)
//
// Author:  Sonny Chan, University of Calgary
// Modifications by: Shannon TJ 10101385

// Date:   Fall 2016
// ==========================================================================

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <iterator>
#include <math.h>

// specify that we want the OpenGL core profile before including GLFW headers
#define GLFW_INCLUDE_GLCOREARB
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

using namespace std;
// --------------------------------------------------------------------------
// OpenGL utility and support function prototypes

float angle = 0.0f;
float magnification = 1.0f;

float mouseX = 0.0f;
float mouseY = 0.0f;

float Vx1 = 0.0f;
float Vx2 = 0.0f;
float Vy1 = 0.0f;
float Vy2 = 0.0f;

float Tx1 = 0.0f;
float Tx2 = 0.0f;
float Ty1 = 0.0f;
float Ty2 = 0.0f;

GLfloat offsetX = 0.0f;
GLfloat offsetY = 0.0f;

GLfloat dragMouseX = 0.0f;
GLfloat dragMouseY = 0.0f;

int counter = 0;

bool drag = false;

bool orgColor = true;
bool g1 = false;
bool g2 = false;
bool g3 = false;
bool sepia = false;
bool gaus1 = false;
bool gaus2 = false;
bool gaus3 = false;
bool sobel1 = false;
bool sobel2 = false;
bool unsharp = false;

const char *file1 = "mandrill.png";
const char *file2 = "uclogo.png";
const char *file3 = "thirsk.jpg";
const char *file4 = "aerial.jpg";
const char *file5 = "pattern.png";
const char *file6 = "test.png";

void QueryGLVersion();
bool CheckGLErrors();

string LoadSource(const string &filename);
GLuint CompileShader(GLenum shaderType, const string &source);
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);

// --------------------------------------------------------------------------
// Functions to set up OpenGL shader programs for rendering

struct MyShader
{
	// OpenGL names for vertex and fragment shaders, shader program
	GLuint  vertex;
	GLuint  fragment;
	GLuint  program;

	// initialize shader and program names to zero (OpenGL reserved value)
	MyShader() : vertex(0), fragment(0), program(0)
	{}
};

// load, compile, and link shaders, returning true if successful
bool InitializeShaders(MyShader *shader)
{
	// load shader source from files
	string vertexSource = LoadSource("vertex.glsl");
	string fragmentSource = LoadSource("fragment.glsl");
	
	if(g1)
	  fragmentSource = LoadSource("g1fragment.glsl");
	else if(g2)
	  fragmentSource = LoadSource("g2fragment.glsl");
	else if(g3)
	  fragmentSource = LoadSource("g3fragment.glsl");
	else if(sepia)
	  fragmentSource = LoadSource("sepfragment.glsl");
	else if(gaus1)
	  fragmentSource = LoadSource("gaus1fragment.glsl");
	else if(gaus2)
	  fragmentSource = LoadSource("gaus2fragment.glsl");
	else if(gaus3)
	  fragmentSource = LoadSource("gaus3fragment.glsl");
	else if(sobel1)
	  fragmentSource = LoadSource("vfragment.glsl");
	else if(sobel2)
	  fragmentSource = LoadSource("hfragment.glsl");
	else if(unsharp)
	  fragmentSource = LoadSource("sfragment.glsl");
	
	if (vertexSource.empty() || fragmentSource.empty()) return false;

	// compile shader source into shader objects
	shader->vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
	shader->fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

	// link shader program
	shader->program = LinkProgram(shader->vertex, shader->fragment);

	// check for OpenGL errors and return false if error occurred
	return !CheckGLErrors();
}

// deallocate shader-related objects
void DestroyShaders(MyShader *shader)
{
	// unbind any shader programs and destroy shader objects
	glUseProgram(0);
	glDeleteProgram(shader->program);
	glDeleteShader(shader->vertex);
	glDeleteShader(shader->fragment);
}

// --------------------------------------------------------------------------
// Functions to set up OpenGL buffers for storing textures

struct MyTexture
{
	GLuint textureID;
	GLuint target;
	int width;
	int height;

	// initialize object names to zero (OpenGL reserved value)
	MyTexture() : textureID(0), target(0), width(0), height(0)
	{}
};

bool InitializeTexture(MyTexture* texture, const char* filename, GLuint target = GL_TEXTURE_2D)
{
	int numComponents;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(filename, &texture->width, &texture->height, &numComponents, 0);
	if (data != nullptr)
	{
		texture->target = target;
		glGenTextures(1, &texture->textureID);
		glBindTexture(texture->target, texture->textureID);
		GLuint format = numComponents == 3 ? GL_RGB : GL_RGBA;
		//cout << numComponents << endl;
		glTexImage2D(texture->target, 0, format, texture->width, texture->height, 0, format, GL_UNSIGNED_BYTE, data);

		// Note: Only wrapping modes supported for GL_TEXTURE_RECTANGLE when defining
		// GL_TEXTURE_WRAP are GL_CLAMP_TO_EDGE or GL_CLAMP_TO_BORDER
		glTexParameteri(texture->target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(texture->target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(texture->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Clean up
		glBindTexture(texture->target, 0);
		stbi_image_free(data);
		return !CheckGLErrors();
	}
	return true; //error
}

// deallocate texture-related objects
void DestroyTexture(MyTexture *texture)
{
	glBindTexture(texture->target, 0);
	glDeleteTextures(1, &texture->textureID);
}

void SaveImage(const char* filename, int width, int height, unsigned char *data, int numComponents = 3, int stride = 0)
{
	if (!stbi_write_png(filename, width, height, numComponents, data, stride))
		cout << "Unable to save image: " << filename << endl;
}

// --------------------------------------------------------------------------
// Functions to set up OpenGL buffers for storing geometry data

struct MyGeometry
{
	// OpenGL names for array buffer objects, vertex array object
	GLuint  vertexBuffer;
	GLuint  textureBuffer;
	GLuint  colourBuffer;
	GLuint  vertexArray;
	GLsizei elementCount;

	// initialize object names to zero (OpenGL reserved value)
	MyGeometry() : vertexBuffer(0), colourBuffer(0), vertexArray(0), elementCount(0)
	{}
};

// create buffers and fill with geometry data, returning true if successful
bool InitializeGeometry(MyGeometry *geometry, float height, float width)
{

  float heightUnit = 0.f;
  float heightPadding = 0.f;
  float widthUnit = 0.f;
  float widthPadding = 0.f;

  if(height < width)
    {
      heightUnit = height/(width/2.0f);
      heightPadding = (2.0f - heightUnit)/2.0f;
    }

  else if(width < height)
    {
      widthUnit = width/(height/2.0f);
      widthPadding = (2.0f - widthUnit)/2.0f;
    }

  //Vertice coordinates, with padding, offset, and drag
  Vx1 = -1.0f+widthPadding+offsetX+drag*(mouseX-dragMouseX);
  Vx2 = 1.0f-widthPadding+offsetX+drag*(mouseX-dragMouseX);
  Vy1 = 1.0f-heightPadding-offsetY-drag*(mouseY-dragMouseY);
  Vy2 = -1.0f+heightPadding-offsetY-drag*(mouseY-dragMouseY);

  //Vertice coordinates, with zoom
  if(magnification > 1.0f)
    {
      //Magnify picture by pulling vertices outwards by zoom level
      Vx1 = Vx1*magnification;
      Vx2 = Vx2*magnification;
      Vy1 = Vy1*magnification;
      Vy2 = Vy2*magnification;
    }

  //Express vertices as points (for rotation)
  float pointx1 = Vx1;
  float pointx2 = Vx2;
  float pointx3 = Vx1;
  float pointx4 = Vx2;

  float pointy1 = Vy1;
  float pointy2 = Vy1;
  float pointy3 = Vy2;
  float pointy4 = Vy2;

  //Apply rotation to each point
  if(angle > 0.0f)
    {
      float saveX1 = pointx1;
      float saveX2 = pointx2;
      float saveX3 = pointx3;
      float saveX4 = pointx4;

      //POINT 1
      pointx1 = (pointx1*cos(angle)) - (pointy1*sin(angle));
      pointy1 = (saveX1*sin(angle)) + (pointy1*cos(angle));

      //POINT 2
      pointx2 = (pointx2*cos(angle)) - (pointy2*sin(angle));
      pointy2 = (saveX2*sin(angle)) + (pointy2*cos(angle));

      //POINT 3
      pointx3 = (pointx3*cos(angle)) - (pointy3*sin(angle));
      pointy3 = (saveX3*sin(angle)) + (pointy3*cos(angle));

      //POINT 4
      pointx4 = (pointx4*cos(angle)) - (pointy4*sin(angle));
      pointy4 = (saveX4*sin(angle)) + (pointy4*cos(angle));
    }
  
  // three vertex positions and associated colours of a triangle
  const GLfloat vertices[][2] = {

    //1st triangle
    { pointx1, pointy1 },
    { pointx2, pointy2 },
    { pointx4, pointy4 },

    //2nd triangle
    { pointx3, pointy3 },
    { pointx1, pointy1 },
    { pointx4, pointy4 }
    
  };

  //Texture coordinates
  Tx1 = 0.0f;
  Tx2 = width;
  Ty1 = height;
  Ty2 = 0.0f;
  
  const GLfloat textureCoords[][2] = {
    //1st triangle
    {Tx1, Ty1},
    {Tx2, Ty1},
    {Tx2, Ty2},
		
    //2nd triangle
    {Tx1, Ty2},
    {Tx1, Ty1},
    {Tx2, Ty2}
     
    };

  const GLfloat colours[][3] = {
    //1st triangle
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },

    //2nd triangle
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f }
  };
  geometry->elementCount = 6;

	// these vertex attribute indices correspond to those specified for the
	// input variables in the vertex shader
	const GLuint VERTEX_INDEX = 0;
	const GLuint COLOUR_INDEX = 1;
	const GLuint TEXTURE_INDEX = 2;

	// create an array buffer object for storing our vertices
	glGenBuffers(1, &geometry->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//
	glGenBuffers(1, &geometry->textureBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, geometry->textureBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), textureCoords, GL_STATIC_DRAW);

	// create another one for storing our colours
	glGenBuffers(1, &geometry->colourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colours), colours, GL_STATIC_DRAW);

	// create a vertex array object encapsulating all our vertex attributes
	glGenVertexArrays(1, &geometry->vertexArray);
	glBindVertexArray(geometry->vertexArray);

	// associate the position array with the vertex array object
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
	glVertexAttribPointer(VERTEX_INDEX, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VERTEX_INDEX);

	// Tell openGL how the data is formatted
	glBindBuffer(GL_ARRAY_BUFFER, geometry->textureBuffer);
	glVertexAttribPointer(TEXTURE_INDEX, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(TEXTURE_INDEX);

	// assocaite the colour array with the vertex array object
	glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
	glVertexAttribPointer(COLOUR_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(COLOUR_INDEX);

	// unbind our buffers, resetting to default state
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// check for OpenGL errors and return false if error occurred
	return !CheckGLErrors();
}


// deallocate geometry-related objects
void DestroyGeometry(MyGeometry *geometry)
{
	// unbind and destroy our vertex array object and associated buffers
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &geometry->vertexArray);
	glDeleteBuffers(1, &geometry->vertexBuffer);
	glDeleteBuffers(1, &geometry->colourBuffer);
}

// --------------------------------------------------------------------------
// Rendering function that draws our scene to the frame buffer

void RenderScene(MyGeometry *geometry, MyTexture* texture, MyShader *shader)
{
	// clear screen to a dark grey colour
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// bind our shader program and the vertex array object containing our
	// scene geometry, then tell OpenGL to draw our geometry
	glUseProgram(shader->program);
	glBindVertexArray(geometry->vertexArray);
	glBindTexture(texture->target, texture->textureID);
	glDrawArrays(GL_TRIANGLES, 0, geometry->elementCount);

	// reset state to default (no shader or geometry bound)
	glBindTexture(texture->target, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	// check for an report any OpenGL errors
	CheckGLErrors();
}

// --------------------------------------------------------------------------
// GLFW callback functions
MyShader shader;
MyGeometry geometry;
MyTexture texture;

// reports GLFW errors
void ErrorCallback(int error, const char* description)
{
	cout << "GLFW ERROR " << error << ":" << endl;
	cout << description << endl;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
      {
	drag = true;

	//get current mouse coordinates while dragging
	dragMouseX = mouseX;
	dragMouseY = mouseY;
	
	// call function to create and fill buffers with geometry data
	if (!InitializeGeometry(&geometry, texture.height, texture.width))
	  cout << "Program failed to intialize geometry!" << endl;
	    
	RenderScene(&geometry, &texture, &shader); //render scene with texture
      }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
      {
	//offset = offset + (current mouse - last drag mouse coordinates)
	offsetX = offsetX + (mouseX - dragMouseX);
	offsetY = offsetY + (mouseY - dragMouseY);
	
	drag = false;
      }
}

static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
  //Get normalized mouse coordinates, account for zoom
  mouseX = (xpos/256.f -1.0f)/magnification;
  mouseY = (ypos/256.f -1.0f)/magnification;

  //Get mouse coordinates when image is rotated
  float saveMouseX = mouseX;
	
  mouseX = (mouseX*cos(angle)) - (mouseY*sin(angle));
  mouseY = (saveMouseX*sin(angle)) + (mouseY*cos(angle));
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  
  //zoom out
  if(yoffset < 0 && magnification > 1.0f)
    {
      //if magnification is not on lowest level...
      //decrease zoom level
      magnification = magnification - 1.0f;
      
      //zoom into centre of window
      offsetX = offsetX; 
      offsetY = offsetY; 
    }

  //zoom in (limited to 10 levels of magnification)
  if(yoffset > 0 && magnification < 10.0f)
    {
      //if magnification is not on highest level...
      //increase zoom level
      magnification = magnification + 1.0f;
      
      //zoom into centre of window
      offsetX = offsetX; 
      offsetY = offsetY;
    }

  // call function to create and fill buffers with geometry data
  if (!InitializeGeometry(&geometry, texture.height, texture.width))
    cout << "Program failed to intialize geometry!" << endl;
	    
  RenderScene(&geometry, &texture, &shader); //render scene with texture
}

void resetVal()
{
  angle = 0.0f;
  magnification = 1.0f;
  mouseX = 0.0f;
  mouseY = 0.0f;
  offsetX = 0.0f;
  offsetY = 0.0f;
}

void resetBool()
{
  orgColor = false;
    
  gaus1 = false;
  gaus2 = false;
  gaus3 = false;

  g1 = false;
  g2 = false;
  g3 = false;
  sepia = false;

  sobel1 = false;
  sobel2 = false;
  unsharp = false;
}

// handles keyboard input events
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	
	else if(key == GLFW_KEY_1 && action == GLFW_PRESS)
	  {
	    resetVal();
	    resetBool();
	    orgColor = true;
	    
	    if(!InitializeTexture(&texture,file1, GL_TEXTURE_RECTANGLE))
	      cout << "Program failed to intialize texture!" << endl;
	  }
	
	else if(key == GLFW_KEY_2 && action == GLFW_PRESS)
	  {
	    resetVal();
	    resetBool();
	    orgColor = true;
	    
	    if(!InitializeTexture(&texture,file2, GL_TEXTURE_RECTANGLE))
	      cout << "Program failed to intialize texture!" << endl;
	  }

	else if(key == GLFW_KEY_3 && action == GLFW_PRESS)
	  {
	    resetVal();
	    resetBool();
	    orgColor = true;
	    
	    if(!InitializeTexture(&texture,file3, GL_TEXTURE_RECTANGLE))
	      cout << "Program failed to intialize texture!" << endl;
	  }

	else if(key == GLFW_KEY_4 && action == GLFW_PRESS)
	  {
	    resetVal();
	    resetBool();
	    orgColor = true;
	    
	    if(!InitializeTexture(&texture,file4, GL_TEXTURE_RECTANGLE))
	      cout << "Program failed to intialize texture!" << endl;
	  }

	else if(key == GLFW_KEY_5 && action == GLFW_PRESS)
	  {
	    resetVal();
	    resetBool();
	    orgColor = true;
	    
	    if(!InitializeTexture(&texture,file5, GL_TEXTURE_RECTANGLE))
	      cout << "Program failed to intialize texture!" << endl;
	  }

	else if(key == GLFW_KEY_6 && action == GLFW_PRESS)
	  {
	    resetVal();
	    resetBool();
	    orgColor = true;
	    
	    if(!InitializeTexture(&texture,file6, GL_TEXTURE_RECTANGLE))
	      cout << "Program failed to intialize texture!" << endl;
	  }

	else if(key == GLFW_KEY_R && action == GLFW_PRESS)
	  {
	    angle = angle + 3.141592f/6.0f;
	    counter++;

	    if(counter==12)
	      {
		angle = 0.0f;
		counter = 0;
	      }
	  }

	//Original colors
	else if(key == GLFW_KEY_Q && action == GLFW_PRESS)
	  {
	    resetBool();
	    orgColor = true;
	  }

	//Grayscale 1
	else if(key == GLFW_KEY_W && action == GLFW_PRESS)
	  {
	    resetBool();
	    g1 = true;
	  }

	//Grayscale 2
	else if(key == GLFW_KEY_E && action == GLFW_PRESS)
	  {
	    resetBool();
	    g2 = true;
	  }

	//Grayscale 3
	else if(key == GLFW_KEY_A && action == GLFW_PRESS)
	  {
	    resetBool();
	    g3 = true;
	  }

	//Sepia filter
	else if(key == GLFW_KEY_S && action == GLFW_PRESS)
	  {
	    resetBool();
	    sepia = true;
	  }

	//3x3 Gaussian
	else if(key == GLFW_KEY_Z && action == GLFW_PRESS)
	  {
	    resetBool();
	    orgColor = true;
	    gaus1 = true;
	  }

	//5x5 Gaussian
	else if(key == GLFW_KEY_X && action == GLFW_PRESS)
	  {
	    resetBool();
	    orgColor = true;
	    gaus2 = true;
	  }

	//7x7 Gaussian
	else if(key == GLFW_KEY_C && action == GLFW_PRESS)
	  {
	    resetBool();
	    orgColor = true;
	    gaus3 = true;
	  }

	//vertical sobel
	else if(key == GLFW_KEY_V && action == GLFW_PRESS)
	  {
	    resetBool();
	    orgColor = true;
	    sobel1 = true;
	  }

	//horizontal sobel
	else if(key == GLFW_KEY_B && action == GLFW_PRESS)
	  {
	    resetBool();
	    orgColor = true;
	    sobel2 = true;
	  }

	//unsharp mask
	else if(key == GLFW_KEY_N && action == GLFW_PRESS)
	  {
	    resetBool();
	    orgColor = true;
	    unsharp = true;
	  }
	// call function to load and compile shader programs
	if (!InitializeShaders(&shader)) {
	  cout << "Program could not initialize shaders, TERMINATING" << endl;
	}
	// call function to create and fill buffers with geometry data
	if (!InitializeGeometry(&geometry, texture.height, texture.width))
	  cout << "Program failed to intialize geometry!" << endl;
	    
	RenderScene(&geometry, &texture, &shader); //render scene with texture
}

// ==========================================================================
// PROGRAM ENTRY POINT

int main(int argc, char *argv[])
{
	// initialize the GLFW windowing system
	if (!glfwInit()) {
		cout << "ERROR: GLFW failed to initialize, TERMINATING" << endl;
		return -1;
	}
	glfwSetErrorCallback(ErrorCallback);

	// attempt to create a window with an OpenGL 4.1 core profile context
	GLFWwindow *window = 0;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(512, 512, "CPSC 453 OpenGL Boilerplate", 0, 0);
	if (!window) {
		cout << "Program failed to create GLFW window, TERMINATING" << endl;
		glfwTerminate();
		return -1;
	}

	// set keyboard callback function and make our context current (active)
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwMakeContextCurrent(window);

	// query and print out information about our OpenGL environment
	QueryGLVersion();

	// call function to load and compile shader programs
	if (!InitializeShaders(&shader)) {
		cout << "Program could not initialize shaders, TERMINATING" << endl;
		return -1;
	}

	if(!InitializeTexture(&texture,file1, GL_TEXTURE_RECTANGLE))
	  cout << "Program failed to intialize texture!" << endl;
	 
	
	// call function to create and fill buffers with geometry data
	if (!InitializeGeometry(&geometry, texture.height, texture.width))
		cout << "Program failed to intialize geometry!" << endl;

	// run an event-triggered main loop
	while (!glfwWindowShouldClose(window))
	  {
	    InitializeGeometry(&geometry, texture.height, texture.width);
	    // call function to draw our scene
	    RenderScene(&geometry, &texture, &shader); //render scene with texture
	    glfwSwapBuffers(window);
	    glfwPollEvents();
	  }

	// clean up allocated resources before exit
	DestroyGeometry(&geometry);
	DestroyShaders(&shader);
	glfwDestroyWindow(window);
	glfwTerminate();

	//cout << "Goodbye!" << endl;
	return 0;
}

// ==========================================================================
// SUPPORT FUNCTION DEFINITIONS

// --------------------------------------------------------------------------
// OpenGL utility functions

void QueryGLVersion()
{
	// query opengl version and renderer information
	string version = reinterpret_cast<const char *>(glGetString(GL_VERSION));
	string glslver = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
	string renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

	cout << "OpenGL [ " << version << " ] "
		<< "with GLSL [ " << glslver << " ] "
		<< "on renderer [ " << renderer << " ]" << endl;
}

bool CheckGLErrors()
{
	bool error = false;
	for (GLenum flag = glGetError(); flag != GL_NO_ERROR; flag = glGetError())
	{
		cout << "OpenGL ERROR:  ";
		switch (flag) {
		case GL_INVALID_ENUM:
			cout << "GL_INVALID_ENUM" << endl; break;
		case GL_INVALID_VALUE:
			cout << "GL_INVALID_VALUE" << endl; break;
		case GL_INVALID_OPERATION:
			cout << "GL_INVALID_OPERATION" << endl; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << endl; break;
		case GL_OUT_OF_MEMORY:
			cout << "GL_OUT_OF_MEMORY" << endl; break;
		default:
			cout << "[unknown error code]" << endl;
		}
		error = true;
	}
	return error;
}

// --------------------------------------------------------------------------
// OpenGL shader support functions

// reads a text file with the given name into a string
string LoadSource(const string &filename)
{
	string source;

	ifstream input(filename.c_str());
	if (input) {
		copy(istreambuf_iterator<char>(input),
			istreambuf_iterator<char>(),
			back_inserter(source));
		input.close();
	}
	else {
		cout << "ERROR: Could not load shader source from file "
			<< filename << endl;
	}

	return source;
}

// creates and returns a shader object compiled from the given source
GLuint CompileShader(GLenum shaderType, const string &source)
{
	// allocate shader object name
	GLuint shaderObject = glCreateShader(shaderType);

	// try compiling the source as a shader of the given type
	const GLchar *source_ptr = source.c_str();
	glShaderSource(shaderObject, 1, &source_ptr, 0);
	glCompileShader(shaderObject);

	// retrieve compile status
	GLint status;
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetShaderInfoLog(shaderObject, info.length(), &length, &info[0]);
		cout << "ERROR compiling shader:" << endl << endl;
		cout << source << endl;
		cout << info << endl;
	}

	return shaderObject;
}

// creates and returns a program object linked from vertex and fragment shaders
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader)
{
	// allocate program object name
	GLuint programObject = glCreateProgram();

	// attach provided shader objects to this program
	if (vertexShader)   glAttachShader(programObject, vertexShader);
	if (fragmentShader) glAttachShader(programObject, fragmentShader);

	// try linking the program with given attachments
	glLinkProgram(programObject);

	// retrieve link status
	GLint status;
	glGetProgramiv(programObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetProgramInfoLog(programObject, info.length(), &length, &info[0]);
		cout << "ERROR linking shader program:" << endl;
		cout << info << endl;
	}

	return programObject;
}
