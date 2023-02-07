// Windows includes (For Time, IO, etc.)
#define NOMINMAX
#include <limits>
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.
#include "stb_image.h"
#include "Shader.h"

#include "Utils.h"
// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>

//#include "Model.h"
// Project includes
//#include "maths_funcs.h"
//#include <glm/glm/glm.hpp>
//#include <glm/glm/gtc/matrix_transform.hpp>
//#include <glm/glm/gtc/type_ptr.hpp>
#include "maths_funcs.h"
#include "PaperBoy.h"
#include "Camera.h"
#include "Newspaper.h"

Camera camera;
Camera isometricCamera;
int width = 1400;
int height = 700;
bool firstMouse = true;
GLfloat seal_forward = 0.0f;
GLfloat seal_right = 0.0f;
float delta;
float lastx = width / 2.0f;
float lasty = height / 2.0f;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
bool keyStates[256];
float skyR = 0.6f;
float skyG = 0.87f;
float skyB = 0.96f;
bool trans = false;
bool skyMap = false;
bool fog = false;
float spinning = 0.0f;

using namespace std;


Texture* pTexture = NULL;
Texture* sphereTexture = NULL;
Shader phongShader;
Shader toonShader;
Shader minnaertShader;
Shader shaderProgram;
Shader skyboxShader;
Shader skyShader;
Shader frenselShader;
Model terrain;
Model sphere;
PaperBoy paperboy;
mat4 gWVP;
Newspaper paper;
Model hedges;
Model teapot;
Newspaper *newspapers;


std::string parentDir = "D:\\Personal\\College\\5thYear\\ComputerGraphics\\";

float skyboxVertices[] =
{
	//   Coordinates
	-1.0f, -1.0f,  1.0f,//        7--------6
	 1.0f, -1.0f,  1.0f,//       /|       /|
	 1.0f, -1.0f, -1.0f,//      4--------5 |
	-1.0f, -1.0f, -1.0f,//      | |      | |
	-1.0f,  1.0f,  1.0f,//      | 3------|-2
	 1.0f,  1.0f,  1.0f,//      |/       |/
	 1.0f,  1.0f, -1.0f,//      0--------1
	-1.0f,  1.0f, -1.0f
};

unsigned int skyboxIndices[] =
{
	// Right
	1, 2, 6,
	6, 5, 1,
	// Left
	0, 4, 7,
	7, 3, 0,
	// Top
	4, 5, 6,
	6, 7, 4,
	// Bottom
	0, 3, 2,
	2, 1, 0,
	// Back
	0, 1, 5,
	5, 4, 0,
	// Front
	3, 7, 6,
	6, 2, 3
};
std::string facesCubemap[6] =
{
	parentDir + "Textures\\nx.png",
	parentDir + "Textures\\px.png",
	parentDir + "Textures\\py.png",
	parentDir + "Textures\\ny.png",
	parentDir + "Textures\\nz.png",
	parentDir + "Textures\\pz.png"
};

unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
unsigned int cubemapTexture;


