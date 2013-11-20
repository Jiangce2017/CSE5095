

#include "GO.h"

// Macro specific keys from keyboard
#define GLFW_KEY_A   65
#define GLFW_KEY_D   68
#define GLFW_KEY_S   83
#define GLFW_KEY_W   87

//Pi


// Window dimensions and A/R
static int	CurrentWidth = 800,
			CurrentHeight = 600,
			WindowHandle = 0;
static float aspectRatio = (float)CurrentHeight/CurrentWidth;

// View global variables
static float horizontalAngle = 6.29f; // Initial horizontal angle : toward -Z
static float verticalAngle = -1.76f; // Initial vertical angle
static float FoV = 75.0f; // Initial Field of View
static const float keySpeed = 0.1; // 3 units/second
static const float mouseCursorSpeed = 0.005f;
static const float mouseWheelSpeed = 4.0f;
static glm::vec3 position = glm::vec3( 0.138f, 4.021f, 1.600f ); // Initial camera position
static glm::vec3 direction;
static glm::vec3 right;
static glm::mat4 ViewMatrix;
static glm::mat4 ProjectionMatrix;
static const glm::mat4 ModelMatrix = glm::mat4(1.0f);
static glm::mat4 MVP;


// Do redraw? (repaint the window only as needed)
static int do_redraw = 1;

// Mouse position
static int xpos = 0, ypos = 0;

// IDs for VBOs and shaders
static GLuint ProgramID, //ID of our shader
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


//glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest){
//    start = normalize(start);
//    dest = normalize(dest);
// 
//    float cosTheta = dot(start, dest);
//    vec3 rotationAxis;
// 
//    if (cosTheta < -1 + 0.001f){
//        // special case when vectors in opposite directions:
//        // there is no "ideal" rotation axis
//        // So guess one; any will do as long as it's perpendicular to start
//        rotationAxis = cross(vec3(0.0f, 0.0f, 1.0f), start);
//        if (gtx::norm::length2(rotationAxis) < 0.01 ) // bad luck, they were parallel, try again!
//            rotationAxis = cross(vec3(1.0f, 0.0f, 0.0f), start);
// 
//        rotationAxis = normalize(rotationAxis);
//        return gtx::quaternion::angleAxis(180.0f, rotationAxis);
//    }
// 
//    rotationAxis = cross(start, dest);
// 
//    float s = sqrt( (1+cosTheta)*2 );
//    float invs = 1 / s;
// 
//    return quat(
//        s * 0.5f, 
//        rotationAxis.x * invs,
//        rotationAxis.y * invs,
//        rotationAxis.z * invs
//    );
// 
//}

// Recompute the MVP for current view setup
void drawNow(void)
{
	// First calculate view direction : Spherical coordinates to Cartesian coordinates conversion
	direction.x = cos(verticalAngle) * sin(horizontalAngle);
	direction.y = sin(verticalAngle);
	direction.z = cos(verticalAngle) * cos(horizontalAngle);

	printf("Vangle %f\n", verticalAngle);
	printf("Hangle %f\n", horizontalAngle);

	// Now calculate the Right vector (remember the World has that weird orientation away from screen)
	right.x = sin(horizontalAngle - 3.14f/2.0f);
	right.y = 0.0f;
	right.z = cos(horizontalAngle - 3.14f/2.0f);

	// Finally, the Up vector
	glm::vec3 up = glm::cross( right, direction );

	// Now calculate the MVP matrices one at a time
	// Projection matrix
	ProjectionMatrix = glm::perspective(FoV, //Field of view
										aspectRatio, //Aspect ratio
										0.1f, //Screen units
										100.0f); //World units
	// Camera matrix
	ViewMatrix = glm::lookAt(position,           // Camera is here
							 position+direction, // and looks here : at the same position, plus "direction"
							 up);                // Head is up (set to 0,-1,0 to look upside-down)
						   
	// Model matrix is always Identity Matrix (no scalling, shearing, translation, rotation or what-not)
	MVP = ProjectionMatrix * ViewMatrix * ModelMatrix; // Remember, matrix multiplication is the other way around
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]); // Send our transformation to the currently bound shader, in the "MVP" uniform
}

