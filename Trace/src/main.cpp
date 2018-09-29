
#include <iostream>
#include <fstream>

#include "camera.hpp"
#include "hitable.hpp"
#include "rand.hpp"
#include "ray.hpp"
#include "vec3.hpp"
#include "material.hpp"

vec3 color(const ray& r, hitable* world, int depth)
{
	hit_record rec;

	if (world->hit(r, 0.001f, FLT_MAX, rec))
	{
		ray scattered;
		vec3 attenuation;

		if (depth < 50 && rec.mat->scatter(r, rec, attenuation, scattered))
		{
			return attenuation * color(scattered, world, depth + 1);
		}
		else
		{
			return vec3(0.0f);
		}
		// Normals visualization:
		//vec3(rec.normal.x()+1, rec.normal.y()+1, rec.normal.z()+1)
	}
	else
	{
		vec3 unit_direction = normalize(r.direction());
		float t = 0.5f * (unit_direction.y() + 1.0f);
		return (1.0f - t) * vec3(1.0f) + t * vec3(0.5f, 0.7f, 1.0f);
	}

}

int main()
{
	int nx = 200;
	int ny = 100;
	int ns = 100;

	std::ofstream out;
	out.open("./../../../out/result.ppm", std::fstream::out);

	if (out.is_open())
	{
		out << "P3\n" << nx << ' ' << ny << "\n255\n";

		vec3 lower_left_corner(-2.0f, -1.0f, -1.0f);
		vec3 horizontal(4.0f, 0.0f, 0.0f);
		vec3 vertical(0.0f, 2.0f, 0.0f);
		vec3 origin(0.0f);

		hitable* list[4];
		list[0] = new sphere(vec3(0, 0, -1), 0.5f, new lambertian(vec3(0.8f, 0.3f, 0.3f)));
		list[1] = new sphere(vec3(0, -100.5f, -1), 100.0f, new lambertian(vec3(0.8f, 0.8f, 0.0f)));
		list[2] = new sphere(vec3(1, 0, -1), 0.5f, new metal(vec3(0.8f, 0.6f, 0.2f)));
		list[3] = new sphere(vec3(-1, 0, -1), 0.5f, new metal(vec3(0.8f, 0.8f, 0.8f)));
		hitable* world = new hitable_list(list, 4);

		camera cam;

		for (int j = ny - 1; j >= 0; j--)
		{
			for (int i = 0; i < nx; i++)
			{
				vec3 col = vec3(0.0f);

				for (int s = 0; s < ns; s++)
				{
					float rand1f = rand::GetFloat01();
					float rand2f = rand::GetFloat01();
					float u = (float)(i + rand1f) / (float)nx;
					float v = (float)(j + rand2f) / (float)ny;

					ray r = cam.get_ray(u, v);
					vec3 p = r.point_at_parameter(2.0f);
					col += color(r, world, 0);
				}
				col /= (float)ns;

				col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));

				int ir = (int)(255.99 * col[0]);
				int ig = (int)(255.99 * col[1]);
				int ib = (int)(255.99 * col[2]);
				out << ir << ' ' << ig << ' ' << ib << '\n';
			}
		}
	}

	std::cout << "Done!\n";

	char c;
	std::cin >> c;
}