void display() {
	// Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
	/*
	//Transparency
	if (trans == true) {
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else if (trans == false) {
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
	}
	*/

	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	// Enables Cull Facing
	glEnable(GL_CULL_FACE);
	// Keeps front faces
	glCullFace(GL_FRONT);
	// Uses counter clock-wise standard
	glFrontFace(GL_CCW);

	//glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor(skyR, skyG, skyB, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	phongShader.use();
	//get uniform location from shader
	int view_loc = glGetUniformLocation(phongShader.ID, "view");
	int proj_loc = glGetUniformLocation(phongShader.ID, "proj");
	int view_pos_loc = glGetUniformLocation(phongShader.ID, "viewPos");
	int fog_loc = glGetUniformLocation(phongShader.ID, "density");
	glUniform1f(fog_loc, 0.007);
	mat4 persp_proj = perspective(camera.Fov, (float)width / (float)height, 0.1f, 1000.0f);
	mat4 view = camera.GetViewMatrix();

	mat4 view1 = view;
	view1.m[12] = 0;
	view1.m[13] = 0;
	view1.m[14] = 0;
	view1.m[15] = 0;

	glDepthFunc(GL_LEQUAL);

	skyboxShader.use();
	glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, view1.m);
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, persp_proj.m);
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	

	glDepthFunc(GL_LESS);
	phongShader.use();
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_loc, 1, GL_FALSE, view.m);
	glUniform3f(view_pos_loc, camera.Pos.v[0], camera.Pos.v[1], camera.Pos.v[2]);
	mat4 teapot1_mat = identity_mat4();
	spinning = spinning + 0.3f;
	teapot1_mat = rotate_y_deg(teapot1_mat, spinning);
	teapot1_mat = rotate_x_deg(teapot1_mat, spinning);
	teapot1_mat = translate(teapot1_mat, vec3(0.0, 00.0, 3.0));
	teapot.RenderModel(teapot1_mat, phongShader.ID);


	
	mat4 terrain_mat = identity_mat4();
	//get uniform location from shader
	toonShader.use();
	int view_loc2 = glGetUniformLocation(toonShader.ID, "view");
	int proj_loc2 = glGetUniformLocation(toonShader.ID, "proj");
	int view_pos_loc2 = glGetUniformLocation(toonShader.ID, "viewPos");
	mat4 persp_proj2 = perspective(camera.Fov, (float)width / (float)height, 0.1f, 1000.0f);
	mat4 view2 = camera.GetViewMatrix();
	glUniformMatrix4fv(proj_loc2, 1, GL_FALSE, persp_proj2.m);
	glUniformMatrix4fv(view_loc2, 1, GL_FALSE, view2.m);
	glUniform3f(view_pos_loc2, camera.Pos.v[0], camera.Pos.v[1], camera.Pos.v[2]);
	mat4 teapot2_mat = identity_mat4();
	spinning = spinning + 0.3f;
	teapot2_mat = rotate_y_deg(teapot2_mat, spinning);
	teapot2_mat = rotate_x_deg(teapot2_mat, spinning);
	teapot2_mat = translate(teapot2_mat, vec3(10.0, 00.0, 3.0));
	teapot.RenderModel(teapot2_mat, toonShader.ID);
	
	minnaertShader.use();
	int view_loc3 = glGetUniformLocation(minnaertShader.ID, "view");
	int proj_loc3 = glGetUniformLocation(minnaertShader.ID, "proj");
	//int world_loc3 = glGetUniformLocation(minnaertShader.ID, "world");
	mat4 persp_proj3 = perspective(camera.Fov, (float)width / (float)height, 0.1f, 1000.0f);
	mat4 view3 = camera.GetViewMatrix();
	//glUniform1i(glGetUniformLocation(minnaertShader.ID, "Cubemap"), 0);
	glUniformMatrix4fv(proj_loc3, 1, GL_FALSE, persp_proj3.m);
	glUniformMatrix4fv(view_loc3, 1, GL_FALSE, view3.m);
	//glUniformMatrix4fv(world_loc3, 1, GL_FALSE, terrain_mat.m);
	mat4 teapot3_mat = identity_mat4();
	spinning = spinning + 0.3f;
	teapot3_mat = rotate_y_deg(teapot3_mat, spinning);
	teapot3_mat = rotate_x_deg(teapot3_mat, spinning);
	teapot3_mat = translate(teapot3_mat, vec3(-10.0, 0.0, 3.0));
	teapot.RenderModel(teapot3_mat, minnaertShader.ID);

	//mat4 paper_mat = identity_mat4();
	//for (int i = 0; i < paperboy.remainingPapers; i++) {
	//	newspapers[i].renderNewspaper(paper_mat, myShader.ID);
	//}
	//paperboy.renderPaperBoy(world, myShader.ID);

	

	skyShader.setFloat("density", 0.007);
	int fog_loc2 = glGetUniformLocation(skyShader.ID, "density");
	if (fog == true) {
		glUniform1f(fog_loc, 0.007);
		glUniform1f(fog_loc2, 0.007);
	}
	else {
		glUniform1f(fog_loc, 0.000);
		glUniform1f(fog_loc2, 0.000);
	}


	glutSwapBuffers();
}

