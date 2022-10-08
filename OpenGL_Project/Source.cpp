
// Library Defines
#define STB_IMAGE_IMPLEMENTATION

// Library Includes
#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <stb_image.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <fmod.hpp>

// Local Includes
#include "ShaderLoader.h"
#include "Utility.h"
#include "TextLabel.h"

// Function prototyping
bool AudioInit();
void InitialSetup();
void Update();
void ProcessInput(float DeltaTime);
void Render();
void ShutDown();
GLuint CreateTexture(std::string FilePath);

// Function callbacks for input
void KeyInput(GLFWwindow* InputWindow, int Key, int ScanCode, int Action, int Mods);
void TextInput(GLFWwindow* InputWindow, unsigned int CodePoint);
void MouseButtonInput(GLFWwindow* InputWindow, int Button, int Action, int Mods);
void CursorPositionInput(GLFWwindow* InputWindow, double PosX, double PosY);

// Inputs
bool Condition = false;

// Variables
GLFWwindow* Window = nullptr;
float CurrentTime;
float TimeElapsed = 0.0f;

// Audio System Variables
FMOD::System* AudioSystem;
FMOD::Sound* FX_Gunshot;
FMOD::Sound* Track_Dance;

// Delta Time
float PreviousTimeStep;

// Programs
GLuint Program_FixedTri;
GLuint Program_PositionOnly;
GLuint Program_Color;
GLuint Program_ColorFade;
GLuint Program_Texture;
GLuint Program_TextureMix;
GLuint Program_WorldSpace;
GLuint Program_ClipSpace;
GLuint Program_ClipSpace3D;
GLuint Program_Depth;

// Graphics IDs
GLuint VAO_Quad;
GLuint VBO_Quad;
GLuint EBO_Quad;
GLuint VAO_Pyramid;
GLuint VBO_Pyramid;
GLuint EBO_Pyramid;
GLuint VAO_Cube;
GLuint VBO_Cube;
GLuint EBO_Cube;

// Texture IDs
GLuint Texture_Rayman;
GLuint Texture_AwesomeFace;

// Object Matrices and Components
glm::vec3 ObjPosition = glm::vec3(0.0f, 0.0f, 0.0f);
float ObjRotationAngle = 0.0f;
glm::vec3 ObjScale = glm::vec3(1.0f, 1.0f, 1.0f);

glm::mat4 TranslationMat;
glm::mat4 RotationMat;
glm::mat4 ScaleMat;
glm::mat4 ObjModelMat;

// Camera Variables
glm::vec3 CameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 CameraLookDir = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 CameraTargetPos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 CameraUpDir = glm::vec3(0.0f, 1.0f, 0.0f);
glm::mat4 ViewMat;
glm::mat4 ProjectionMat;

// Text Variables
TextLabel* Text_Message;

// Vertices / Indices
GLfloat Vertices_Quad[] = {
	// Index	// Position				// Color			// Texture Coords
	/* 1 */		-0.5f,  0.5f, 0.0f,		1.0f, 0.0f, 0.0f,	0.0f, 1.0f,		// Top - Left
	/* 2 */		-0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,	0.0f, 0.0f, 	// Bot - Left
	/* 3 */		 0.5f, -0.5f, 0.0f,		1.0f, 0.0f, 1.0f,	1.0f, 0.0f,		// Bot - Right
	/* 4 */		 0.5f,  0.5f, 0.0f,		0.0f, 1.0f, 1.0f,	1.0f, 1.0f,		// Top - Right
};

GLuint Indices_Quad[] = {
	0, 1, 2,	// First Triangle	(TL -> BL -> BR)
	0, 2, 3,	// Second Triangle	(TL -> BR -> TR)
};

