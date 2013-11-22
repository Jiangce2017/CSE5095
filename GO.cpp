

//#include "GO.h"

//Aplication includes
#include "rnd.h"
#include "centerpoint.h"
#include "utils.h"

//OpenGL includes
#include <glew-1.10.0\include\GL\glew.h>
#include <glfw-2.7.6\include\GL\glfw.h> //Toolkit
#include <glm-0.9.4.0\glm\glm.hpp> //Matrix manipulations
#include <glm-0.9.4.0\glm\gtc\matrix_transform.hpp> //View transforms
#include <glm-0.9.4.0\glm\gtc\quaternion.hpp> //Quaternions operations

// Macro specific keys from keyboard
#define GLFW_KEY_A   65
#define GLFW_KEY_D   68
#define GLFW_KEY_S   83
#define GLFW_KEY_W   87

//Pi
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Window dimensions and A/R
static int	CurrentWidth = 800,
			CurrentHeight = 600,
			WindowHandle = 0;
static float aspectRatio = (float)CurrentWidth/CurrentHeight;

// View globals
static float horizontalAngle = M_PI, // Initial horizontal angle : toward -Z
			 verticalAngle = -M_PI/2.0f, // Initial vertical angle
			 FoV = 75.0f; // Initial Field of View
static const float keySpeed = 0.1, 
				   mouseCursorSpeed = 0.005f,
				   mouseWheelSpeed = 4.0f;
static glm::vec3 vPosition = glm::vec3( 0.0f, 5.0f, 2.0f ), // Initial camera position
				 vDirection, 
				 vRight, 
				 vUp;
static glm::mat4 ViewMatrix, 
				 ProjectionMatrix, 
				 MVP;
static const glm::mat4 ModelMatrix = glm::mat4(1.0f);

static int do_redraw = 1, // Do redraw? (repaint the window only as needed)
		   xpos = 0, // Mouse position
		   ypos = 0; // Mouse position

// IDs for VBOs and shaders
GLuint ProgramID, //ID of our shader
			  MatrixID, //ID for view matrix transformations3
			  VertexShaderId,
			  FragmentShaderId,
			  VertexArrayID,
			  VertexBufferID,
			  ColorBufferID;

// GLSL vertex shader
static const GLchar* VertexShader =
{
    "#version 330 core\n"\
 
    "layout(location = 0) in vec3 vertexPosition_modelspace;\n"\
    "layout(location = 1) in vec3 vertexColor;\n"\
    "out vec3 fragmentColor;\n"\

	"uniform mat4 MVP;\n"\
 
    "void main(void)\n"\
    "{\n"\
    "   gl_Position =  MVP * vec4(vertexPosition_modelspace,1);\n"\
    "   fragmentColor = vertexColor;\n"\
    "}\n"
};

// GLSL fragment shader
static const GLchar* FragmentShader =
{
    "#version 330 core\n"\
 
    "in vec3 fragmentColor;\n"\
    "out vec3 color;\n"\
 
    "void main(void)\n"\
    "{\n"\
    "   color = fragmentColor;\n"\
    "}\n"
};


// Recompute the MVP for current view setup
void updateView(void)
{
	//printf("Vangle %f\n", verticalAngle);
	//printf("Hangle %f\n", horizontalAngle);
	//printf("Position [%f;%f;%f]\n",vPosition.x,vPosition.y,vPosition.z);

	// First calculate view Direction : Spherical coordinates to Cartesian coordinates conversion
	vDirection.x = cos(verticalAngle) * sin(horizontalAngle);
	vDirection.y = sin(verticalAngle);
	vDirection.z = cos(verticalAngle) * cos(horizontalAngle);

	// Now calculate the Right vector (remember the World has that weird orientation away from screen)
	vRight.x = sin(horizontalAngle - M_PI/2.0f);
	vRight.y = 0.0f;
	vRight.z = cos(horizontalAngle - M_PI/2.0f);

	// Finally, the Up vector
	vUp = glm::cross( vDirection, vRight );

	// Now calculate the MVP matrices one at a time
	// Projection matrix
	ProjectionMatrix = glm::perspective(FoV, //Field of view
										aspectRatio, //Aspect ratio
										0.1f, //Screen units
										100.0f); //World units
	// Camera matrix
	ViewMatrix = glm::lookAt(vPosition,           // Camera is here
							 vPosition+vDirection, // and looks here : at the same position, plus "direction"
							 vUp);                // Head is up (set to 0,-1,0 to look upside-down)
						   
	// Model matrix is always Identity Matrix (no scalling, shearing, translation, rotation or what-not)
	MVP = ProjectionMatrix * ViewMatrix * ModelMatrix; // Remember, matrix multiplication is the other way around
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]); // Send our transformation to the currently bound shader, in the "MVP" uniform
}

