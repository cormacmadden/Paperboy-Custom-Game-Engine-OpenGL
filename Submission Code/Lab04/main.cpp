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

using namespace std;

Texture* pTexture = NULL;
Texture* sphereTexture = NULL;
Shader myShader;
Shader skyShader;
Model terrain;
Model sphere;
PaperBoy paperboy;
mat4 gWVP;
Newspaper paper;
Model hedges;
Newspaper *newspapers;

void display() {
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

	//glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor(skyR, skyG, skyB, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//get uniform location from shader
	int view_loc = glGetUniformLocation(myShader.ID, "view");
	int proj_loc = glGetUniformLocation(myShader.ID, "proj");
	int view_pos_loc = glGetUniformLocation(myShader.ID, "viewPos");
	int fog_loc = glGetUniformLocation(myShader.ID, "density");
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
	mat4 persp_proj = perspective(camera.Fov, (float)width / (float)height, 0.1f, 1000.0f);
	mat4 view = camera.GetViewMatrix();
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_loc, 1, GL_FALSE, view.m);
	mat4 world = identity_mat4();
	mat4 sky_mat = identity_mat4();
	
	if (skyMap == true) {
		skyShader.use();
		sky_mat = scale(sky_mat, vec3(3.2f, 3.2f, 3.2f));
		sky_mat = translate(sky_mat, vec3(0.0f, 17.0f, 0.0f));
		//sky_mat = translate(sky_mat, camera.Pos);
		//sky_mat = rotate_y_deg(sky_mat, camera.Yaw);
		vec4 pos = sky_mat * vec4(camera.Pos, 0.0f);
		vec3 pos1 = vec3(pos.v[0], pos.v[1], pos.v[2]);
		sphere.RenderModel(view, skyShader.ID);
	}


	//myShader.setVec3("dirLight.direction", vec3(-0.2f, -1.0f, -0.3f));
	//myShader.setVec3("dirLight.ambient", vec3(0.05f, 0.05f, 0.05f));
	//myShader.setVec3("dirLight.diffuse", vec3(0.4f, 0.4f, 0.4f));
	//myShader.setVec3("dirLight.specular", vec3(0.5f, 0.5f, 0.5f));
	glUniform3f(view_pos_loc, camera.Pos.v[0], camera.Pos.v[1], camera.Pos.v[2]);
	myShader.use();
	mat4 terrain_mat = identity_mat4();
	terrain_mat = translate(terrain_mat, vec3(0.0, 00.0, 0.0));
	terrain.RenderModel(terrain_mat, myShader.ID);

	hedges.RenderModel(terrain_mat, myShader.ID);

	mat4 paper_mat = identity_mat4();

	for (int i = 0; i < paperboy.remainingPapers; i++) {
		newspapers[i].renderNewspaper(paper_mat, myShader.ID);
	}

	paperboy.renderPaperBoy(world, myShader.ID);
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
	myShader = Shader("D:\\Personal\\College\\5thYear\\ComputerGraphics\\simpleVertexShader.txt","D:\\Personal\\College\\5thYear\\ComputerGraphics\\simpleFragmentShader.txt");
	skyShader = Shader("D:\\Personal\\College\\5thYear\\ComputerGraphics\\simpleVertexShader.txt", "D:\\Personal\\College\\5thYear\\ComputerGraphics\\skyShader.txt");
	paperboy = PaperBoy(vec3(0.5f, 0.1f, 2.0f));
	terrain = Model("D:\\Personal\\College\\5thYear\\ComputerGraphics\\Models\\Map.obj");
	sphere = Model("D:\\Personal\\College\\5thYear\\ComputerGraphics\\Models\\SphereScene.obj");
	hedges = Model("D:\\Personal\\College\\5thYear\\ComputerGraphics\\Models\\Hedges.obj");
	pTexture = new Texture(GL_TEXTURE_2D, "qG00I.png");
	sphereTexture = new Texture(GL_TEXTURE_2D, "sunflowers_2k.hdr");
	paper = Newspaper(vec3(0.3f, 0.3f, 0.0f));
	newspapers = new Newspaper[10];
	Newspaper* newspapers = (Newspaper*)malloc(sizeof(Newspaper) * 50);
	// calling constructor
	// for each index of array
	for (int i = 0; i < 50; i++) {
	///	arr[i] = Newspaper();
	}


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
		paperboy.throwPapers(paper, myShader, paperboy.remainingPapers);
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
	// Begin infinite event loop
	glutMainLoop();
	return 0;
}