//GLfloat Vertices_Pyramid[] = {
//	// Index	// Position				// Tex Coords
//	/* 00 */	-0.5f,  0.0f, -0.5f,	0.0f, 1.0f,
//	/* 01 */	-0.5f,  0.0f,  0.5f,	0.0f, 0.0f,
//	/* 02 */	 0.5f,  0.0f,  0.5f,	1.0f, 0.0f,
//	/* 03 */	 0.5f,  0.0f, -0.5f,	1.0f, 1.0f,
//
//	/* 04 */	 0.0f,  0.5f,  0.0f,	0.5f, 0.5f,	// Top Point
//};
//
//GLuint Indices_Pyramid[] = {
//	1, 0 ,3,	// Bottom (Quad) Tri 1
//	1, 3, 2,	// Bottom (Quad) Tri 2
//
//	0, 4, 3,	// Side 1
//	3, 4, 2,	// Side 2
//	2, 4, 1,	// Side 3
//	1, 4, 0,	// Side 4
//};

GLfloat Vertices_Pyramid[] = {
	// Index	// Position				// Tex Coords	// Position Index
				// Bottom Quad
	/* 00 */	-0.5f,  0.0f, -0.5f,	0.0f, 1.0f,		/* 00 */
	/* 01 */	-0.5f,  0.0f,  0.5f,	0.0f, 0.0f,		/* 01 */
	/* 02 */	 0.5f,  0.0f,  0.5f,	1.0f, 0.0f,		/* 02 */
	/* 03 */	 0.5f,  0.0f, -0.5f,	1.0f, 1.0f,		/* 03 */
	// Side 1										
	/* 04 */	-0.5f,  0.0f, -0.5f,	0.0f, 0.0f,		/* 00 */
	/* 05 */	-0.5f,  0.0f,  0.5f,	1.0f, 0.0f,		/* 01 */
	// Side 2										
	/* 06 */	-0.5f,  0.0f,  0.5f,	0.0f, 0.0f,		/* 01 */
	/* 07 */	 0.5f,  0.0f,  0.5f,	1.0f, 0.0f,		/* 02 */
	// Side 3										
	/* 08 */	 0.5f,  0.0f,  0.5f,	0.0f, 0.0f,		/* 02 */
	/* 09 */	 0.5f,  0.0f, -0.5f,	1.0f, 0.0f,		/* 03 */
	// Side 4										
	/* 10 */	 0.5f,  0.0f, -0.5f,	0.0f, 0.0f,		/* 03 */
	/* 11 */	-0.5f,  0.0f, -0.5f,	1.0f, 0.0f,		/* 00 */
	// Top Point									   
	/* 12 */	 0.0f,  0.5f,  0.0f,	0.5f, 1.0f,		/* 04 */
};

GLuint Indices_Pyramid[] = {
	 1,  0,  3,	// Bottom (Quad) Tri 1
	 1,  3,  2,	// Bottom (Quad) Tri 2

	 4,  5, 12,	// Side 1
	 6,  7, 12,	// Side 2
	 8,  9, 12,	// Side 3
	10, 11, 12,	// Side 4
};

GLfloat Vertices_Cube[] = {
	// Index	// Position				// Tex Coords	// Position Index
				// Front Quad
	/* 00 */	-0.5f,  0.5f,  0.5f,	0.0f, 1.0f,		/* 00 */
	/* 01 */	-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,		/* 01 */
	/* 02 */	 0.5f, -0.5f,  0.5f,	1.0f, 0.0f,		/* 02 */
	/* 03 */	 0.5f,  0.5f,  0.5f,	1.0f, 1.0f,		/* 03 */
	// Back Quad
	/* 04 */	 0.5f,  0.5f, -0.5f,	0.0f, 1.0f,		/* 04 */
	/* 05 */	 0.5f, -0.5f, -0.5f,	0.0f, 0.0f,		/* 05 */
	/* 06 */	-0.5f, -0.5f, -0.5f,	1.0f, 0.0f,		/* 06 */
	/* 07 */	-0.5f,  0.5f, -0.5f,	1.0f, 1.0f,		/* 07 */
	// Right
	/* 08 */	 0.5f,  0.5f,  0.5f,	0.0f, 1.0f,		/* 03 */
	/* 09 */	 0.5f, -0.5f,  0.5f,	0.0f, 0.0f,		/* 02 */
	/* 10 */	 0.5f, -0.5f, -0.5f,	1.0f, 0.0f,		/* 05 */
	/* 11 */	 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,		/* 04 */
	// Left
	/* 12 */	-0.5f,  0.5f, -0.5f,	0.0f, 1.0f,		/* 07 */
	/* 13 */	-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,		/* 06 */
	/* 14 */	-0.5f, -0.5f,  0.5f,	1.0f, 0.0f,		/* 01 */
	/* 15 */	-0.5f,  0.5f,  0.5f,	1.0f, 1.0f,		/* 00 */
	// Top
	/* 16 */	-0.5f,  0.5f, -0.5f,	0.0f, 1.0f,		/* 07 */
	/* 17 */	-0.5f,  0.5f,  0.5f,	0.0f, 0.0f,		/* 00 */
	/* 18 */	 0.5f,  0.5f,  0.5f,	1.0f, 0.0f,		/* 03 */
	/* 19 */	 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,		/* 04 */
	// Bottom
	/* 20 */	-0.5f, -0.5f,  0.5f,	0.0f, 1.0f,		/* 01 */
	/* 21 */	-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,		/* 06 */
	/* 22 */	 0.5f, -0.5f, -0.5f,	1.0f, 0.0f,		/* 05 */
	/* 23 */	 0.5f, -0.5f,  0.5f,	1.0f, 1.0f,		/* 02 */
};

