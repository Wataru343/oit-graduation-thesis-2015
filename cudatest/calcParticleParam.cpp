#define _USE_MATH_DEFINES
#include <math.h>
#include <random>
#include "particle.h"
#include "calcEnergyCuda.cuh"

void makeUniformSphere(float *px, float *py, float *pz, float *vx, float *vy, float *vz, float *m, float offset_px, float offset_py, float offset_pz, float offset_vx, float offset_vy, float offset_vz, int particleNum, float radius, float mass, float vr){
	for (int i = 0; i < particleNum; i++){
		std::random_device rd;
		std::mt19937_64 mt(rd());
		std::uniform_real_distribution<float> rand1(0.0, 0.5f * radius * radius);
		std::uniform_real_distribution<float> rand_z(-1.0, 1.0);
		std::uniform_real_distribution<float> rand_2pi(0.0, 2.0f * (float)M_PI);

		float r = sqrt(2.0f * rand1(mt));
		float theta = rand_2pi(mt);
		float z = rand_z(mt);

		px[i] = r * sqrt(1.0f - z * z) * cos(theta);
		py[i] = r * sqrt(1.0f - z * z) * sin(theta);
		pz[i] = r * z;
	}

	for (int i = 0; i < particleNum; i++){
		m[i] = mass;
	}

	float pe = calcPotentialEnergyCuda(px, py, pz, m, particleNum);;//potentialEnergy

	float sumM = 0;
	for (int i = 0; i < particleNum; i++)sumM += m[i];

	float sigma = sqrt((2.0f * vr * abs(pe)) / (3.0f * sumM));

	for (int i = 0; i < particleNum; i++){
		std::random_device rd;
		std::mt19937_64 mt(rd());
		std::uniform_real_distribution<float> rand1(-1.0, 1.0);
		std::uniform_real_distribution<float> rand2(-1.0, 1.0);

		float x, y, r2;
		float z;
		do{
			x = rand1(mt);
			y = rand1(mt);
			r2 = x * x + y * y;
		} while (r2 >= 1.0f || r2 == 0.0f);
		z = sqrt(-2.0f * log(r2) / r2) * x;
		vx[i] = sigma * z;


		do{
			x = rand1(mt);
			y = rand1(mt);
			r2 = x * x + y * y;
		} while (r2 >= 1.0f || r2 == 0.0f);
		z = sqrt(-2.0f * log(r2) / r2) * x;

		vy[i] = sigma * z;


		do{
			x = rand1(mt);
			y = rand1(mt);
			r2 = x * x + y * y;
		} while (r2 >= 1.0f || r2 == 0.0f);
		z = sqrt(-2.0f * log(r2) / r2) * x;

		vz[i] = sigma * z;
	}

	/*for (int i = 0; i < particleNum; i++){
		float r = sqrt(px[i] * px[i] + py[i] * py[i] + pz[i] * pz[i]);
		float a = radius;
		float v = sqrt(3.0f * G * sumM * (a * a - r * r / 3.0f) / (a * a * a));
		vx[i] = py[i] / radius * v;
		vy[i] = -px[i] / radius * v;
		vz[i] = -pz[i] / radius  * v;
	}*/

	if (offset_px != 0.0f || offset_py != 0.0f || offset_pz != 0.0f && offset_vx != 0.0f || offset_vy != 0.0f || offset_vz != 0.0f){
		for (int i = 0; i < particleNum; i++){
			px[i] += offset_px;
			py[i] += offset_py;
			pz[i] += offset_pz;
			vx[i] += offset_vx;
			vy[i] += offset_vy;
			vz[i] += offset_vz;
		}
	}
}

void makePlummer(float *px, float *py, float *pz, float *vx, float *vy, float *vz, float *m, float offset_px, float offset_py, float offset_pz, float offset_vx, float offset_vy, float offset_vz, int particleNum, float a, float mass, float vr){
	for (int i = 0; i < particleNum; i++){
		m[i] = mass;
	}

	float sumM = 0;
	for (int i = 0; i < particleNum; i++)sumM += m[i];


	float n = 0;
	float max = pow(1.0f + pow(n / a, 2), -5.0f / 2.0f);

	std::random_device rd;
	std::mt19937_64 mt(rd());
	std::uniform_real_distribution<float> rand1(0, max);
	std::uniform_real_distribution<float> rand2(0, a * 2.0f);

	int i = 0;
	float radius, value, random;
	for (int i = 0; i < particleNum; i++){
		do{
			radius = rand2(mt);
			value = pow(1.0f + pow(radius / a, 2), -5.0f / 2.0f);
			random = rand1(mt);
		} while (random * max >= value);

		std::uniform_real_distribution<float> rand_z(-1.0, 1.0);
		std::uniform_real_distribution<float> rand_2pi(0.0, 2.0f * (float)M_PI);
		
		float r = radius * 1.0f;
		float theta = rand_2pi(mt);
		float z = rand_z(mt);

		px[i] = r * sqrt(1.0f - z * z) * cos(theta) * 1.0f;
		py[i] = r * sqrt(1.0f - z * z) * sin(theta) * 1.0f;
		pz[i] = r * z;
	}


	for (int i = 0; i < particleNum; i++){
		std::uniform_real_distribution<float> rand_z(-1.0, 1.0);
		std::uniform_real_distribution<float> rand_2pi(0.0, 2.0f * (float)M_PI);

		float r = sqrt(pow(px[i], 2) * pow(py[i], 2) * pow(pz[i], 2));
		float v = sqrt(G * sumM * pow(r, 2) * pow(pow(r, 2) + pow(a, 2), -3.0f / 2.0f));
		float theta = rand_2pi(mt);
		float z = rand_z(mt);
		

		vx[i] = v * sqrt(1.0f - z * z) * cos(theta) * 1.0f;
		vy[i] = v * sqrt(1.0f - z * z) * sin(theta) * 1.0f;
		vz[i] = v * z;
	}

	if (offset_px != 0.0f || offset_py != 0.0f || offset_pz != 0.0f && offset_vx != 0.0f || offset_vy != 0.0f || offset_vz != 0.0f){
		for (int i = 0; i < particleNum; i++){
			px[i] += offset_px;
			py[i] += offset_py;
			pz[i] += offset_pz;
			vx[i] += offset_vx;
			vy[i] += offset_vy;
			vz[i] += offset_vz;
		}
	}
}

