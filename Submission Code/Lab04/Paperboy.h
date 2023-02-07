#pragma once
#include "maths_funcs.h"
#include "Model.h"
#include "Utils.h"
#include "Newspaper.h"
#include <list>


class PaperBoy
{
public:
	vec3 Pos;
	vec3 Front;
	vec3 Up;
	vec3 Right;
	vec3 WorldUp = vec3(0.0f, 1.0f, 0.0f);
	float Yaw = 0.0f;
	float Pitch = 0.0f;
	float MovementSpeed;
	Model paperboy;
	GLfloat rotate_y = 0.0f;
	GLfloat rotate_y_sin = 00.0f;
	int remainingPapers = 10;
	float health;
	Newspaper newspapers[];

	PaperBoy();
	PaperBoy(vec3 position);

	void renderPaperBoy(mat4 gWVP, int shaderID);
	void ProcessKeyboard(MovementDir direction, float deltaTime);
	void MovePaperBoy(int x, int y, float deltaTime);
	void throwPapers(Newspaper paper, Shader myShader, int remainingPapers);

	void spinWheels(float delta);

	float convert(float degree) {
		float pi = M_PI;
		return (degree * (pi / 180));
	}

	

private:
	mat4 GetViewMatrix();
	void updatePaperBoyVectors();

};