void updateCamera() {
	if (camera.freeCam == true) {
		if (keyStates['w'] == true) { //move cam forward
			camera.ProcessKeyboard(FORWARD, delta);
		}
		if (keyStates['a'] == true) {//mpve cam left
			camera.ProcessKeyboard(LEFT, delta);
		}
		if (keyStates['d'] == true) {//move cam right
			camera.ProcessKeyboard(RIGHT, delta);
		}
		if (keyStates['s'] == true) {//move cam backward
			camera.ProcessKeyboard(BACKWARD, delta);
		}
		if (keyStates['q'] == true) {//move cam backward
			camera.ProcessKeyboard(DOWN, delta);
		}
		if (keyStates['e'] == true) {//move cam backward
			camera.ProcessKeyboard(UP, delta);
		}

	}
	else if (camera.thirdPerson == true) {
		camera.processThirdPerson(paperboy.Pos, paperboy.Front);
	}
	else if (camera.originalCamera == true)
		camera.processIso(paperboy.Pos, paperboy.Front);
}

void updatePaperboy() {
	if (keyStates['i'] == true) {
		paperboy.ProcessKeyboard(FORWARD, delta);
	}
	if (keyStates['k'] == true) {
		paperboy.ProcessKeyboard(BACKWARD, delta);
	}
	if (keyStates['l'] == true) {
		paperboy.ProcessKeyboard(RIGHT, delta);
	}
	if (keyStates['j'] == true) {
		paperboy.ProcessKeyboard(LEFT, delta);
	}
	if (keyStates['i'] == true) {
		paperboy.spinWheels(delta);
	}
}

void updateScene() {
	static DWORD last_time = 0;
	DWORD curr_time = timeGetTime();
	if (last_time == 0)
		last_time = curr_time;
	delta = (curr_time - last_time) * 0.001f;
	last_time = curr_time;

	updatePaperboy();
	updateCamera();
	glutPostRedisplay();
}

void init()
{
	// Set up the models and shaders
	phongShader = Shader("D:\\Personal\\College\\5thYear\\ComputerGraphics\\simpleVertexShader.txt","D:\\Personal\\College\\5thYear\\ComputerGraphics\\simpleFragmentShader.txt");
	toonShader = Shader("D:\\Personal\\College\\5thYear\\ComputerGraphics\\simpleVertexShader.txt", "D:\\Personal\\College\\5thYear\\ComputerGraphics\\simpleFragmentShaderToon.txt");
	minnaertShader = Shader("D:\\Personal\\College\\5thYear\\ComputerGraphics\\simpleVertexShader.txt", "D:\\Personal\\College\\5thYear\\ComputerGraphics\\simpleFragmentShaderMinnaert.txt");
	// Generates Shader objects
	//Shader shaderProgram("default.vert", "default.frag");
	skyboxShader = Shader("D:\\Personal\\College\\5thYear\\ComputerGraphics\\Lab04\\skyboxVert.txt", "D:\\Personal\\College\\5thYear\\ComputerGraphics\\Lab04\\skyboxFrag.txt");
	skyboxShader.use();
	glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);
	//frenselShader = Shader("D:\\Personal\\College\\5thYear\\ComputerGraphics\\Lab04\\frenselVertexShader.txt", "D:\\Personal\\College\\5thYear\\ComputerGraphics\\Lab04\\frenselFragmentShader.txt");
	skyShader = Shader("D:\\Personal\\College\\5thYear\\ComputerGraphics\\simpleVertexShader.txt", "D:\\Personal\\College\\5thYear\\ComputerGraphics\\skyShader.txt");
	paperboy = PaperBoy(vec3(0.5f, 0.1f, 2.0f));
	terrain = Model("D:\\Personal\\College\\5thYear\\ComputerGraphics\\Models\\Map.obj");
	sphere = Model("D:\\Personal\\College\\5thYear\\ComputerGraphics\\Models\\SphereScene.obj");
	hedges = Model("D:\\Personal\\College\\5thYear\\ComputerGraphics\\Models\\Hedges.obj");
	teapot = Model("D:\\Personal\\College\\5thYear\\ComputerGraphics\\Models\\teapot.obj");
	pTexture = new Texture(GL_TEXTURE_2D, "qG00I.png");
	sphereTexture = new Texture(GL_TEXTURE_2D, "sunflowers_2k.hdr");
	paper = Newspaper(vec3(0.3f, 0.3f, 0.0f));
	newspapers = new Newspaper[10];
	Newspaper* newspapers = (Newspaper*)malloc(sizeof(Newspaper) * 50);
	// calling constructor
	// for each index of array
	//for (int i = 0; i < 50; i++) {
	///	arr[i] = Newspaper();
		// All the faces of the cubemap (make sure they are in this exact order)



}

