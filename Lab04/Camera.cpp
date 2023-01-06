#include "Camera.h"
#include <iostream>


//function to return cameras viewpoint as mat4
mat4 Camera::GetViewMatrix()
{
    return look_at(Pos, Pos + Front, Up);
}
//function to handle change of cam position due to keyboard input 
void Camera::ProcessKeyboard(MovementDir direction, float deltaTime)
{
    float speed = MovementSpeed * deltaTime;
    //std::cout << deltaTime;
    if (direction == FORWARD)Pos += Front * speed;
    if (direction == BACKWARD)Pos -= Front * speed;
    if (direction == LEFT)Pos -= Right * speed;
    if (direction == RIGHT) Pos += Right * speed;
    if (direction == UP) Pos += WorldUp * speed;
    if (direction == DOWN) Pos -= WorldUp * speed;
}

void Camera::processThirdPerson(vec3 position, vec3 direction) {
    Pos = position + direction*(-2.0) + vec3(0.0,0.9,0.0);
    Front = direction;
    
    Fov = 70.0f;
    //Pos += vec3(3.0, 0.0, 0.0);
    Pitch = -25.0f;
    //Yaw = convert(atan(direction.v[2] / direction.v[0]));
    updateCamVectorsNoFront();
}

void Camera::processIso(vec3 position, vec3 direction) {
    Front = direction;
    Pos = position + vec3(-200.0, 150.0, 200.0);
    //Pos += vec3(3.0, 0.0, 0.0);
    Fov = 2.0f;
    Pitch = -27.5f;
    Yaw = -45.0f;
    updateCamVectors();
}

 //function to allow camera to move with mouse pos
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
{
    xoffset *= MouseSensitivity;//multiply offset by how sensitive mouse is
    yoffset *= MouseSensitivity;
    Yaw += xoffset;
    Pitch += yoffset;

    if (constrainPitch) //not allow camera to go behind us 
    {
        if (Pitch > 89.0f) Pitch = 89.0f;
        if (Pitch < -89.0f) Pitch = -89.0f;
    }
    updateCamVectors();
}


//function to convert degrees to radians
float Camera::convert(float degree) {
    float pi = M_PI;
    return (degree * (pi / 180));
}

//function to update cam's vectors 
void Camera::updateCamVectors()
{
    vec3 front;
    front.v[0] = cos(convert(Yaw)) * cos(convert(Pitch));
    front.v[1] = sin(convert(Pitch));
    front.v[2] = sin(convert(Yaw)) * cos(convert(Pitch));

    Front = normalise(front);
    Right = normalise(cross(Front, WorldUp));
    Up = normalise(cross(Right, Front));
}
void Camera::updateCamVectorsNoFront()
{
    
    Right = normalise(cross(Front, WorldUp));
    Up = normalise(cross(Right, Front));
}