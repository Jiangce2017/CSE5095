

#include "GO.h"

#define WINDOW_TITLE_PREFIX "Geometric Separators"

int CurrentWidth = 800,
    CurrentHeight = 600,
    WindowHandle = 0;

static GLuint ProgramID, //ID of our shader
			  MatrixID, //ID for view matrix transformations3
			  VertexShaderId,
			  FragmentShaderId,
			  VertexArrayID,
			  VertexBufferID,
			  ColorBufferID;

// GLSL vertex shader
const GLchar* VertexShader =
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
const GLchar* FragmentShader =
{
    "#version 330 core\n"\
 
    "in vec3 fragmentColor;\n"\
    "out vec3 color;\n"\
 
    "void main(void)\n"\
    "{\n"\
    "   color = fragmentColor;\n"\
    "}\n"
};

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

	// Ensure we can capture the escape key being pressed below
	glfwEnable( GLFW_STICKY_KEYS );
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f); // Dark blue background
	glEnable(GL_DEPTH_TEST); // Enable depth test
	glDepthFunc(GL_LESS); // Accept fragment if it closer to the camera than the former one
	glEnable(GL_CULL_FACE); // Cull triangles which normal is not towards the camera
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
 
        exit(EXIT_FAILURE);
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
 
        exit(EXIT_FAILURE);
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
	glewInit(); // Don't ask. GLEW_3.3 is still on the phase "do this, it's magic"

	//CreateVBO();

	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	CreateShaders();

	glfwSetMousePos(1024/2, 768/2); // Start by butting the cursor in the middle of the window (be carefull if you change the size of the window at runtime)

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

	// Init the projections, view and model matrices which will form the Model-View-Projection (MVP) matrix
	glm::mat4 ProjectionMatrix, ViewMatrix, ModelMatrix, MVP;

	do{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen
		glUseProgram(ProgramID); // Use our shader

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		ProjectionMatrix = getProjectionMatrix();
		ViewMatrix = getViewMatrix();
		ModelMatrix = glm::mat4(1.0);
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix; // Remember, matrix multiplication is the other way around
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]); // Send our transformation to the currently bound shader, in the "MVP" uniform

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

		// Swap buffers
		glfwSwapBuffers();

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
	// Exit program
	exit( EXIT_SUCCESS );
}