// Mouse position callback function
void GLFWCALL MousePosFun(int x, int y)
{
	// Reorient only on left mouse button pressed
	if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		// Calculate new orientation
		horizontalAngle += mouseCursorSpeed * float(x - xpos); //Some people call this "angleX"
		verticalAngle   -= mouseCursorSpeed * float(y - ypos); // ----||---- "angleY"
		do_redraw = 1;
	}

	float panx, pany;
	// Pan if the middle button was pressed
	if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
	{
		panx = mouseCursorSpeed * float(x - xpos);
		pany = mouseCursorSpeed * float(y - ypos);
		vPosition += vRight * panx; // Strafe left/right
		vPosition += vUp * pany; // Pan up/down
		do_redraw = 1;
	}

    // Remember cursor position
    xpos = x;
    ypos = y;
}

// Mouse wheel is rotated
void GLFWCALL MouseWheelFun(int wheel)
{
	float currFoV = FoV + mouseWheelSpeed * wheel;
	FoV = (currFoV < 180.0f && currFoV > 0.0f) ? currFoV : FoV; // Field of view can only be between [0 - 180]deg
	glfwSetMouseWheel(0); //Reset the wheel relative position
	do_redraw = 1;
}

// Keyboard input callback function
void GLFWCALL KeyboardFun(int key, int state)
{
	// These are for now panning events
	if (state == GLFW_PRESS){
		switch (key){
		case GLFW_KEY_HOME: //Orthographic projection
			verticalAngle = -M_PI;
			horizontalAngle = 2.0f * M_PI;
			vPosition = glm::vec3( 0.0f, 0.0f, 20.0f );
			break;
		default:
			break; //No default key mapped
		}
		do_redraw = 1;
	}
}

// Recalculate the aspect ratio and limits when window has been resized
void GLFWCALL WindowResize( int w, int h )
{
	CurrentWidth = w > 30 ? w : 30; //To prevent division by 0
	CurrentHeight = h > 30 ? h : 30;

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION); /* switch matrix mode */
	glLoadIdentity();
	if (w <= h){
		aspectRatio = (GLfloat)h/(GLfloat)w; //Recalculate the A/R
		glm::ortho(-2.0 * aspectRatio, 2.0 * aspectRatio, -2.0, 2.0);
	}
	else {
		aspectRatio = (GLfloat)w/(GLfloat)h; //Recalculate the A/R
		glm::ortho(-2.0 * aspectRatio, 2.0 * aspectRatio, -2.0, 2.0);
	}
	glMatrixMode(GL_MODELVIEW); /* return to modelview mode */
	
	do_redraw = 1; //Ask repainting
}

// Window has been refreshed, redraw everything
void GLFWCALL windowRefreshFun(void)
{
	do_redraw = 1;
}


// Draw a 2D grid (used to orient better)
static void drawGrid(float scale, int steps, GLfloat *gridVertex, GLfloat *gridColour)
{
    int i;
    float x, y;

    // Set grid color
    glColor3f(0.0f, 0.5f, 0.5f);
	
    // Horizontal lines
    x = scale * 0.5f * (float) (steps - 1);
    y = -scale * 0.5f * (float) (steps - 1);
    for (i = 0;  i < steps;  i++)
    {
		gridVertex[4*i+0] = -x;
		gridVertex[4*i+1] = y;
		gridVertex[4*i+2] = x;
		gridVertex[4*i+3] = y;
        //glVertex3f(-x, y, 0.0f);
        //glVertex3f(x, y, 0.0f);
        y += scale;
    }

    //// Vertical lines
    //x = -scale * 0.5f * (float) (steps - 1);
    //y = scale * 0.5f * (float) (steps - 1);
    //for (i = 0;  i < steps;  i++)
    //{
    //    glVertex3f(x, -y, 0.0f);
    //    glVertex3f(x, y, 0.0f);
    //    x += scale;
    //}

	//// Draw the Floor
	//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	//glDisableClientState(GL_COLOR_ARRAY);
	//glEnableClientState(GL_VERTEX_ARRAY);
	//glVertexPointer(3, GL_FLOAT, 0, zFloorVertices);
	//glDrawArrays(GL_LINES, 0, 42);
	//glVertexPointer(3, GL_FLOAT, 0, xFloorVertices);
	//glDrawArrays(GL_LINES, 0, 42);
}

