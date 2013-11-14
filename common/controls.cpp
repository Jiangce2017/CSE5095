///////////////////////////
////// controls.cpp ///////
///////////////////////////

// This is a mouse+keyboard input translator for OpenGL.
// Originially wrote by http://www.opengl-tutorial.org/ with permission to modify original code.


// Include GLFW
#include <glfw.h>

// Include GLM
#include <glm.hpp>
#include <./gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}

glm::vec3 position = glm::vec3( 0.138f, 4.021f, 1.600f ); // Initial position : on +Z
//float grd2rad = 3.14f/180;
float horizontalAngle = 6.29f; // Initial horizontal angle : toward -Z
float verticalAngle = -1.76f; // Initial vertical angle : none
float initialFoV = 75.0f; // Initial Field of View
float keySpeed = 3.0f; // 3 units/second
float mouseSpeed = 0.005f;


void computeMatricesFromInputs(){

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	int xpos, ypos;
	if (glfwGetMouseButton( GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
		glfwGetMousePos(&xpos, &ypos);
		// Reset mouse position for next frame
		glfwSetMousePos(1024/2, 768/2);
	}
	else{
		xpos = 1024/2;
		ypos = 768/2;
	}

	// Move in the scene like in COD - Black Ops. (comment-out the above IF\ELSE)
	//glfwGetMousePos(&xpos, &ypos);
	//// Reset mouse position for next frame
	//glfwSetMousePos(1024/2, 768/2);
	
	// Compute new orientation
	horizontalAngle += mouseSpeed * float(1024/2 - xpos );
	verticalAngle   += mouseSpeed * float( 768/2 - ypos );

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle), 
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
	
	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f), 
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);
	
	// Up vector
	glm::vec3 up = glm::cross( right, direction );

	// Move forward
	if (glfwGetKey( 'W' ) == GLFW_PRESS){ //GLFW_KEY_UP
		position += direction * deltaTime * keySpeed;
	}
	// Move backward
	if (glfwGetKey( 'S' ) == GLFW_PRESS){ //GLFW_KEY_DOWN
		position -= direction * deltaTime * keySpeed;
	}
	// Strafe right
	if (glfwGetKey( 'D' ) == GLFW_PRESS){ //GLFW_KEY_RIGHT
		position += right * deltaTime * keySpeed;
	}
	// Strafe left
	if (glfwGetKey( 'A' ) == GLFW_PRESS){ //GLFW_KEY_LEFT
		position -= right * deltaTime * keySpeed;
	}

	float FoV = initialFoV - 5 * glfwGetMouseWheel();

	// Projection matrix : Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix       = glm::lookAt(
								position,           // Camera is here
								position+direction, // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
	//printf("horizontalAngle = %f\n",horizontalAngle);
	//printf("verticalAngle = %f\n",verticalAngle);
	//printf("FoV = %f\n",FoV);
	//printf("position[%f\t%f\t%f]\n",position.x,position.y,position.z);
}