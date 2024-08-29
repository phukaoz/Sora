#pragma once

#include "Sora.h"

#include <glm/glm.hpp>

struct ParticleProps
{
	glm::vec2 Position;
	glm::vec2 Velocity, VelocityVariation;
	glm::vec4 ColorBegin, ColorEnd;
	float SizeBegin, SizeEnd, SizeVariation;
	float LifeTime;

	ParticleProps()
		: Position({ 0.0f, 0.0f }),
		Velocity({ 0.0f, 0.0f }), VelocityVariation({ 3.0f, 1.0f }),
		ColorBegin({ 0.996f, 0.831f, 0.482f, 1.0f }), ColorEnd({ 0.966f, 0.427f, 0.160f, 1.0f }),
		SizeBegin(0.5f), SizeEnd(0.0f), SizeVariation(0.3f),
		LifeTime(1.0f)
	{
	}
};

class ParticleSystem
{
public:
	ParticleSystem(uint32_t maxParticle = 1000);

	void OnUpdate(Sora::Timestep ts);
	void OnRender(Sora::OrthographicCamera& camera);

	void Emit(const ParticleProps& particleProps);
private:
	struct Particle
	{
		glm::vec2 Position;
		glm::vec2 Velocity;
		glm::vec4 ColorBegin, ColorEnd;
		float Rotation = 0.0f;
		float SizeBegin, SizeEnd;

		float LifeTime = 1.0f;
		float LifeRemaining = 0.0f;

		bool Active = false;
	};
	std::vector<Particle> m_ParticlePool;
	uint32_t m_PoolIndex;
};