void InitWindow(void)
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		exit(EXIT_FAILURE);
	}

	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	WindowHandle = glfwOpenWindow( CurrentWidth, CurrentHeight, 0,0,0,0, 32,0, GLFW_WINDOW );

	if( WindowHandle < 1 )
	{
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n" );
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetWindowTitle( "Geometric Separators" );
}

void Initialize(void)
{
	// Start the show
	InitWindow();

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	GLenum GlewInitResult = glewInit(); //Initialize GLEW
	
	if (GLEW_OK != GlewInitResult) {
		fprintf(stderr, "GLEW_ERROR: %s\n", glewGetErrorString(GlewInitResult));
		exit(EXIT_FAILURE);
	}

	fprintf(stdout,	"INFO: OpenGL Version: %s\n", glGetString(GL_VERSION)); //GL version

	// Ensure we can capture a continuous panning event (hold down a pan key)
	glfwEnable( GLFW_KEY_REPEAT );
	glClearColor(0.0f, 0.0f, 0.35f, 0.0f); // Dark blue background
	glEnable(GL_DEPTH_TEST); // Enable depth test
	glDepthFunc(GL_LESS); // Accept fragment if it closer to the camera than the former one
	glEnable(GL_CULL_FACE); // Cull triangles which normal is not towards the camera

	// Register callback functions
	glfwSetWindowSizeCallback( WindowResize ); //Window resize
	glfwSetWindowRefreshCallback( windowRefreshFun ); //Window refresh
	glfwSetMousePosCallback( MousePosFun ); //Mouse moves
	glfwSetKeyCallback( KeyboardFun ); //Some key is pressed
	glfwSetMouseWheelCallback( MouseWheelFun ); //Mouse wheel is rotated

	// Generate the VAO(s) (needs to be in the same place with the glewInit() for some magical reason)
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
}

// Error check for all GL operations
void CheckGLErrors()
{
  GLenum err ;
  while ( (err = glGetError()) != GL_NO_ERROR )
    fprintf( stderr, "OpenGL Error: %s\n", glewGetErrorString ( err ) );
}

// Creates a vertex array to load onto the display buffer
bool sPoint2VertexArray(sPoint const* sPtIn, unsigned n, GLfloat **gfOut)
{
	*gfOut = new GLfloat[3*n]; //Allocate space here, delete after passing to buffer
	for (unsigned i=0; i<n; i++)
	{
		(*gfOut)[0+3*i] = (GLfloat) sPtIn[i].x;
		(*gfOut)[1+3*i] = (GLfloat) sPtIn[i].y;
		(*gfOut)[2+3*i] = (GLfloat) sPtIn[i].w;
	}
	return true;
}

// Creates a colour array to load onto the display buffer
bool sPoint2ColourArray(sPoint const* sPtIn, unsigned n, GLfloat **gfOut)
{
	*gfOut = new GLfloat[3*n]; //Allocate space and load with some colour
	for (unsigned i=0; i<n; i++)
	{
		(*gfOut)[0+3*i] = 1.0f;
		(*gfOut)[1+3*i] = 0.1f;
		(*gfOut)[2+3*i] = 0.4f;
	}
	return true;
}

// Creates and bounds a VBO to an array of vertices plus colour
//mofify to take argument (GLfloat* vertexArray, GLfloat* colourArray, N = # vertices)
void CreateVBO(void)
{
	// Generate the set of points
	sPoint *points = NULL;
	unsigned N;
	generatePoints(&points,N);

	// Calculate the partition
	sPoint pntCenter, sPartCenter;
	double dRadius;
	getPartition(points,N,&pntCenter,&dRadius,&sPartCenter);

	// Arrange the points into an array
	static GLfloat *gfVertices = NULL;
	sPoint2VertexArray(points, N, &gfVertices);
	static GLfloat *gfColours = NULL;
	sPoint2VertexArray(points, N, &gfColours);

	glGenBuffers(1, &VertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, 3*N*sizeof(gfVertices[0]), &gfVertices[0], GL_STATIC_DRAW); //Transfer to GPU memory (this is the point set)
	//glBufferData(GL_ARRAY_BUFFER, (8*N+4*40)*sizeof(gfVertices[0]), &gfVertices[0], GL_STATIC_DRAW); //Transfer to GPU memory (this is the point set)
	//glBufferSubData(GL_ARRAY_BUFFER,8*N*sizeof(gfVertices[0]),4*40*sizeof(gfPartition[0]),&gfPartition[0]); //Add the partition points to the display buffer
	
	glGenBuffers(1,&ColorBufferID);
	glBindBuffer(GL_ARRAY_BUFFER,ColorBufferID);
	glBufferData(GL_ARRAY_BUFFER, 3*N*sizeof(gfColours[0]), &gfColours[0], GL_STATIC_DRAW); //Transfer to GPU memory (this colours both the point sets)
	//glBufferData(GL_ARRAY_BUFFER, (8*N+4*40)*sizeof(gfColours[0]), &gfColours[0], GL_STATIC_DRAW); //Transfer to GPU memory (this colours both the point sets)
	//glBufferSubData(GL_ARRAY_BUFFER,8*N*sizeof(gfColours[0]),4*40*sizeof(gfPartitionColours[0]),&gfPartitionColours[0]); //Add the partition points to the display buffer

	//Dont forget to deallocate if you don't need them anymore
	delete [] points; points = NULL;
	delete [] gfVertices; gfVertices = NULL;
	delete [] gfColours; gfColours = NULL;
 }