// Mouse position callback function
void GLFWCALL MousePosFun(int x, int y)
{
	// Reorient only on middle mouse button pressed
	if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
	{
		// Calculate new orientation
		horizontalAngle += mouseCursorSpeed * float(x - xpos); //Some people call this "angleX"
		verticalAngle   += mouseCursorSpeed * float(y - ypos); // ----//---- "angleY"
		do_redraw = 1;
	}

    // Remember cursor position
    xpos = x;
    ypos = y;
}

// Mouse wheel is rotated
void GLFWCALL MouseWheelFun(int wheel)
{
	FoV += mouseWheelSpeed * wheel;
	glfwSetMouseWheel(0); //Reset the wheel relative position
	do_redraw = 1;
}

// Keyboard input callback function
void GLFWCALL KeyboardFun(int key, int state)
{
	// These are for now panning events
	if (state == GLFW_PRESS){
		switch (key){
		case GLFW_KEY_W: //Mode forward
			position += direction * keySpeed;
			break;
		case GLFW_KEY_S: //Move backward
			position -= direction * keySpeed;
			break;
		case GLFW_KEY_A: //Strafe left
			position -= right * keySpeed;
			break;
		case GLFW_KEY_D: //Strafe right
			position += right * keySpeed;
			break;
		case GLFW_KEY_HOME: //Orthographic projection
			//position  = glm::vec3( 0.0f, 0.0f, 5.0f ); // Camera position
			//direction = glm::vec3( 0.0f, 0.0f, -1.0f ); // Camera direction
			verticalAngle = -3.1415f;
			horizontalAngle = 2.0f*3.1415f;
			break;
		default:
			break; //No default key mapped
		}
		do_redraw = 1;
	}
}

// Recalculate the aspect ratio and limits when window has been resized
void GLFWCALL WindowResize( int width, int height )
{
	CurrentWidth = width > 30 ? width : 30;
	CurrentHeight = height > 30 ? height : 30;
	aspectRatio = (float)height/width; //Recalculate the scene A/R
	do_redraw = 1;
}

// Window has been refreshed, redraw everything
void GLFWCALL windowRefreshFun(void)
{
	do_redraw = 1;
}

void Initialize(int argc, char* argv[])
{
	// Start the show
	InitWindow(argc, argv);

	// Initialize GLEW
	GLenum GlewInitResult = glewInit();
	glewExperimental = true; // Needed for core profile
	if (GLEW_OK != GlewInitResult) {
		fprintf(stderr, "GLEW_ERROR: %s\n", glewGetErrorString(GlewInitResult));
		exit(EXIT_FAILURE);
	}
	
	fprintf(
		stdout,
		"INFO: OpenGL Version: %s\n",
		glGetString(GL_VERSION)
	);

	// Ensure we can capture a continuous panning event (hold down a pan key)
	glfwEnable( GLFW_KEY_REPEAT );
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f); // Dark blue background
	glEnable(GL_DEPTH_TEST); // Enable depth test
	glDepthFunc(GL_LESS); // Accept fragment if it closer to the camera than the former one
	glEnable(GL_CULL_FACE); // Cull triangles which normal is not towards the camera

	// Register callback functions
	glfwSetWindowSizeCallback( WindowResize ); //Window resize
	glfwSetWindowRefreshCallback( windowRefreshFun ); //Window refresh
	glfwSetMousePosCallback( MousePosFun ); //Mouse moves
	glfwSetKeyCallback( KeyboardFun ); //Some key is pressed
	glfwSetMouseWheelCallback( MouseWheelFun ); //Mouse wheel is rotated
}

void InitWindow(int argc, char* argv[])
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