GLuint Indices_Cube[] = {
	 0,  1,  2,	// Front Tri 1
	 0,  2,  3,	// Front Tri 2		 	 	 
	 4,  5,  6,	// Back Tri 1
	 4,  6,  7,	// Back Tri 2		 	 
	 8,  9, 10,	// Right Tri 1
	 8, 10, 11,	// Right Tri 2
	12, 13, 14,	// Left Tri 1
	12, 14, 15,	// Left Tri 2
	16, 17, 18,	// Top Tri 1
	16, 18, 19,	// Top Tri 2
	20, 21, 22,	// Bottom Tri 1
	20, 22, 23,	// Bottom Tri 2
};

int main()
{
	// Initializing GLFW and setting the version to 4.6 with only Core functionality available
	glfwInit();
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	// Create an GLFW controlled context window
	Window = glfwCreateWindow(Utils::WindowWidth, Utils::WindowHeight, "First OpenGL Window", NULL, NULL);
	if (Window == NULL)
	{
		std::cout << "GLFW failed to initialize properly. Terminating program." << std::endl;
		system("pause");

		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(Window);

	// Initializing GLEW to populate OpenGL function pointers
	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW failed to initialize properly. Terminating program." << std::endl;
		system("pause");

		glfwTerminate();
		return -1;
	}

	// Setup the Initial elements of the program
	InitialSetup();

	// Main loop
	while (glfwWindowShouldClose(Window) == false)
	{
		// Update all objects and run the processes
		Update();

		// Render all the objects
		Render();
	}

	// Release memory and ensure correct shutdown order of created objects
	ShutDown();

	// Ensuring correct shutdown of GLFW
	glfwTerminate();
	return 0;
}

// Initialize the Audio System for FMOD
bool AudioInit()
{
	if (FMOD::System_Create(&AudioSystem) != FMOD_OK)
	{
		std::cout << "FMOD ERROR: Audio System failed to create." << std::endl;
		return false;
	}
	if (AudioSystem->init(100, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, 0) != FMOD_OK)
	{
		std::cout << "FMOD ERROR: Audio System failed to initialize." << std::endl;
		return false;
	}

	return true;
}

