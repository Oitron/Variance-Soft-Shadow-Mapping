#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


//default point light settings:
const glm::vec3 SL_POSITION(-1.0f, 1.0f, 1.0f);
const glm::vec3 SL_DIRECTION(glm::vec3(0.0f) - SL_POSITION);
const float INPHI = 12.5f;
const float OUTPHI = 17.5f;
const glm::vec3 SL_COLOR(1.0f, 1.0f, 1.0f);
const float SL_AMBIENT = 0.1f;
const float SL_DIFFUSE = 1.0f;
const float SL_SPECULAR = 0.5f;
const float SL_INTENSITY = 1.0f;

const float SL_KC = 1.0f;
const float SL_KL = 0.09f;
const float SL_KQ = 0.032f;

const bool SL_DISABLE = false;


class SpotLight {
public:
	glm::vec3 Position;
	glm::vec3 Direction;
	//The middle area of the inner and outer circle is the transition area, 
	//that is, between illuminated and unilluminated
	float InPhi; //Cutting angle: that is, the spotlight illumination range (inner circle)
	float OutPhi; //Cutting angle: that is, the spotlight illumination range (outer circle)
	glm::vec3 Color;
	float Intensity;

	float Constant;
	float Linear;
	float Quadratic;

	bool Disable;

private:
	float InCutOff;
	float OutCutOff;
public:
	float AmbientCoef;
	float DiffuseCoef;
	float SpecularCoef;

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
	void CalCutOff() {
		InCutOff = glm::cos(glm::radians(InPhi));
		OutCutOff = glm::cos(glm::radians(OutPhi));
	}
public:
	//ctor
	SpotLight()
		: Position(SL_POSITION), Direction(SL_DIRECTION), InPhi(INPHI), OutPhi(OUTPHI),
		Color(SL_COLOR), Intensity(SL_INTENSITY),
		AmbientCoef(SL_AMBIENT), DiffuseCoef(SL_DIFFUSE), SpecularCoef(SL_SPECULAR),
		Constant(SL_KC), Linear(SL_KL), Quadratic(SL_KQ),
		Disable(SL_DISABLE)
	{
		CalADSStrength();
		CalCutOff();
	}

	SpotLight(glm::vec3 position, glm::vec3 direction, float phi)
		: Position(position), Direction(direction), InPhi(INPHI), OutPhi(OUTPHI),
		Color(SL_COLOR), Intensity(SL_INTENSITY),
		AmbientCoef(SL_AMBIENT), DiffuseCoef(SL_DIFFUSE), SpecularCoef(SL_SPECULAR),
		Constant(SL_KC), Linear(SL_KL), Quadratic(SL_KQ),
		Disable(SL_DISABLE)
	{
		CalADSStrength();
		CalCutOff();
	}

	SpotLight(float posX, float posY, float posZ,
		float directX, float directY, float directZ,
		float phi)
		: Position(glm::vec3(posX, posY, posZ)), Direction(glm::vec3(directX, directY, directZ)), InPhi(INPHI), OutPhi(OUTPHI),
		Color(SL_COLOR), Intensity(SL_INTENSITY),
		AmbientCoef(SL_AMBIENT), DiffuseCoef(SL_DIFFUSE), SpecularCoef(SL_SPECULAR),
		Constant(SL_KC), Linear(SL_KL), Quadratic(SL_KQ),
		Disable(SL_DISABLE)
	{
		CalADSStrength();
		CalCutOff();
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
	float GetInCutOff() {
		CalCutOff();
		return InCutOff;
	}
	float GetOutCutOff() {
		CalCutOff();
		return OutCutOff;
	}
	float GetInCutOff() const {
		return InCutOff;
	}
	float GetOutCutOff() const {
		return OutCutOff;
	}
};