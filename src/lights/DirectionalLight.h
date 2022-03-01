#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//default point light settings:
const glm::vec3 DL_DIRECTION(-1.0f, -1.0f, -1.0f);
const glm::vec3 DL_COLOR(1.0f, 1.0f, 1.0f);
const float DL_AMBIENT = 0.1f;
const float DL_DIFFUSE = 1.0f;
const float DL_SPECULAR = 0.5f;
const float DL_INTENSITY = 1.0f;
const bool DL_DISABLE = false;

class DirectionalLight {
public:
	glm::vec3 Direction;
	glm::vec3 Color;
	float Intensity;

	float AmbientCoef;
	float DiffuseCoef;
	float SpecularCoef;

	bool Disable;

private:
	glm::vec3 AmbientStrength;
	glm::vec3 DiffuseIntensity;
	glm::vec3 SpecularStrength;

private:
	void CalADSStrength() {
		AmbientStrength = AmbientCoef * Color;
		DiffuseIntensity = DiffuseCoef * Color;
		SpecularStrength = SpecularCoef * Color;
	}
public:
	DirectionalLight()
		: Direction(DL_DIRECTION),
		Color(DL_COLOR), Intensity(DL_INTENSITY),
		AmbientCoef(DL_AMBIENT), DiffuseCoef(DL_DIFFUSE), SpecularCoef(DL_SPECULAR),
		Disable(DL_DISABLE)
	{
		CalADSStrength();
	}

	DirectionalLight(glm::vec3 direction)
		: Direction(direction),
		Color(DL_COLOR), Intensity(DL_INTENSITY),
		AmbientCoef(DL_AMBIENT), DiffuseCoef(DL_DIFFUSE), SpecularCoef(DL_SPECULAR),
		Disable(DL_DISABLE)
	{
		CalADSStrength();
	}

	DirectionalLight(float directX, float directY, float directZ)
		: Direction(glm::vec3(directX, directY, directZ)),
		Color(DL_COLOR), Intensity(DL_INTENSITY),
		AmbientCoef(DL_AMBIENT), DiffuseCoef(DL_DIFFUSE), SpecularCoef(DL_SPECULAR),
		Disable(DL_DISABLE)
	{
		CalADSStrength();
	}

	//gtor
	glm::vec3 GetAmbient() const {
		return AmbientStrength;
	}
	glm::vec3 GetDiffuse() const {
		return DiffuseIntensity;
	}
	glm::vec3 GetSpecular() const {
		return SpecularStrength;
	}
};