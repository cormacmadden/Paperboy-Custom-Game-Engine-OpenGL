#include "PaperBoy.h"

PaperBoy::PaperBoy(vec3 position)
{
	paperboy = Model("D:\\Personal\\College\\5thYear\\ComputerGraphics\\Seal.obj");
	Pos = position;
	MovementSpeed = 5.0f;
	Front = vec3(1.0f, 0.0f, 0.0f);
	Right = vec3(0.0f, 0.0f, 1.0f);
}

PaperBoy::PaperBoy()
{
}

void PaperBoy::renderPaperBoy(mat4 pos, int shaderID) {

	//Transform the PaperBoy
	mat4 paperboy_pos = identity_mat4();
	paperboy_pos = translate(paperboy_pos, vec3(0.0f, 0.0f, 0.0f));
	paperboy_pos = rotate_y_deg(paperboy_pos, Yaw);
	paperboy_pos = translate(paperboy_pos, Pos);

	int world_loc = glGetUniformLocation(shaderID, "world");
	glUniformMatrix4fv(world_loc, 1, GL_FALSE, paperboy_pos.m);
	paperboy.RenderMesh(paperboy_pos, shaderID, 0);
	paperboy.RenderMesh(paperboy_pos, shaderID, 4);
	paperboy.RenderMesh(paperboy_pos, shaderID, 3);
	paperboy.RenderMesh(paperboy_pos, shaderID, 6);
	paperboy.RenderMesh(paperboy_pos, shaderID, 7);

	mat4 tailMat = identity_mat4();
	tailMat = translate(tailMat, vec3(0.25, -0.05, 0.0f));
	tailMat = rotate_z_deg(tailMat, -rotate_y_sin);
	tailMat = translate(tailMat, vec3(-0.25, 0.05, 0.0f));
	tailMat = paperboy_pos * tailMat;

	glUniformMatrix4fv(world_loc, 1, GL_FALSE, tailMat.m);
	paperboy.RenderMesh(tailMat, shaderID, 2);

	mat4 wingMatL = identity_mat4();
	mat4 wingMatR = identity_mat4();
	wingMatL = rotate_x_deg(wingMatL, -rotate_y_sin);
	wingMatR = rotate_x_deg(wingMatR, rotate_y_sin);
	wingMatL = paperboy_pos * wingMatL;
	wingMatR = paperboy_pos * wingMatR;

	glUniformMatrix4fv(world_loc, 1, GL_FALSE, wingMatL.m);
	paperboy.RenderMesh(wingMatL, shaderID, 5);
	glUniformMatrix4fv(world_loc, 1, GL_FALSE, wingMatR.m);
	paperboy.RenderMesh(wingMatR, shaderID, 1);
}

void PaperBoy::ProcessKeyboard(MovementDir direction, float deltaTime)
{
	float speed = MovementSpeed * deltaTime;
	if (direction == FORWARD)Pos += Front * speed;
	if (direction == BACKWARD)Pos -= Front * speed;
	if (direction == LEFT) {
		Yaw += 20.0f * speed;
	}
	if (direction == RIGHT) {
		Yaw -= 20.0f * speed;
	}
	updatePaperBoyVectors();
}

void PaperBoy::MovePaperBoy(int x, int y, float deltaTime) {

	float speed = MovementSpeed * deltaTime;
	Pos += Front * speed;
	if (x == 1)Yaw += 20.0f * speed;
	if (x == -1)Yaw -= 20.0f * speed;
	updatePaperBoyVectors();

}

mat4 PaperBoy::GetViewMatrix()
{
	return look_at(Pos, Pos + Front, Up);
}

void PaperBoy::updatePaperBoyVectors()
{
	vec3 front;
	front.v[0] = cos(convert(-Yaw)) * cos(convert(Pitch));
	front.v[1] = sin(convert(Pitch));
	front.v[2] = sin(convert(-Yaw)) * cos(convert(Pitch));

	Front = normalise(front);
	Right = normalise(cross(Front, WorldUp));
	Up = normalise(cross(Right, Front));
}
