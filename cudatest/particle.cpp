#define _USE_MATH_DEFINES
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <Windows.h>
#include <random>
#include <omp.h>
#include "particle.h"
#include "calcEnergyCuda.cuh"
#include "calcNbodyCuda.cuh"


particle::particle(){
	px = py = pz = vx = vy = vz = ax = ay = az = jx = jy = jz = m = preax = preay = preaz = prejx = prejy = prejz = NULL;
	texid = NULL;
	node_num = NULL;
	size = 0;
	eps2 = 0.0078125f;
	bFirstCalculation = TRUE;
}

particle::~particle(){
	_aligned_free(px);
	_aligned_free(py);
	_aligned_free(pz);
	_aligned_free(vx);
	_aligned_free(vy);
	_aligned_free(vz);
	_aligned_free(ax);
	_aligned_free(ay);
	_aligned_free(az);
	_aligned_free(jx);
	_aligned_free(jy);
	_aligned_free(jz);
	_aligned_free(preax);
	_aligned_free(preay);
	_aligned_free(preaz);
	_aligned_free(prejx);
	_aligned_free(prejy);
	_aligned_free(prejz);
	_aligned_free(m);
	_aligned_free(texid);
	_aligned_free(node_num);
}

void particle::makeUniformSphere(unsigned int num, float radius, float mass, float vr, float shift_px, float shift_py, float shift_pz, float shift_vx, float shift_vy, float shift_vz){
	addParticle(num);
	ptrShift(size - num);

	for (unsigned int i = 0; i < num; i++){
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

	for (unsigned int i = 0; i < num; i++){
		m[i] = mass;
	}

	float pe = calcPotentialEnergyCuda(px, py, pz, m, num);//potentialEnergy

	float sumM = 0;
	for (unsigned int i = 0; i < num; i++)sumM += m[i];

	float sigma = sqrt((2.0f * vr * abs(pe)) / (3.0f * sumM));

	for (unsigned int i = 0; i < num; i++){
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

	if (shift_px != 0.0f || shift_py != 0.0f || shift_pz != 0.0f && shift_vx != 0.0f || shift_vy != 0.0f || shift_vz != 0.0f){
		paramShift(num, shift_px, shift_py, shift_pz, shift_vx, shift_vy, shift_vz);
	}

	ptrShift(-(int)(size - num));
}

void particle::makePlummer(unsigned int num, float a, float mass, float shift_px, float shift_py, float shift_pz, float shift_vx, float shift_vy, float shift_vz){
	addParticle(num);
	ptrShift(size - num);

	float max = calcPlummerDensity(0.0f, a);
	std::random_device rd;
	std::mt19937_64 mt(rd());
	std::uniform_real_distribution<float> rand1(0, max);
	std::uniform_real_distribution<float> rand2(0, a * 2.0f);

	float radius, value, random;
	for (unsigned int i = 0; i < num; i++){
		do{
			radius = rand2(mt);
			value = calcPlummerDensity(radius, a);
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


	for (unsigned int i = 0; i < num; i++){
		m[i] = mass;
	}

	float sumM = 0;
	for (unsigned int i = 0; i < num; i++)sumM += m[i];


	for (unsigned int i = 0; i < num; i++){
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

	if (shift_px != 0.0f || shift_py != 0.0f || shift_pz != 0.0f && shift_vx != 0.0f || shift_vy != 0.0f || shift_vz != 0.0f){
		paramShift(num, shift_px, shift_py, shift_pz, shift_vx, shift_vy, shift_vz);
	}

	ptrShift(-(int)(size - num));
}

void particle::quickSort(unsigned int num, float *r, float *_px, float *_py, float *_pz){
	if (num <= 1)return;

	int i = 0, j = num - 1;
	while (r[0] == r[j]){
		if (i == j)return;
		j--;
	}

	float pivot = r[0] > r[1] ? r[0] : r[1];
	i = 0, j = num - 1;

	while(i != j){
		if (r[i] >= pivot){
			float tmp = r[i];
			r[i] = r[j];
			r[j] = tmp;

			tmp = _px[i];
			_px[i] = _px[j];
			_px[j] = tmp;

			tmp = _py[i];
			_py[i] = _py[j];
			_py[j] = tmp;

			tmp = _pz[i];
			_pz[i] = _pz[j];
			_pz[j] = tmp;
			j--;

		}
		else{
			i++;
		}
	}

	if (r[i] < pivot)i++;

	quickSort(i, r, _px, _py, _pz);
	quickSort(num - i, r + i, _px + i, _py + i, _pz + i);
}

void particle::makeMiyamotoNagai(unsigned int num, float a, float b, float mass, float shift_px, float shift_py, float shift_pz, float shift_vx, float shift_vy, float shift_vz){
	addParticle(num);
	ptrShift(size - num);

	float max = calcMiyamotoNagaiDensity(a, b, 0.0f, 0.0f);

	std::random_device rd;
	std::mt19937_64 mt(rd());
	std::uniform_real_distribution<float> rand1(0.0f, 1.0f);
	std::uniform_real_distribution<float> rand_x(a * -5.0f, a * 5.0f);
	std::uniform_real_distribution<float> rand_y(b * -3.0f, b * 3.0f);
	std::uniform_real_distribution<float> rand_z(a * -5.0f, a * 5.0f);

	float threshold = calcMiyamotoNagaiDensity(a, b, 0.0f, rand_x.b());
	
	#pragma omp parallel for
	for (long long int i = 0; i < (long long int)num; i++){
		float x, y, z, r, value, random;
		do{
			x = rand_x(mt);
			y = rand_y(mt);
			z = rand_z(mt);
			r = sqrt(x * x + z * z);

			value = calcMiyamotoNagaiDensity(a, b, y, r);

			random = rand1(mt);
		} while (random * max + threshold >= value || r > rand_x.b());
		px[i] = x;
		py[i] = y;
		pz[i] = z;
		vx[i] = 0;
		vy[i] = 0;
		vz[i] = 0;
	}


	float *dist = (float*)malloc(sizeof(float)* num);
	for (unsigned int i = 0; i < num; i++){
		dist[i] = sqrt(px[i] * px[i] + py[i] * py[i] + pz[i] * pz[i]);
	}


	quickSort(num, dist, px, py, pz);
	

	float sumM = 0;
	for (unsigned int i = 0; i < num; i++){
		m[i] = mass;
	}

	px[0] = 0;
	py[0] = 0;
	pz[0] = 0;
	vx[0] = 0;
	vy[0] = 0;
	vz[0] = 0;
	m[0] = mass * num * 3;
	sumM = m[0];

	for (unsigned int i = 1; i < num; i++){
		float v = sqrt(1.0f * G * sumM / (dist[i] + eps2));
		vx[i] = -pz[i] / dist[i] * v;
		vz[i] = px[i] / dist[i] * v;
		sumM += m[i];
	}	


	free(dist);

	if (shift_px != 0.0f || shift_py != 0.0f || shift_pz != 0.0f && shift_vx != 0.0f || shift_vy != 0.0f || shift_vz != 0.0f){
		paramShift(num, shift_px, shift_py, shift_pz, shift_vx, shift_vy, shift_vz);
	}

	ptrShift(-(int)(size - num));
}

inline float particle::calcPlummerDensity(float r, float a){
	return pow(1.0f + pow(r / a, 2), -5.0f / 2.0f);
}

inline float particle::calcMiyamotoNagaiDensity(float a, float b, float y, float r){
	return (a * pow(r, 2) + (a + 3.0f * sqrt(pow(y, 2) + pow(b, 2))) * pow((a + sqrt(pow(y, 2) + pow(b, 2))), 2)) / (pow((pow(r, 2) + pow((a + sqrt(pow(y, 2) + pow(b, 2))), 2)), 5.0f / 2.0f) * pow((pow(y, 2) + pow(b, 2)), 3.0f / 2.0f));
}


void particle::paramShift(unsigned int num, float shift_px, float shift_py, float shift_pz, float shift_vx, float shift_vy, float shift_vz){
	for (unsigned int i = 0; i < num; i++){
		px[i] += shift_px;
		py[i] += shift_py;
		pz[i] += shift_pz;
		vx[i] += shift_vx;
		vy[i] += shift_vy;
		vz[i] += shift_vz;
	}
}

void particle::addParticle(unsigned int num){
	float *tmp_px = (float*)_aligned_malloc(sizeof(float)* (size + num), 16);
	float *tmp_py = (float*)_aligned_malloc(sizeof(float)* (size + num), 16);
	float *tmp_pz = (float*)_aligned_malloc(sizeof(float)* (size + num), 16);
	float *tmp_vx = (float*)_aligned_malloc(sizeof(float)* (size + num), 16);
	float *tmp_vy = (float*)_aligned_malloc(sizeof(float)* (size + num), 16);
	float *tmp_vz = (float*)_aligned_malloc(sizeof(float)* (size + num), 16);
	float *tmp_ax = (float*)_aligned_malloc(sizeof(float)* (size + num), 16);
	float *tmp_ay = (float*)_aligned_malloc(sizeof(float)* (size + num), 16);
	float *tmp_az = (float*)_aligned_malloc(sizeof(float)* (size + num), 16);
	float *tmp_jx = (float*)_aligned_malloc(sizeof(float)* (size + num), 16);
	float *tmp_jy = (float*)_aligned_malloc(sizeof(float)* (size + num), 16);
	float *tmp_jz = (float*)_aligned_malloc(sizeof(float)* (size + num), 16);
	float *tmp_preax = (float*)_aligned_malloc(sizeof(float)* (size + num), 16);
	float *tmp_preay = (float*)_aligned_malloc(sizeof(float)* (size + num), 16);
	float *tmp_preaz = (float*)_aligned_malloc(sizeof(float)* (size + num), 16);
	float *tmp_prejx = (float*)_aligned_malloc(sizeof(float)* (size + num), 16);
	float *tmp_prejy = (float*)_aligned_malloc(sizeof(float)* (size + num), 16);
	float *tmp_prejz = (float*)_aligned_malloc(sizeof(float)* (size + num), 16);
	float *tmp_m = (float*)_aligned_malloc(sizeof(float)* (size + num), 16);
	unsigned char *tmp_texid = (unsigned char*)_aligned_malloc(sizeof(unsigned char)* (size + num), 8);
	unsigned int *tmp_node_num = (unsigned int*)_aligned_malloc(sizeof(unsigned int)* (size + num), 8);

	if (px != NULL){
		memcpy(tmp_px, px, sizeof(float)* size);
		memcpy(tmp_py, py, sizeof(float)* size);
		memcpy(tmp_pz, pz, sizeof(float)* size);
		memcpy(tmp_vx, vx, sizeof(float)* size);
		memcpy(tmp_vy, vy, sizeof(float)* size);
		memcpy(tmp_vz, vz, sizeof(float)* size);
		memcpy(tmp_ax, ax, sizeof(float)* size);
		memcpy(tmp_ay, ay, sizeof(float)* size);
		memcpy(tmp_az, az, sizeof(float)* size);
		memcpy(tmp_jx, jx, sizeof(float)* size);
		memcpy(tmp_jy, jy, sizeof(float)* size);
		memcpy(tmp_jz, jz, sizeof(float)* size);
		memcpy(tmp_preax, preax, sizeof(float)* size);
		memcpy(tmp_preay, preay, sizeof(float)* size);
		memcpy(tmp_preaz, preaz, sizeof(float)* size);
		memcpy(tmp_prejx, prejx, sizeof(float)* size);
		memcpy(tmp_prejy, prejy, sizeof(float)* size);
		memcpy(tmp_prejz, prejz, sizeof(float)* size);
		memcpy(tmp_m, m, sizeof(float)* size);
		memcpy(tmp_texid, texid, sizeof(unsigned char)* size);
		memcpy(tmp_node_num, node_num, sizeof(unsigned int)* size);


		_aligned_free(px);
		_aligned_free(py);
		_aligned_free(pz);
		_aligned_free(vx);
		_aligned_free(vy);
		_aligned_free(vz);
		_aligned_free(ax);
		_aligned_free(ay);
		_aligned_free(az);
		_aligned_free(jx);
		_aligned_free(jy);
		_aligned_free(jz);
		_aligned_free(preax);
		_aligned_free(preay);
		_aligned_free(preaz);
		_aligned_free(prejx);
		_aligned_free(prejy);
		_aligned_free(prejz);
		_aligned_free(m);
		_aligned_free(texid);
		_aligned_free(node_num);
	}

	px = tmp_px;
	py = tmp_py;
	pz = tmp_pz;
	vx = tmp_vx;
	vy = tmp_vy;
	vz = tmp_vz;
	ax = tmp_ax;
	ay = tmp_ay;
	az = tmp_az;
	jx = tmp_jx;
	jy = tmp_jy;
	jz = tmp_jz;
	preax = tmp_preax;
	preay = tmp_preay;
	preaz = tmp_preaz;
	prejx = tmp_prejx;
	prejy = tmp_prejy;
	prejz = tmp_prejz;
	m = tmp_m;
	texid = tmp_texid;
	node_num = tmp_node_num;

	size += num;
}

void particle::deleteAllParticle(){
	if (size == 0)return;

	_aligned_free(px);
	_aligned_free(py);
	_aligned_free(pz);
	_aligned_free(vx);
	_aligned_free(vy);
	_aligned_free(vz);
	_aligned_free(ax);
	_aligned_free(ay);
	_aligned_free(az);
	_aligned_free(jx);
	_aligned_free(jy);
	_aligned_free(jz);
	_aligned_free(preax);
	_aligned_free(preay);
	_aligned_free(preaz);
	_aligned_free(prejx);
	_aligned_free(prejy);
	_aligned_free(prejz);
	_aligned_free(m);
	_aligned_free(texid);
	_aligned_free(node_num);

	px = py = pz = vx = vy = vz = ax = ay = az = jx = jy = jz = m = preax = preay = preaz = prejx = prejy = prejz = NULL;
	texid = NULL;
	node_num = NULL;
	size = 0;
	bFirstCalculation = TRUE;
}

void particle::ptrShift(int offset){
	px += offset;
	py += offset;
	pz += offset;
	vx += offset;
	vy += offset;
	vz += offset;
	m += offset;
}

void particle::calcNbodyCuda(){
	
	//calcAcceleration(px, py, pz, vx, vy, vz, ax, ay, az, m, size);
	//calcEulerMethod(px, py, pz, vx, vy, vz, ax, ay, az, size);
	calcRungeKutta(px, py, pz, vx, vy, vz, ax, ay, az, m, size, eps2);
	/*if (bFirstCalculation){
		calcAcceleration(px, py, pz, vx, vy, vz, ax, ay, az, m, size);
		bFirstCalculation = FALSE;
	}
	calcLeapfrog(px, py, pz, vx, vy, vz, ax, ay, az, m, size, preax, preay, preaz);*/

	/*if (bFirstCalculation){
		calcAccelerationJerk(px, py, pz, vx, vy, vz, ax, ay, az, jx, jy, jz, m, size);
		bFirstCalculation = FALSE;
	}
	calcHermiteIntegration(px, py, pz, vx, vy, vz, ax, ay, az, jx, jy, jz, preax, preay, preaz, prejx, prejy, prejz, m, size);*/
}