// Setup the Initial elements of the program
void InitialSetup()
{
	// Set the color of the window for when the buffer is cleared
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

	// Maps the range of the window size to NDC (-1 -> 1)
	glViewport(0, 0, Utils::WindowWidth, Utils::WindowHeight);

	// Enable Culling for 3D efficiency
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// Enable Depth testing for 3D
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Create the programs 
	Program_FixedTri = ShaderLoader::CreateProgram("Resources/Shaders/FixedTriangle.vs",
		"Resources/Shaders/FixedColor.fs");
	Program_PositionOnly = ShaderLoader::CreateProgram("Resources/Shaders/PositionOnly.vs",
		"Resources/Shaders/FixedColor.fs");
	Program_Color = ShaderLoader::CreateProgram("Resources/Shaders/VertexColor.vs",
		"Resources/Shaders/VertexColor.fs");
	Program_ColorFade = ShaderLoader::CreateProgram("Resources/Shaders/VertexColor.vs",
		"Resources/Shaders/VertexColorFade.fs");
	Program_Texture = ShaderLoader::CreateProgram("Resources/Shaders/NDC_Texture.vs",
		"Resources/Shaders/Texture.fs");
	Program_TextureMix = ShaderLoader::CreateProgram("Resources/Shaders/NDC_Texture.vs",
		"Resources/Shaders/TextureMix.fs");
	Program_WorldSpace = ShaderLoader::CreateProgram("Resources/Shaders/WorldSpace.vs",
		"Resources/Shaders/Texture.fs");
	Program_ClipSpace = ShaderLoader::CreateProgram("Resources/Shaders/ClipSpace.vs",
		"Resources/Shaders/Texture.fs");
	Program_ClipSpace3D = ShaderLoader::CreateProgram("Resources/Shaders/3D_ClipSpace.vs",
		"Resources/Shaders/3D_Texture.fs");
	Program_Depth = ShaderLoader::CreateProgram("Resources/Shaders/3D_ClipSpace.vs",
		"Resources/Shaders/3D_Depth.fs");

	// Generate the VAO for a Quad
	glGenVertexArrays(1, &VAO_Quad);
	glBindVertexArray(VAO_Quad);

	// Generate the EBO for a Quad
	glGenBuffers(1, &EBO_Quad);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Quad);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices_Quad), Indices_Quad, GL_STATIC_DRAW);

	// Generate the VBO for a Quad
	glGenBuffers(1, &VBO_Quad);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Quad);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices_Quad), Vertices_Quad, GL_STATIC_DRAW);

	// Set the Vertex Attibute information (how to interpret the vertex data)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);


#pragma region Pyramid Mesh
	// Generate the VAO for a 3D Pyramid Object
	glGenVertexArrays(1, &VAO_Cube);
	glBindVertexArray(VAO_Cube);

	// Generate the EBO for a 3D Pyramid Object
	glGenBuffers(1, &EBO_Cube);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Cube);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices_Cube), Indices_Cube, GL_STATIC_DRAW);

	// Generate the VBO for a 3D Pyramid Object
	glGenBuffers(1, &VBO_Cube);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices_Cube), Vertices_Cube, GL_STATIC_DRAW);

	// Set the Vertex Attibute information (how to interpret the vertex data)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
#pragma endregion



	// Load and create the textures
	Texture_Rayman = CreateTexture("unknown.png");
	Texture_AwesomeFace = CreateTexture("unknown.png");

	// Initialize the first value of the "previous" time step
	PreviousTimeStep = (float)glfwGetTime();

	// Set the callback functions for input processing
	glfwSetKeyCallback(Window, KeyInput);
	glfwSetCharCallback(Window, TextInput);
	glfwSetMouseButtonCallback(Window, MouseButtonInput);
	glfwSetCursorPosCallback(Window, CursorPositionInput);
	// Keep the default cursor visibility
	glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	if (AudioInit() == true)
	{
		// Loading sounds using FMOD
		if (AudioSystem->createSound("Resources/Audio/Gunshot.wav", FMOD_DEFAULT, 0, &FX_Gunshot) != FMOD_OK)
		{
			std::cout << "FMOD ERROR: Failed to load sound using createSound(...)" << std::endl;
		}
		if (AudioSystem->createSound("Resources/Audio/DanceTrack.mp3", FMOD_LOOP_NORMAL, 0, &Track_Dance) != FMOD_OK)
		{
			std::cout << "FMOD ERROR: Failed to load sound using createSound(...)" << std::endl;
		}

		//AudioSystem->playSound(Track_Dance, 0, false, 0);
	}

	// Create Text objects
	Text_Message = new TextLabel("Orbital camera around a very cute cube.", "Resources/Fonts/ARIAL.ttf", glm::ivec2(0, 30), glm::vec2(20.0f, 20.0f));
}



