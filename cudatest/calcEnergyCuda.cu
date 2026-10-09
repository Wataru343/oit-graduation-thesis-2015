#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "particle.h"


__global__ void calcPotentialEnergy(float *px, float *py, float *pz, float *m, unsigned int particleNum, float *pe){
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	pe[i] = 0.0;
	for (unsigned int j = i + 1; j < particleNum; j++){
		float dx = px[i] - px[j];
		float dy = py[i] - py[j];
		float dz = pz[i] - pz[j];
		float r = rsqrt(dx * dx + dy * dy + dz * dz + EPS * EPS);
		//float r = sqrt(__fadd_rn(__fadd_rn(__fadd_rn(__fmul_rn(dx, dx), __fmul_rn(dy, dy)), __fmul_rn(dz, dz)), __fmul_rn(EPS, EPS)));

		pe[i] -= G * m[i] * m[j] * r;
	}
	__syncthreads();
}

float calcPotentialEnergyCuda(float *px, float *py, float *pz, float *m, unsigned int particleNum){
	dim3 block(particleNum >= THREADS ? particleNum / THREADS : 1, 1, 1);
	dim3 thread(particleNum >= THREADS ? THREADS : particleNum, 1, 1);

	float *dstPe = (float*)_aligned_malloc(sizeof(float)* particleNum, 16);

	float *device_px;
	float *device_py;
	float *device_pz;
	float *device_m;
	float *device_pe;

	cudaMalloc((void**)&device_px, sizeof(float)* particleNum);
	cudaMalloc((void**)&device_py, sizeof(float)* particleNum);
	cudaMalloc((void**)&device_pz, sizeof(float)* particleNum);
	cudaMalloc((void**)&device_m, sizeof(float)* particleNum);
	cudaMalloc((void**)&device_pe, sizeof(float)* particleNum);

	cudaMemcpy(device_px, px, sizeof(float)* particleNum, cudaMemcpyHostToDevice);
	cudaMemcpy(device_py, py, sizeof(float)* particleNum, cudaMemcpyHostToDevice);
	cudaMemcpy(device_pz, pz, sizeof(float)* particleNum, cudaMemcpyHostToDevice);
	cudaMemcpy(device_m, m, sizeof(float)* particleNum, cudaMemcpyHostToDevice);
	cudaMemcpy(device_pe, dstPe, sizeof(float)* particleNum, cudaMemcpyHostToDevice);

	calcPotentialEnergy<<<block, thread >>>(device_px, device_py, device_pz, device_m, particleNum, device_pe);

	cudaMemcpy(dstPe, device_pe, sizeof(float)* particleNum, cudaMemcpyDeviceToHost);

	cudaFree(device_px);
	cudaFree(device_py);
	cudaFree(device_pz);
	cudaFree(device_m);
	cudaFree(device_pe);

	float pe = 0;
	for (unsigned int i = 0; i < particleNum; i++)pe += dstPe[i];

	_aligned_free(dstPe);

	return pe;
}

float calcKineticEnergyCuda(float *px, float *py, float *pz, float *vx, float *vy, float *vz, float *m, unsigned int particleNum){
	float ke = 0;//kineticEnergy
	for (unsigned int i = 0; i < particleNum; i++){
		ke += 0.5f * m[i] * (vx[i] * vx[i] + vy[i] * vy[i] + vz[i] * vz[i]);
	}

	return ke;
}