void DestroyVBO(void)
{
	glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
         
    glBindBuffer(GL_ARRAY_BUFFER, 0);
 
    glDeleteBuffers(1, &ColorBufferID);
    glDeleteBuffers(1, &VertexBufferID);
 
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VertexArrayID);
}

void CreateShaders(void)
{
    // Create and compile the GLSL vertex shader 
    VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(VertexShaderId, 1, &VertexShader, NULL);
    glCompileShader(VertexShaderId);

	 // Create and compile the GLSL fragment shader 
    FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(FragmentShaderId, 1, &FragmentShader, NULL);
    glCompileShader(FragmentShaderId);

	// Create the program and attach the shaders to it
    ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderId);
    glAttachShader(ProgramID, FragmentShaderId);
    glLinkProgram(ProgramID);
    glUseProgram(ProgramID);

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(ProgramID, "MVP");
 }

void DestroyShaders(void)
{ 
    glUseProgram(0); //Invoke current program
 
    glDetachShader(ProgramID, VertexShaderId);
    glDetachShader(ProgramID, FragmentShaderId);
 
    glDeleteShader(FragmentShaderId);
    glDeleteShader(VertexShaderId);
 
    glDeleteProgram(ProgramID);
}

void Cleanup(void)
{
	DestroyVBO();
    DestroyShaders();
}


void DrawCircle(float cx, float cy, float r, int num_segments, GLfloat **gfOut) 
{ 
	float theta = 2 * 3.1415926 / float(num_segments-1); 
	float tangetial_factor = tanf(theta);//calculate the tangential factor 
	float radial_factor = cosf(theta);//calculate the radial factor 
	float x = r; //we start at angle = 0 
	float y = 0; 

	*gfOut = new GLfloat[num_segments*4];
    
	for(int ii = 0; ii < num_segments-1; ii++) 
	{ 
		(*gfOut)[ii*4+0] = x + cx;
		(*gfOut)[ii*4+1] = y + cy;
		(*gfOut)[ii*4+2] = 0.0f;
		(*gfOut)[ii*4+3] = 1.0f;
        
		//Calculate the tangential vector {radial vector is (x,y)}. Simply flip coordinates and negate one
		float tx = -y; 
		float ty = x; 
        
		//add the tangential vector 
		x += tx * tangetial_factor; 
		y += ty * tangetial_factor; 
        
		//correct using the radial factor 
		x *= radial_factor; 
		y *= radial_factor; 
	} 
	//Last point is identical to the first (to have a closed loop)
	(*gfOut)[(num_segments-1)*4+0] = (*gfOut)[0];
	(*gfOut)[(num_segments-1)*4+1] = (*gfOut)[1];
	(*gfOut)[(num_segments-1)*4+2] = (*gfOut)[2];
	(*gfOut)[(num_segments-1)*4+3] = (*gfOut)[3];
}