void mouseCallback(int x, int y) {

	if (firstMouse)
	{
		lastx = x;
		lasty = y;
		firstMouse = false;
	}

	static bool wrap = false;
	float dx = 0.0;
	float dy = 0.0;
	if (!wrap) {
		int ww = glutGet(GLUT_WINDOW_WIDTH);
		int wh = glutGet(GLUT_WINDOW_HEIGHT);

		dx = x - ww / 2;
		dy = y - wh / 2;

		// Do something with dx and dy here

		// move mouse pointer back to the center of the window
		wrap = true;
		glutWarpPointer(ww / 2, wh / 2);
	}
	else {
		wrap = false;
	}

	float sensitivity = 0.1f;
	dx *= sensitivity;
	dy *= sensitivity;

	yaw += dx;
	pitch -= dy;

	if (pitch > 89.0f)
		pitch = 89.0f;

	if (pitch < -89.0f)
		pitch = -89.0f;
	if (camera.thirdPerson == false) {
		camera.ProcessMouseMovement(dx, -dy, true);
	}
}

void keyDown(unsigned char key, int x, int y) {
	keyStates[key] = true;
	if (key == 'z') {
		cout << "z key pressed";
		camera.thirdPerson = true;
		camera.originalCamera = false;
		camera.freeCam = false;
	}
	if (key == 'x') {
		camera.thirdPerson = false;
		camera.originalCamera = true;
		camera.freeCam = false;
		fog = false;
	}
	if (key == 'c') {
		camera.thirdPerson = false;
		camera.originalCamera = false;
		camera.freeCam = true;
		fog = false;
	}
	if (key == 'u') {
		paperboy.throwPapers(paper, phongShader, paperboy.remainingPapers);
	}
	if (key == 'b') {//move cam backward
		skyMap = !skyMap;
	}
	if (key == 't') {//move cam backward
		trans = !trans;
	}
	if (key == 'f') {//move cam backward
		fog = !fog;
	}

}

void keyUp(unsigned char key, int x, int y) {
	keyStates[key] = false;
}

int main(int argc, char** argv) {

	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("Paperboy");

	// Tell glut where the display function is

	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);
	glutPassiveMotionFunc(mouseCallback);
	glutSetCursor(GLUT_CURSOR_NONE);

	// A call to glewInit() must be done after glut is initialized!
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	// Set up your objects and shaders
	init();

	//}
	// Create VAO, VBO, and EBO for the skybox
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Creates the cubemap texture object
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// These are very important to prevent seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// This might help with seams on some systems
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);


	// Cycles through all the textures and attaches them to the cubemap object
	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
			stbi_image_free(data);
		}
	}

	// Begin infinite event loop
	glutMainLoop();
	return 0;
}