void makeMiyamotoNagai(float *px, float *py, float *pz, float *vx, float *vy, float *vz, float *m, float offset_px, float offset_py, float offset_pz, float offset_vx, float offset_vy, float offset_vz, int particleNum, float a, float b, float mass, float vr){
	float n = 0;
	float max = (0.0f + (a + 3.0f * sqrt(0.0f + pow(b, 2))) * pow((a + sqrt(0.0f + pow(b, 2))), 2)) / (pow((0.0f + pow((a + sqrt(0.0f + pow(b, 2))), 2)), 5.0f / 2.0f) * pow((0 + pow(b, 2)), 3.0f / 2.0f));

	std::random_device rd;
	std::mt19937_64 mt(rd());
	std::uniform_real_distribution<float> rand1(0, 1.0f);
	std::uniform_real_distribution<float> rand_x(a * -5.0f, a * 5.0f);
	std::uniform_real_distribution<float> rand_y(b * -3.0f, b * 3.0f);
	std::uniform_real_distribution<float> rand_z(a * -5.0f, a * 5.0f);

	float threshold = (a * pow(rand_x.max(), 2) + (a + 3.0f * sqrt(pow(0.0f, 2) + pow(b, 2))) * pow((a + sqrt(pow(0.0f, 2) + pow(b, 2))), 2)) / (pow((pow(rand_x.max(), 2) + pow((a + sqrt(pow(0.0f, 2) + pow(b, 2))), 2)), 5.0f / 2.0f) * pow((pow(0.0f, 2) + pow(b, 2)), 3.0f / 2.0f));

	float x, y, z, r, value, random;
	for (int i = 0; i < particleNum; i++){
		do{
			x = rand_x(mt);
			y = rand_y(mt);
			z = rand_z(mt);
			r = sqrt(x * x + z * z);

			value = (a * pow(r, 2) + (a + 3.0f * sqrt(pow(y, 2) + pow(b, 2))) * pow((a + sqrt(pow(y, 2) + pow(b, 2))), 2)) / (pow((pow(r, 2) + pow((a + sqrt(pow(y, 2) + pow(b, 2))), 2)), 5.0f / 2.0f) * pow((pow(y, 2) + pow(b, 2)), 3.0f / 2.0f));
			random = rand1(mt);
		} while (random * max + threshold >= value || r > rand_x.max());
		px[i] = x;
		py[i] = y;
		pz[i] = z;
		vx[i] = 0;
		vy[i] = 0;
		vz[i] = 0;

	}

	

	float sumM = 0;
	for (int i = 0; i < particleNum; i++){
		m[i] = mass;
		sumM += m[i];
	}

	px[0] = 0;
	py[0] = 0;
	pz[0] = 0;
	vx[0] = 0;
	vy[0] = 0;
	vz[0] = 0;
	m[0] = mass * 10000.0f;

	for (int i = 0; i < particleNum; i++){
		float r = sqrt(px[i] * px[i] + pz[i] * pz[i]);
		if (r == 0)r = 0.00001f;
		vx[i] = sin(abs(pz[i]) / r) * (pz[i] / r) * 80.0f;
		vz[i] = -cos(abs(px[i]) / r) * (px[i] / r) * 80.0f;
	}

	if (offset_px != 0.0f || offset_py != 0.0f || offset_pz != 0.0f && offset_vx != 0.0f || offset_vy != 0.0f || offset_vz != 0.0f){
		for (int i = 0; i < particleNum; i++){
			px[i] += offset_px;
			py[i] += offset_py;
			pz[i] += offset_pz;
			vx[i] += offset_vx;
			vy[i] += offset_vy;
			vz[i] += offset_vz;
		}
	}
}