// Update all objects and run the processes
void Update()
{
	// Update the FMOD Audio System
	AudioSystem->update();

	// Calculate the current DeltaTime and update the PreviousTimeStep for the next frame
	float CurrentTimeStep = (float)glfwGetTime();
	float DeltaTime = CurrentTimeStep - PreviousTimeStep;
	PreviousTimeStep = CurrentTimeStep;

	glfwPollEvents();
	ProcessInput(DeltaTime);

	// Calculate the Translation matrix
	TranslationMat = glm::translate(glm::mat4(), ObjPosition);

	// Calculate the Rotation matrix
	RotationMat = glm::rotate(glm::mat4(), glm::radians(ObjRotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

	// Calculate the Scale matrix
	ScaleMat = glm::scale(glm::mat4(), ObjScale);

	// Create the Model matrix
	ObjModelMat = TranslationMat * RotationMat * ScaleMat;

	// Calculate the View matrix from the camera variables
	//ViewMat = glm::lookAt(CameraPos, CameraPos + CameraLookDir, CameraUpDir);
	//ViewMat = glm::lookAt(CameraPos, CameraTargetPos, CameraUpDir);

	// Calculate the Projection matrix - Anchor point (0, 0) at the top left
	//ProjectionMat = glm::ortho(0.0f, (float)WindowWidth, (float)WindowHeight, 0.0f, 0.1f, 100.0f);

	// Calculate the Projection matrix - Anchor point (0, 0) at the center
	//float HalfWindowWidth = (float)WindowWidth * 0.5f;
	//float HalfWindowHeight = (float)WindowHeight * 0.5f;
	//ProjectionMat = glm::ortho(-HalfWindowWidth, HalfWindowWidth, -HalfWindowHeight, HalfWindowHeight, 0.1f, 100.0f);

	//Calculate the Persepective Projection matrix
	ProjectionMat = glm::perspective(glm::radians(45.0f), (float)Utils::WindowWidth / (float)Utils::WindowHeight, 0.1f, 100.0f);

	TimeElapsed += DeltaTime;
	float CameraOrbitRadius = 3.0f;
	float CameraOrbitHeight = 2.0f;
	CameraPos.x = sin(TimeElapsed) * CameraOrbitRadius;
	CameraPos.y = CameraOrbitHeight;
	CameraPos.z = cos(TimeElapsed) * CameraOrbitRadius;

	ViewMat = glm::lookAt(CameraPos, glm::vec3(0.0f, 0.0f, 0.0f), CameraUpDir);
}

// Query GLFW input states
void ProcessInput(float DeltaTime)
{
	double Xpos;
	double Ypos;
	glfwGetCursorPos(Window, &Xpos, &Ypos);
	if (Xpos > ((double)Utils::WindowWidth * 0.5) && Ypos > ((double)Utils::WindowHeight * 0.5))
	{
		//std::cout << "Cursor is in lower right quadrant" << std::endl;
	}

	if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		//std::cout << "Mouse button left: Press" << std::endl;
	}

	if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
	{
		ObjPosition += glm::vec3(0.0f, 1.0f, 0.0f) * DeltaTime;
	}
	if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
	{
		ObjPosition += glm::vec3(0.0f, -1.0f, 0.0f) * DeltaTime;
	}
	if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
	{
		ObjPosition += glm::vec3(-1.0f, 0.0f, 0.0f) * DeltaTime;
	}
	if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
	{
		ObjPosition += glm::vec3(1.0f, 0.0f, 0.0f) * DeltaTime;
	}
}

// Render all the objects
void Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind the Program and VAO
	glUseProgram(Program_ClipSpace3D);
	glBindVertexArray(VAO_Cube);

	// Send variables to the shaders via Uniform
	GLint ProjectionMatLoc = glGetUniformLocation(Program_ClipSpace3D, "ProjectionMat");
	glUniformMatrix4fv(ProjectionMatLoc, 1, GL_FALSE, glm::value_ptr(ProjectionMat));
	GLint ViewMatLoc = glGetUniformLocation(Program_ClipSpace3D, "ViewMat");
	glUniformMatrix4fv(ViewMatLoc, 1, GL_FALSE, glm::value_ptr(ViewMat));
	GLint ModelMatLoc = glGetUniformLocation(Program_ClipSpace3D, "ModelMat");
	glUniformMatrix4fv(ModelMatLoc, 1, GL_FALSE, glm::value_ptr(ObjModelMat));

	// Activate and Bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture_AwesomeFace);
	glUniform1i(glGetUniformLocation(Program_ClipSpace3D, "ImageTexture"), 0);

	// Render the Quad
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	// Unbind assets to prevent accidental use/modification
	glBindVertexArray(0);
	glUseProgram(0);

	// Render the Text objects last - Keep them on top (Orthographic)
	Text_Message->Render();

	glfwSwapBuffers(Window);
}

