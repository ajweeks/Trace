#pragma once

#include <random>

#include "vec3.hpp"

class rand
{
public:
	static float GetFloat01()
	{
		float result = (float)dist(gen) / INT_MAX;
		return result;
	}

	static vec3 random_in_unit_disk()
	{
		vec3 p;
		do
		{
			p = 2.0f * vec3(GetFloat01(), GetFloat01(), 0.0f) - vec3(1.0f, 1.0f, 0.0f);
		} while (dot(p, p) >= 1.0f);
		return p;
	}

	static vec3 random_in_unit_sphere()
	{
		vec3 p;
		do
		{
			float rand1f = rand::GetFloat01();
			float rand2f = rand::GetFloat01();
			float rand3f = rand::GetFloat01();
			p = 2.0f * vec3(rand1f, rand2f, rand3f) - vec3(1.0f);
		} while (p.length_squared() >= 1.0f);
		return p;
	}

	static std::random_device rd;
	static std::mt19937 gen;// (rd());
	static std::uniform_int_distribution<> dist;// (0, INT_MAX - 1);
};
