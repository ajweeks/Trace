
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

		if (depth < 30 && rec.mat->scatter(r, rec, attenuation, scattered))
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
	int screenWidth = 480;
	int screenHeight = 360;
	int numSamples = 100;

	float aspectRatio = (float)screenWidth / (float)screenHeight;

	std::ofstream imageOut;
	imageOut.open("./../../../out/result.ppm", std::fstream::out);

	if (imageOut.is_open())
	{
		imageOut << "P3\n" << screenWidth << ' ' << screenHeight << "\n255\n";

		vec3 lower_left_corner(-2.0f, -1.0f, -1.0f);
		vec3 horizontal(4.0f, 0.0f, 0.0f);
		vec3 vertical(0.0f, 2.0f, 0.0f);
		vec3 origin(0.0f);

#if 0
		constexpr int numHitables = 5;
		hitable* list[numHitables];
		list[0] = new sphere(vec3(0, 0, -1), 0.5f, new lambertian(vec3(0.1f, 0.2f, 0.5f)));
		list[1] = new sphere(vec3(0, -100.5f, -1), 100.0f, new lambertian(vec3(0.8f, 0.8f, 0.0f)));
		list[2] = new sphere(vec3(1, 0, -1), 0.5f, new metal(vec3(0.8f, 0.6f, 0.2f), 0.0f));
		list[3] = new sphere(vec3(-1, 0, -1), 0.5f, new dielectric(1.5f));
		list[4] = new sphere(vec3(-1, 0, -1), -0.45f, new dielectric(1.5f));
		hitable* world = new hitable_list(list, numHitables);
#elif 0
		float R = cos((float)M_PI / 4.0f);
		constexpr int numHitables = 2;
		hitable* list[numHitables];
		list[0] = new sphere(vec3(-R, 0, -1), R, new lambertian(vec3(0, 0, 1)));
		list[1] = new sphere(vec3(R, 0, -1), R, new lambertian(vec3(1, 0, 0)));
		hitable* world = new hitable_list(list, numHitables);
#else
		constexpr int numHitables = 500;
		hitable* list[numHitables + 1];
		list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5f, 0.5f, 0.5f)));

		int i = 1;
		for (int a = -11; a < 11; a++)
		{
			for (int b = -11; b < 11; b++)
			{
				float mat = rand::GetFloat01();
				vec3 center(a + 0.9f * rand::GetFloat01(), 0.2f, b + 0.9f * rand::GetFloat01());
				if ((center - vec3(1, 0.2f, 0)).length() > 0.9f)
				{
					if (mat < 0.75f) // diffuse
					{
						float red = rand::GetFloat01() * rand::GetFloat01();
						float green = rand::GetFloat01() * rand::GetFloat01();
						float blue = rand::GetFloat01() * rand::GetFloat01();
						list[i++] = new sphere(center, 0.2f, new lambertian(vec3(red, green, blue)));
					}
					else if (mat < 0.9f) // metal
					{
						float red = 0.5f * (1.0f + rand::GetFloat01());
						float green = 0.5f * (1.0f + rand::GetFloat01());
						float blue = 0.5f * (1.0f + rand::GetFloat01());
						float fuzziness = 0.5f * rand::GetFloat01();
						list[i++] = new sphere(center, 0.2f, new metal(vec3(red, green, blue), fuzziness));
					}
					else // glass
					{
						list[i++] = new sphere(center, 0.2f, new dielectric(1.5f));
					}
				}
			}
		}

		list[i++] = new sphere(vec3(0, 1, 0), 1.0f, new dielectric(1.5f));
		list[i++] = new sphere(vec3(-4, 1, 0), 1.0f, new lambertian(vec3(0.4f, 0.2f, 0.1f)));
		list[i++] = new sphere(vec3(4, 1, 0), 1.0f, new metal(vec3(0.7f, 0.6f, 0.5f), 0.0f));

		hitable* world = new hitable_list(list, i);
#endif

		vec3 eye(10, 2.0f, 2);
		vec3 lookAt(0, 0, 0);
		float aperture = 0.075f;
		float dist_to_focus = (eye - lookAt).length() - 5;
		camera cam(eye, lookAt, vec3(0, 1, 0), 30, aspectRatio, aperture, dist_to_focus);

		for (int j = screenHeight - 1; j >= 0; j--)
		{
			for (int i = 0; i < screenWidth; i++)
			{
				vec3 col = vec3(0.0f);

				for (int s = 0; s < numSamples; s++)
				{
					float rand1f = rand::GetFloat01();
					float rand2f = rand::GetFloat01();
					float u = (float)(i + rand1f) / (float)screenWidth;
					float v = (float)(j + rand2f) / (float)screenHeight;

					ray r = cam.get_ray(u, v);
					vec3 p = r.point_at_parameter(2.0f);
					col += color(r, world, 0);
				}
				col /= (float)numSamples;

				col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));

				int ir = (int)(255.99 * col[0]);
				int ig = (int)(255.99 * col[1]);
				int ib = (int)(255.99 * col[2]);
				imageOut << ir << ' ' << ig << ' ' << ib << '\n';
			}

			if (j % (screenHeight / 10) == 0)
			{
				int percentComplete = 100 - (int)(100.0f * (j / (float)screenHeight));
				printf("%d%% complete...\n", percentComplete);
			}
		}

		imageOut.close();
	}

	std::cout << "Done!\n";

	char c;
	std::cin >> c;
}