// Properly close and shut down all active objects.
void ShutDown()
{
	FX_Gunshot->release();
	Track_Dance->release();
	AudioSystem->release();
}

GLuint CreateTexture(std::string FileName)
{
	GLuint TextureID;
	std::string FullFilePath = "Resources/Textures/";
	FullFilePath.append(FileName);

	// Load the Image data
	int ImageWidth;
	int ImageHeight;
	int ImageComponents;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* ImageData = stbi_load(FullFilePath.c_str(),
		&ImageWidth, &ImageHeight, &ImageComponents, 0);

	// Create and bind a new texture template
	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);

	// Check how many components the loaded image has (RGB or RGBA?)
	GLint LoadedComponents = (ImageComponents == 4) ? GL_RGBA : GL_RGB;

	// Populate the texture with the image data
	glTexImage2D(GL_TEXTURE_2D, 0, LoadedComponents, ImageWidth, ImageHeight, 0,
		LoadedComponents, GL_UNSIGNED_BYTE, ImageData);

	// Setting the address mode for this texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Generate the mipmaps, free the memory and unbind the texture
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(ImageData);
	glBindTexture(GL_TEXTURE_2D, 0);

	return TextureID;
}

// Callback function called in response to keyboard input. Processed during glfwPollEvents().
void KeyInput(GLFWwindow* InputWindow, int Key, int ScanCode, int Action, int Mods)
{
	if (Key == GLFW_KEY_ESCAPE && Action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(Window, true);
	}

	if (Key == GLFW_KEY_ENTER && Action == GLFW_PRESS)
	{
		Condition = !Condition;
		(Condition == true) ? glfwSetCharCallback(Window, TextInput) : glfwSetCharCallback(Window, 0);
	}
}

// Callback function called in response to keyboard input for text printing. Processed during glfwPollEvents().
void TextInput(GLFWwindow* InputWindow, unsigned int CodePoint)
{
	//std::cout << "Text input detected: " << (unsigned char)CodePoint << std::endl;
}

// Callback function called in response to mouse button input. Processed during glfwPollEvents().
void MouseButtonInput(GLFWwindow* InputWindow, int Button, int Action, int Mods)
{
	if (Button == GLFW_MOUSE_BUTTON_LEFT && Action == GLFW_PRESS)
	{
		AudioSystem->playSound(FX_Gunshot, 0, false, 0);
	}
	if (Button == GLFW_MOUSE_BUTTON_LEFT && Action == GLFW_RELEASE)
	{

	}

	if (Button == GLFW_MOUSE_BUTTON_RIGHT && Action == GLFW_PRESS)
	{
		//Text_Message->SetText("Changing the text is easy :)");
		//Text_Message->SetPosition(glm::vec2(100.0f, 650.0f));
		//Text_Message->SetColor(glm::vec3(1.0f, 1.0f, 0.0f));
		//Text_Message->SetScale(glm::vec2(0.5f, 0.5f));
	}
	if (Button == GLFW_MOUSE_BUTTON_RIGHT && Action == GLFW_RELEASE)
	{
		//std::cout << "Mouse Button Right: Release" << std::endl;
	}
}

// Callback function called in response to cursor movement within the bounds of the window context. Processed during glfwPollEvents().
void CursorPositionInput(GLFWwindow* InputWindow, double PosX, double PosY)
{
	//std::cout << "Cursor Coords:  X: " << PosX << "   Y: " << PosY<< std::endl;

}