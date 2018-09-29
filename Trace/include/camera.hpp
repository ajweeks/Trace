#pragma once

#define _USE_MATH_DEFINES
#include "math.h"

#include "ray.hpp"
#include "rand.hpp"

class camera
{
public:
	camera(const vec3& eye, const vec3& lookAt, const vec3& up,
		float verticalFOVDeg, float aspectRatio,
		float aperture, float focusDist)
	{
		lensRadius = aperture / 2.0f;
		float theta = verticalFOVDeg * (float)M_PI / 180.0f;
		float halfHeight = tan(theta/2.0f);
		float halfWidth = aspectRatio * halfHeight;

		origin = eye;
		w = normalize(eye - lookAt);
		u = normalize(cross(up, w));
		v = cross(w, u);
		lower_left_corner = origin - halfWidth * focusDist * u - halfHeight * focusDist * v - focusDist * w;
		horizontal = 2.0f * halfWidth * focusDist * u;
		vertical = 2.0f * halfHeight * focusDist * v;
	}

	ray get_ray(float s, float t)
	{
		vec3 rd = lensRadius * rand::random_in_unit_disk();
		vec3 offset = u * rd.x() + v * rd.y();
		return ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset);
	}

	vec3 origin;
	vec3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 u, v, w;
	float lensRadius;
};