void CreateVBO(void)
{
	GLfloat Vertices[] = {
		-0.8f, -0.8f, 0.0f, 1.0f,
		 0.0f,  0.8f, 0.0f, 1.0f,
		 0.8f, -0.8f, 0.0f, 1.0f
	};

	GLfloat Colors[] = {
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f
	};

	GLenum ErrorCheckValue = glGetError();
	
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	glGenBuffers(1, &VertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	
	glGenBuffers(1, &ColorBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

 
    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR)
    {
        fprintf(
            stderr,
            "ERROR: Could not create a VBO: %s \n",
            gluErrorString(ErrorCheckValue)
        );
 
        exit(EXIT_FAILURE);
    }
 }

void DestroyVBO(void)
{
    GLenum ErrorCheckValue; 
	
	ErrorCheckValue = glGetError();
 
	glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
         
    glBindBuffer(GL_ARRAY_BUFFER, 0);
 
    glDeleteBuffers(1, &ColorBufferID);
    glDeleteBuffers(1, &VertexBufferID);
 
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VertexArrayID);
 
    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR)
    {
        fprintf(
            stderr,
            "ERROR: Could not destroy the VBO: %s \n",
            gluErrorString(ErrorCheckValue)
        );
 
        //exit(EXIT_FAILURE);
    }
}

void CreateShaders(void)
{
    GLenum ErrorCheckValue = glGetError();

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
 
    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR)
    {
        fprintf(
            stderr,
            "ERROR: Could not create the shaders: %s \n",
            gluErrorString(ErrorCheckValue)
        );
 
        exit(EXIT_FAILURE);
    }
}

void DestroyShaders(void)
{
    GLenum ErrorCheckValue = glGetError();
 
    glUseProgram(0); //Invoke current program
 
    glDetachShader(ProgramID, VertexShaderId);
    glDetachShader(ProgramID, FragmentShaderId);
 
    glDeleteShader(FragmentShaderId);
    glDeleteShader(VertexShaderId);
 
    glDeleteProgram(ProgramID);
 
    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR)
    {
        fprintf(
            stderr,
            "ERROR: Could not destroy the shaders: %s \n",
            gluErrorString(ErrorCheckValue)
        );
 
        //exit(EXIT_FAILURE);
    }
}

void Cleanup(void)
{
	DestroyVBO();
    DestroyShaders();
}

