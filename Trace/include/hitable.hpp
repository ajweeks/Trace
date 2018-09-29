#pragma once

#include "ray.hpp"

class material;

struct hit_record
{
	float t;
	vec3 p;
	vec3 normal;
	material* mat;
};

class hitable
{
public:
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
};

class sphere : public hitable
{
public:
	sphere() = default;
	sphere(const vec3& center, float radius, material* mat) : center(center), radius(radius), mat(mat) {}
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override
	{
		vec3 oc = r.origin() - center;
		float a = dot(r.direction(), r.direction());
		float b = dot(oc, r.direction());
		float c = dot(oc, oc) - radius * radius;
		float discriminant = b * b - a * c;
		if (discriminant > 0)
		{
			float temp = (-b - sqrt(discriminant)) / a;
			if (temp < t_max && temp > t_min)
			{
				rec.t = temp;
				rec.p = r.point_at_parameter(rec.t);
				rec.normal = (rec.p - center) / radius;
				rec.mat = mat;
				return true;
			}

			temp = (-b + sqrt(discriminant)) / a;
			if (temp < t_max && temp > t_min)
			{
				rec.t = temp;
				rec.p = r.point_at_parameter(rec.t);
				rec.normal = (rec.p - center) / radius;
				rec.mat = mat;
				return true;
			}
		}

		return false;
	}

	vec3 center;
	float radius;
	material* mat;
};

class hitable_list : public hitable
{
public:
	hitable_list() = default;
	hitable_list(hitable** l, int n) : list(l), list_length(n) {}

	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override
	{
		hit_record temp_rec;
		bool bHitAnything = false;
		float closestSoFar = t_max;
		for (int i = 0; i < list_length; i++)
		{
			if (list[i]->hit(r, t_min, closestSoFar, temp_rec))
			{
				bHitAnything = true;
				closestSoFar = temp_rec.t;
				rec = temp_rec;
			}
		}

		return bHitAnything;
	}

	hitable** list;
	int list_length;
};
