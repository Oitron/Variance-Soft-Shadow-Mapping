#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//default point light settings:
const glm::vec3 PL_POSITION(2.0f, 2.0f, 2.0f);
const glm::vec3 PL_COLOR(1.0f, 1.0f, 1.0f);
const float PL_AMBIENT = 0.1f;
const float PL_DIFFUSE = 1.0f;
const float PL_SPECULAR = 0.5f;
const float PL_INTENSITY = 1.0f;
const float PL_KC = 1.0f;
const float PL_KL = 0.09f;
const float PL_KQ = 0.032f;
const bool PL_DISABLE = false;

class PointLight {
public:
	glm::vec3 Position;
	glm::vec3 Color;
	float Intensity;

	float Constant;
	float Linear;
	float Quadratic;

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
	//ctor
	PointLight()
		: Position(PL_POSITION),
		Color(PL_COLOR), Intensity(PL_INTENSITY),
		Constant(PL_KC), Linear(PL_KL), Quadratic(PL_KQ),
		AmbientCoef(PL_AMBIENT), DiffuseCoef(PL_DIFFUSE), SpecularCoef(PL_SPECULAR),
		Disable(PL_DISABLE)
	{
		CalADSStrength();
	}

	PointLight(glm::vec3 position)
		: Position(position),
		Color(PL_COLOR), Intensity(PL_INTENSITY),
		Constant(PL_KC), Linear(PL_KL), Quadratic(PL_KQ),
		AmbientCoef(PL_AMBIENT), DiffuseCoef(PL_DIFFUSE), SpecularCoef(PL_SPECULAR),
		Disable(PL_DISABLE)
	{
		CalADSStrength();
	}

	PointLight(float posX, float posY, float posZ)
		: Position(glm::vec3(posX, posY, posZ)),
		Color(PL_COLOR), Intensity(PL_INTENSITY),
		Constant(PL_KC), Linear(PL_KL), Quadratic(PL_KQ),
		AmbientCoef(PL_AMBIENT), DiffuseCoef(PL_DIFFUSE), SpecularCoef(PL_SPECULAR),
		Disable(PL_DISABLE)
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