void runRenderLoop()
{
	// Start by butting the cursor in the middle of the window 
	glfwSetMousePos(CurrentWidth/2, CurrentHeight/2); 

	CreateVBO();
	CreateShaders();
	
	//// Generate the set of points
	//sPoint *points = NULL;
	//unsigned N;
	//generatePoints(&points,N);

	//// Calculate the partition
	//sPoint pntCenter, sPartCenter;
	//double dRadius;
	//getPartition(points,N,&pntCenter,&dRadius,&sPartCenter);


	// Sample the partition to we can show it on the screen
	//GLfloat *gfPartition = NULL;
	//DrawCircle(sPartCenter.x,sPartCenter.y,dRadius,40,&gfPartition);
	
	// Arrange the points into an array
	//static GLfloat *gfVertices = NULL;
	//sPoint2VertexArray(points, N, &gfVertices);
	//static GLfloat *gfColours = NULL;
	//sPoint2VertexArray(points, N, &gfColours);
	//delete [] points; points = NULL; 

	// Colour the points on the paraboloid in yellow and the ones in the plane with magenta (sort-of)
	//static GLfloat *gfColours = new GLfloat[8*N];
	//for (unsigned i=0; i<N; i++)
	//{
	//	gfColours[0+4*i] = 1.0f; //R
	//	gfColours[1+4*i] = 1.0f; //G
	//	gfColours[2+4*i] = 0.1f; //B
	//	gfColours[3+4*i] = 1.0f; //Alpha-chanel
	//	gfColours[4*(N+i)+0] = 1.0f; //R
	//	gfColours[4*(N+i)+1] = 0.1f; //G
	//	gfColours[4*(N+i)+2] = 0.4f; //B
	//	gfColours[4*(N+i)+3] = 1.0f; //Alpha-chanel
	//}

	// Colour the points on the partition in white
	//static GLfloat *gfPartitionColours = new GLfloat[4*40];
	//for (unsigned i=0; i<40; i++)
	//{
	//	gfPartitionColours[0+4*i] = 1.0f; //R
	//	gfPartitionColours[1+4*i] = 1.0f; //G
	//	gfPartitionColours[2+4*i] = 1.0f; //B
	//	gfPartitionColours[3+4*i] = 1.0f; //Alpha-chanel
	//}

	//glGenBuffers(1, &VertexBufferID);
	//glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
	//glBufferData(GL_ARRAY_BUFFER, 3*N*sizeof(gfVertices[0]), &gfVertices[0], GL_STATIC_DRAW); //Transfer to GPU memory (this is the point set)
	////glBufferData(GL_ARRAY_BUFFER, (8*N+4*40)*sizeof(gfVertices[0]), &gfVertices[0], GL_STATIC_DRAW); //Transfer to GPU memory (this is the point set)
	////glBufferSubData(GL_ARRAY_BUFFER,8*N*sizeof(gfVertices[0]),4*40*sizeof(gfPartition[0]),&gfPartition[0]); //Add the partition points to the display buffer
	//
	//glGenBuffers(1,&ColorBufferID);
	//glBindBuffer(GL_ARRAY_BUFFER,ColorBufferID);
	//glBufferData(GL_ARRAY_BUFFER, 3*N*sizeof(gfColours[0]), &gfColours[0], GL_STATIC_DRAW); //Transfer to GPU memory (this colours both the point sets)
	////glBufferData(GL_ARRAY_BUFFER, (8*N+4*40)*sizeof(gfColours[0]), &gfColours[0], GL_STATIC_DRAW); //Transfer to GPU memory (this colours both the point sets)
	////glBufferSubData(GL_ARRAY_BUFFER,8*N*sizeof(gfColours[0]),4*40*sizeof(gfPartitionColours[0]),&gfPartitionColours[0]); //Add the partition points to the display buffer

	do{
		if (do_redraw)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen
			glUseProgram(ProgramID); // Use our shader

			// Recalculate scene (using MVP matrix)
			updateView();

			// 1rst attribute buffer : vertices
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
			glVertexAttribPointer(
				0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);

			// 2nd attribute buffer : colors
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, ColorBufferID);
			glVertexAttribPointer(
				1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
				3,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);

			//// Draw the triangle !
			//glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles

			//// Draw the whole thing
			glDrawArrays(GL_POINTS, 0, 15625); //Draw points on the paraboloid
			//glDrawArrays(GL_POINTS, N, N); //Draw points in the plane
			//glDrawArrays(GL_LINE_STRIP, 2*N, 40); //Draw the partition

			// Detach vertex attributes
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);

			//Stop redrawing stuff
			do_redraw = 0; 

			// Swap buffers
			glfwSwapBuffers();
		}
		
		// Wait for new events
        glfwWaitEvents();
	} 
	while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
		   glfwGetWindowParam( GLFW_OPENED ) ); // Check if the ESC key was pressed or the window was closed

	// Destroy VBO and Shaders
	Cleanup();

	//delete [] gfVertices; gfVertices = NULL;
	//delete [] gfColours; gfColours = NULL;
	//delete [] gfPartition; gfPartition = NULL;
	//delete [] gfPartitionColours; gfPartitionColours = NULL;

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	glfwCloseWindow();
	// Exit program
	exit( EXIT_SUCCESS );
}

