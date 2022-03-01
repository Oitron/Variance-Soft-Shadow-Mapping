#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum CameraDirection
{
	FORWORD,
	BACKWORD,
	LEFT,
	RIGHT
};

enum ViewMode
{
	PERSPECTIVE,
	ORTHO
};

enum CameraMode
{
	FPS,
	FPS_FLY
};

//default camera settings
const glm::vec3 CAMERA_POS(0.0f, 0.0f, 5.0f);
const glm::vec3 WORLD_UP(0.0f, 1.0f, 0.0f);
const float PITCH = 0.0f;
const float YAW = -90.0f;

const float MOVEMENT_SPEED = 4.0f;
const float MOUSE_SENSITIVITY = 0.2f;

const float FOV = 45.0f;
const float ASPECT_RATIO = 4 / 3;
const float NEAR_PLANE = 0.1f;
const float FAR_PLANE = 100.0f;
const float LEFT_PLANE = 0.0f;
const float RIGHT_PLANE = 1600.0f;
const float BOTTOM_PLANE = 0.0f;
const float TOP_PLANE = 1200.f;
const CameraMode MODE = FPS_FLY;

class Camera {
private:
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Right;
	glm::vec3 Up;
	glm::vec3 WorldUp;

	float Pitch;
	float Yaw;

public:
	float MovementSpeed;
	float MouseSensitivity;

	float Fov;
	float AspectRatio;
	float NearPlane;
	float FarPlane;
	float LeftPlane;
	float RightPlane;
	float TopPlane;
	float BottomPlane;

	CameraMode Mode;

private:
	void updateCameraVectors() {
		glm::vec3 front;
		front.x = cos(glm::radians(Pitch)) * cos(glm::radians(Yaw));
		front.y = sin(glm::radians(Pitch));
		front.z = cos(glm::radians(Pitch)) * sin(glm::radians(Yaw));

		Front = glm::normalize(front);
		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}

public:
	//ctor
	Camera()
		: Position(CAMERA_POS), WorldUp(WORLD_UP), Pitch(PITCH), Yaw(YAW),
		MovementSpeed(MOVEMENT_SPEED), MouseSensitivity(MOUSE_SENSITIVITY),
		Fov(FOV), AspectRatio(ASPECT_RATIO),
		NearPlane(NEAR_PLANE), FarPlane(FAR_PLANE),
		LeftPlane(LEFT_PLANE), RightPlane(RIGHT_PLANE),
		TopPlane(TOP_PLANE), BottomPlane(BOTTOM_PLANE),
		Mode(MODE)
	{
		updateCameraVectors();
	}

	Camera(glm::vec3 pos, glm::vec3 world_up, float pitch, float yaw)
		: Position(pos), WorldUp(world_up), Pitch(pitch), Yaw(yaw),
		MovementSpeed(MOVEMENT_SPEED), MouseSensitivity(MOUSE_SENSITIVITY),
		Fov(FOV), AspectRatio(ASPECT_RATIO),
		NearPlane(NEAR_PLANE), FarPlane(FAR_PLANE),
		LeftPlane(LEFT_PLANE), RightPlane(RIGHT_PLANE),
		TopPlane(TOP_PLANE), BottomPlane(BOTTOM_PLANE),
		Mode(MODE)
	{
		updateCameraVectors();
	}

	Camera(float posX, float posY, float posZ, float wuX, float wuY, float wuZ, float pitch, float yaw)
		: Position(glm::vec3(posX, posY, posZ)), WorldUp(glm::vec3(wuX, wuY, wuZ)), Pitch(pitch), Yaw(yaw),
		MovementSpeed(MOVEMENT_SPEED), MouseSensitivity(MOUSE_SENSITIVITY),
		Fov(FOV), AspectRatio(ASPECT_RATIO),
		NearPlane(NEAR_PLANE), FarPlane(FAR_PLANE),
		LeftPlane(LEFT_PLANE), RightPlane(RIGHT_PLANE),
		TopPlane(TOP_PLANE), BottomPlane(BOTTOM_PLANE),
		Mode(MODE)
	{
		updateCameraVectors();
	}
	//dtor
	~Camera() {};


	glm::mat4 GetViewMatrix() {
		return glm::lookAt(Position, Position + Front, WorldUp);
	}

	glm::mat4 GetProjectionMatrix(ViewMode mode) {
		glm::mat4 projection;
		if (mode == PERSPECTIVE)
			projection = glm::perspective(glm::radians(Fov), AspectRatio, NearPlane, FarPlane);
		else if (mode == ORTHO)
			projection = glm::ortho(LeftPlane, RightPlane, BottomPlane, TopPlane, NearPlane, FarPlane);
		else
			std::cout << "Camera mode invalid!" << std::endl;
		return projection;
	}


	void ProcessKeyboard(CameraDirection direction, float deltaTime) {
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWORD)
			Position += velocity * Front;
		if (direction == BACKWORD)
			Position -= velocity * Front;
		if (direction == LEFT)
			Position -= velocity * Right;
		if (direction == RIGHT)
			Position += velocity * Right;
		if (Mode == FPS)
			Position.y = 0.0f;
	}

	void ProcessMouseMovement(float x_offset, float y_offset) {
		x_offset *= MouseSensitivity;
		y_offset *= MouseSensitivity;

		Yaw += x_offset;
		Pitch += y_offset;

		//ÏÞÖÆÊÓ½Ç·¶Î§
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		else if (Pitch < -89.0f)
			Pitch = -89.0f;

		updateCameraVectors();
	}

	//gtor
	glm::vec3 GetVectorFront() {
		return Front;
	}
	glm::vec3 GetVectorRight() {
		return Right;
	}
	glm::vec3 GetVectorUp() {
		return Up;
	}
	glm::vec3 GetCamPos() {
		return Position;
	}

};