bool sPoint2Array(sPoint const* sPtIn, unsigned n, GLfloat **gfOut)
{
	*gfOut = new GLfloat[8*n]; //Both the projected and the E^2 points
	for (unsigned i=0; i<n; i++)
	{
		(*gfOut)[0+4*i] = (GLfloat) sPtIn[i].x;
		(*gfOut)[1+4*i] = (GLfloat) sPtIn[i].y;
		(*gfOut)[2+4*i] = (GLfloat) sPtIn[i].w;
		(*gfOut)[3+4*i] = 1.0f;
		(*gfOut)[4*(n+i)+0] = (GLfloat) sPtIn[i].x;
		(*gfOut)[4*(n+i)+1] = (GLfloat) sPtIn[i].y;
		(*gfOut)[4*(n+i)+2] = 0.0f;
		(*gfOut)[4*(n+i)+3] = 1.0f;
	}
	return true;
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
        
		//calculate the tangential vector {remember, the radial vector is (x, y)}
		//to get the tangential vector we flip those coordinates and negate one of them 
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
	glewExperimental = GL_TRUE;
	glewInit(); // Don't ask. GLEW_3.3 is on the phase "do this, it's magic"

	//CreateVBO();

	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	CreateShaders();

	glfwSetMousePos(CurrentWidth/2, CurrentHeight/2); // Start by butting the cursor in the middle of the window (be carefull if you change the size of the window at runtime)

	// Generate the set of points
	sPoint *points = NULL;
	unsigned N;
	generatePoints(&points,N);
	printf("\nGenerated %d points in R^2\n",N);

	// Calculate their CenterPoint
	sPoint pntCenter;
	getCenterPoint(points,N,pntCenter);
	printf("CenterPoint = {%f; %f; %f}\n",pntCenter.x,pntCenter.y,pntCenter.w);

	// Calculate the partition
	double dRadius;
	sPoint sPartCenter;
	getPartition(&pntCenter,&dRadius,&sPartCenter);
	printf("Partition center = {%f; %f; %f}\n",sPartCenter.x,sPartCenter.y,sPartCenter.w);
	printf("Partition Radius = %f\n",dRadius);

	// Sample the partition to we can show it on the screen
	GLfloat *gfPartition = NULL;
	DrawCircle(sPartCenter.x,sPartCenter.y,dRadius,40,&gfPartition);
	
	// Arrange the points into an array (in homogeneous coordinates)
	static GLfloat *gfVertices = NULL;
	sPoint2Array(points, N, &gfVertices);
	delete [] points; points = NULL; //Dont forget to deallocate if you don't need them anymore

	// Colour the points on the paraboloid in yellow and the ones in the plane with magenta (sort-of)
	static GLfloat *gfColours = new GLfloat[8*N];
	for (unsigned i=0; i<N; i++)
	{
		gfColours[0+4*i] = 1.0f; //R
		gfColours[1+4*i] = 1.0f; //G
		gfColours[2+4*i] = 0.1f; //B
		gfColours[3+4*i] = 1.0f; //Alpha-chanel
		gfColours[4*(N+i)+0] = 1.0f; //R
		gfColours[4*(N+i)+1] = 0.1f; //G
		gfColours[4*(N+i)+2] = 0.4f; //B
		gfColours[4*(N+i)+3] = 1.0f; //Alpha-chanel
	}

	// Colour the points on the partition in white
	static GLfloat *gfPartitionColours = new GLfloat[4*40];
	for (unsigned i=0; i<40; i++)
	{
		gfPartitionColours[0+4*i] = 1.0f; //R
		gfPartitionColours[1+4*i] = 1.0f; //G
		gfPartitionColours[2+4*i] = 1.0f; //B
		gfPartitionColours[3+4*i] = 1.0f; //Alpha-chanel
	}

	glGenBuffers(1, &VertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, (8*N+4*40)*sizeof(gfVertices[0]), &gfVertices[0], GL_STATIC_DRAW); //Transfer to GPU memory (this is the point set)
	glBufferSubData(GL_ARRAY_BUFFER,8*N*sizeof(gfVertices[0]),4*40*sizeof(gfPartition[0]),&gfPartition[0]); //Add the partition points to the display buffer
	
	glGenBuffers(1,&ColorBufferID);
	glBindBuffer(GL_ARRAY_BUFFER,ColorBufferID);
	glBufferData(GL_ARRAY_BUFFER, (8*N+4*40)*sizeof(gfColours[0]), &gfColours[0], GL_STATIC_DRAW); //Transfer to GPU memory (this colours both the point sets)
	glBufferSubData(GL_ARRAY_BUFFER,8*N*sizeof(gfColours[0]),4*40*sizeof(gfPartitionColours[0]),&gfPartitionColours[0]); //Add the partition points to the display buffer

	do{
		if (do_redraw)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen
			glUseProgram(ProgramID); // Use our shader

			// Recalculate scene (using MVP matrix)
			drawNow();

			// 1-st attribute buffer : vertices
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glEnableVertexAttribArray(0);

			// 2-nd attribute buffer : colour
			glBindBuffer(GL_ARRAY_BUFFER, ColorBufferID);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glEnableVertexAttribArray(1);

			// Draw the whole thing
			glDrawArrays(GL_POINTS, 0, N); //Draw points on the paraboloid
			glDrawArrays(GL_POINTS, N, N); //Draw points in the plane
			glDrawArrays(GL_LINE_STRIP, 2*N, 40); //Draw the partition

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
		//glfwPollEvents();
	} 
	while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
		   glfwGetWindowParam( GLFW_OPENED ) ); // Check if the ESC key was pressed or the window was closed

	// Destroy VBO and Shaders
	Cleanup();

	delete [] gfVertices; gfVertices = NULL;
	delete [] gfColours; gfColours = NULL;
	delete [] gfPartition; gfPartition = NULL;
	delete [] gfPartitionColours; gfPartitionColours = NULL;

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	glfwCloseWindow();
	// Exit program
	exit( EXIT_SUCCESS );
}

