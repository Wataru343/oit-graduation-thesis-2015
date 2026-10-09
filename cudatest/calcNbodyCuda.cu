#include <stdio.h>
#include <math.h>
#include <deque>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "particle.h"

std::deque<double> queue_gpu(5); 

/*__device__ int add(int a, int b){
int c = 0;
for (unsigned int i = 0; i < 10000; i++){

for (unsigned int j = 0; j < 10000; j++){
a *= i;
for (unsigned int k = 0; k < 0; k++){

}
}
}
return a + b;
}

__device__ int sub(int a, int b){
for (unsigned int i = 0; i < 10000; i++){

for (unsigned int j = 0; j < 10000; j++){
b *= i;
for (unsigned int k = 0; k < 0; k++){

}
}
}
return a - b;
}*/

__global__ void calcAccelerationCuda(float2 *px, float2 *py, float2 *pz, float2 *ax, float2 *ay, float2 *az, float2 *m, unsigned int num, float eps2){
	__shared__ float2 sh_px[THREADS], sh_py[THREADS], sh_pz[THREADS], sh_m[THREADS];
	unsigned int i = blockDim.x * blockIdx.x + threadIdx.x;
	float2 _px = px[i], _py = py[i], _pz = pz[i];
	float2 _ax = { 0, 0 }, _ay = { 0, 0 }, _az = { 0, 0 };
	ax[i] = ay[i] = az[i] = make_float2(0.0f, 0.0f);
	for (unsigned int j = 0; j < num; j += THREADS){
		sh_px[threadIdx.x] = px[threadIdx.x + j];
		sh_py[threadIdx.x] = py[threadIdx.x + j];
		sh_pz[threadIdx.x] = pz[threadIdx.x + j];
		sh_m[threadIdx.x] = m[threadIdx.x + j];

		__syncthreads();

		#pragma unroll
		for (unsigned int k = 0; k < THREADS; k++){
			float2 dx, dy, dz, r, a;
			dx.x = _px.x - sh_px[k].x;
			dx.y = _px.y - sh_px[k].y;

			dy.x = _py.x - sh_py[k].x;
			dy.y = _py.y - sh_py[k].y;

			dz.x = _pz.x - sh_pz[k].x;
			dz.y = _pz.y - sh_pz[k].y;

			r.x = rsqrt(eps2 + dx.x * dx.x + dy.x * dy.x + dz.x * dz.x);
			r.y = rsqrt(eps2 + dx.y * dx.y + dy.y * dy.y + dz.y * dz.y);

			a.x = G * r.x * r.x * r.x * sh_m[k].x;
			a.y = G * r.y * r.y * r.y * sh_m[k].y;

			_ax.x -= a.x * dx.x;
			_ax.y -= a.y * dx.y;

			_ay.x -= a.x * dy.x;
			_ay.y -= a.y * dy.y;

			_az.x -= a.x * dz.x;
			_az.y -= a.y * dz.y;


			dx.x = _px.x - sh_px[k].y;
			dx.y = _px.y - sh_px[k].x;

			dy.x = _py.x - sh_py[k].y;
			dy.y = _py.y - sh_py[k].x;

			dz.x = _pz.x - sh_pz[k].y;
			dz.y = _pz.y - sh_pz[k].x;

			r.x = rsqrt(eps2 + dx.x * dx.x + dy.x * dy.x + dz.x * dz.x);
			r.y = rsqrt(eps2 + dx.y * dx.y + dy.y * dy.y + dz.y * dz.y);

			a.x = G * r.x * r.x * r.x * sh_m[k].y;
			a.y = G * r.y * r.y * r.y * sh_m[k].x;

			_ax.x -= a.x * dx.x;
			_ax.y -= a.y * dx.y;

			_ay.x -= a.x * dy.x;
			_ay.y -= a.y * dy.y;

			_az.x -= a.x * dz.x;
			_az.y -= a.y * dz.y;
		}
	}
	ax[i] = _ax;
	ay[i] = _ay;
	az[i] = _az;
}


__global__ void calcAccelerationJerkCuda(float *px, float *py, float *pz, float *vx, float *vy, float *vz, float *ax, float *ay, float *az, float *jx, float *jy, float *jz, float *m, int num){
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	
	const float _px = px[i], _py = py[i], _pz = pz[i];
	const float _vx = vx[i], _vy = vy[i], _vz = vz[i];
	float _ax = 0, _ay = 0, _az = 0;
	float _jx = 0, _jy = 0, _jz = 0;

	for (int j = 0; j < num; j++){

		const float dx = _px - px[j];
		const float dy = _py - py[j];
		const float dz = _pz - pz[j];
		const float dvx = _vx - vx[j];
		const float dvy = _vy - vy[j];
		const float dvz = _vz - vz[j];

		const float r = rsqrt(EPS * EPS + dx * dx + dy * dy + dz * dz);
		const float r2 = r * r;
		const float a = G * m[j] * r2 * r;
		const float drdv = -3.0 * r2 * (dx * dvx + dy * dvy + dz * dvz);


		_ax -= a * dx;
		_ay -= a * dy;
		_az -= a * dz;
		_jx -= a * (dvx + drdv * dx);
		_jy -= a * (dvy + drdv * dy);
		_jz -= a * (dvz + drdv * dz);




		/*float dx = _px - px[j];
		float dy = _py - py[j];
		float dz = _pz - pz[j];
		float r = rsqrt(EPS * EPS + dx * dx + dy * dy + dz * dz);
		float a = G * m[j] * r * r * r;

		_ax -= a * dx;
		_ay -= a * dy;
		_az -= a * dz;

		float dvx = _vx - vx[j];
		float dvy = _vy - vy[j];
		float dvz = _vz - vz[j];
		float v = dx * dvx + dy * dvy + dz * dvz;

		_jx -= G * m[j] * ((dvx * r * r * r) - (3.0f * (v)* dx * r * r * r * r * r));
		_jy -= G * m[j] * ((dvy * r * r * r) - (3.0f * (v)* dy * r * r * r * r * r));
		_jz -= G * m[j] * ((dvz * r * r * r) - (3.0f * (v)* dz * r * r * r * r * r));*/
	}
	ax[i] = _ax;
	ay[i] = _ay;
	az[i] = _az;
	jx[i] = _jx;
	jy[i] = _jy;
	jz[i] = _jz;
}


void calcAcceleration(float *px, float *py, float *pz, float *vx, float *vy, float *vz, float *ax, float *ay, float *az, float *m, int num, float eps2){
	//処理時間計測用
	float elapsed_time_ms = 0.0f;
	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	cudaEventRecord(start, 0);
	for (int k = 0; k < num; k++){
		ax[k] = 0;
		ay[k] = 0;
		az[k] = 0;
	}

	dim3 block(num  >= THREADS * 2 ? num / THREADS / 2 : 1, 1, 1);
	dim3 thread(num >= THREADS * 2 ? THREADS : num, 1, 1);

	float *device_px;
	float *device_py;
	float *device_pz;
	float *device_ax;
	float *device_ay;
	float *device_az;
	float *device_m;

	cudaMalloc((void**)&device_px, sizeof(float)* num);
	cudaMalloc((void**)&device_py, sizeof(float)* num);
	cudaMalloc((void**)&device_pz, sizeof(float)* num);
	cudaMalloc((void**)&device_ax, sizeof(float)* num);
	cudaMalloc((void**)&device_ay, sizeof(float)* num);
	cudaMalloc((void**)&device_az, sizeof(float)* num);
	cudaMalloc((void**)&device_m, sizeof(float)* num);

	cudaMemcpy(device_px, px, sizeof(float)* num, cudaMemcpyHostToDevice);
	cudaMemcpy(device_py, py, sizeof(float)* num, cudaMemcpyHostToDevice);
	cudaMemcpy(device_pz, pz, sizeof(float)* num, cudaMemcpyHostToDevice);
	cudaMemcpy(device_ax, ax, sizeof(float)* num, cudaMemcpyHostToDevice);
	cudaMemcpy(device_ay, ay, sizeof(float)* num, cudaMemcpyHostToDevice);
	cudaMemcpy(device_az, az, sizeof(float)* num, cudaMemcpyHostToDevice);
	cudaMemcpy(device_m, m, sizeof(float)* num, cudaMemcpyHostToDevice);

	cudaFuncSetCacheConfig(calcAccelerationCuda, cudaFuncCachePreferEqual);
	
	calcAccelerationCuda <<<block, thread >>>((float2*)device_px, (float2*)device_py, (float2*)device_pz, (float2*)device_ax, (float2*)device_ay, (float2*)device_az, (float2*)device_m, num / 2, eps2);

	cudaMemcpy(ax, device_ax, sizeof(float)* num, cudaMemcpyDeviceToHost);
	cudaMemcpy(ay, device_ay, sizeof(float)* num, cudaMemcpyDeviceToHost);
	cudaMemcpy(az, device_az, sizeof(float)* num, cudaMemcpyDeviceToHost);
	cudaMemcpy(az, device_az, sizeof(float)* num, cudaMemcpyDeviceToHost);
	//cudaMemcpy(aaaa, arraya, sizeof(int)* num, cudaMemcpyDeviceToHost);

	//printf("%d\n", aaaa[0]);

	cudaFree(device_px);
	cudaFree(device_py);
	cudaFree(device_pz);
	cudaFree(device_ax);
	cudaFree(device_ay);
	cudaFree(device_az);
	cudaFree(device_m);

	//処理時間をqueueに入れる
	cudaEventRecord(stop, 0);
	cudaEventSynchronize(stop);
	cudaEventElapsedTime(&elapsed_time_ms, start, stop);
	queue_gpu.push_back(elapsed_time_ms);
	queue_gpu.pop_front();
	cudaEventDestroy(start);
	cudaEventDestroy(stop);
}

void calcAccelerationJerk(float *px, float *py, float *pz, float *vx, float *vy, float *vz, float *ax, float *ay, float *az, float *jx, float *jy, float *jz, float *m, int num){
	//処理時間計測用
	float elapsed_time_ms = 0.0f;
	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	cudaEventRecord(start, 0);

	for (int k = 0; k < num; k++){
		ax[k] = 0;
		ay[k] = 0;
		az[k] = 0;
		jx[k] = 0;
		jy[k] = 0;
		jz[k] = 0;
	}

	dim3 block(num / THREADS, 1, 1);
	dim3 thread(THREADS, 1, 1);

	float *device_px;
	float *device_py;
	float *device_pz;
	float *device_vx;
	float *device_vy;
	float *device_vz;
	float *device_ax;
	float *device_ay;
	float *device_az;
	float *device_jx;
	float *device_jy;
	float *device_jz;
	float *device_m;

	cudaMalloc((void**)&device_px, sizeof(float)* num);
	cudaMalloc((void**)&device_py, sizeof(float)* num);
	cudaMalloc((void**)&device_pz, sizeof(float)* num);
	cudaMalloc((void**)&device_vx, sizeof(float)* num);
	cudaMalloc((void**)&device_vy, sizeof(float)* num);
	cudaMalloc((void**)&device_vz, sizeof(float)* num);
	cudaMalloc((void**)&device_ax, sizeof(float)* num);
	cudaMalloc((void**)&device_ay, sizeof(float)* num);
	cudaMalloc((void**)&device_az, sizeof(float)* num);
	cudaMalloc((void**)&device_jx, sizeof(float)* num);
	cudaMalloc((void**)&device_jy, sizeof(float)* num);
	cudaMalloc((void**)&device_jz, sizeof(float)* num);
	cudaMalloc((void**)&device_m, sizeof(float)* num);
	
	cudaMemcpy(device_px, px, sizeof(float)* num, cudaMemcpyHostToDevice);
	cudaMemcpy(device_py, py, sizeof(float)* num, cudaMemcpyHostToDevice);
	cudaMemcpy(device_pz, pz, sizeof(float)* num, cudaMemcpyHostToDevice);
	cudaMemcpy(device_vx, vx, sizeof(float)* num, cudaMemcpyHostToDevice);
	cudaMemcpy(device_vy, vy, sizeof(float)* num, cudaMemcpyHostToDevice);
	cudaMemcpy(device_vz, vz, sizeof(float)* num, cudaMemcpyHostToDevice);
	cudaMemcpy(device_ax, ax, sizeof(float)* num, cudaMemcpyHostToDevice);
	cudaMemcpy(device_ay, ay, sizeof(float)* num, cudaMemcpyHostToDevice);
	cudaMemcpy(device_az, az, sizeof(float)* num, cudaMemcpyHostToDevice);
	cudaMemcpy(device_jx, jx, sizeof(float)* num, cudaMemcpyHostToDevice);
	cudaMemcpy(device_jy, jy, sizeof(float)* num, cudaMemcpyHostToDevice);
	cudaMemcpy(device_jz, jz, sizeof(float)* num, cudaMemcpyHostToDevice);
	cudaMemcpy(device_m, m, sizeof(float)* num, cudaMemcpyHostToDevice);

	calcAccelerationJerkCuda << <block, thread >> >(device_px, device_py, device_pz, device_vx, device_vy, device_vz, device_ax, device_ay, device_az, device_jx, device_jy, device_jz, device_m, num / 2);

	cudaMemcpy(ax, device_ax, sizeof(float)* num, cudaMemcpyDeviceToHost);
	cudaMemcpy(ay, device_ay, sizeof(float)* num, cudaMemcpyDeviceToHost);
	cudaMemcpy(az, device_az, sizeof(float)* num, cudaMemcpyDeviceToHost);
	cudaMemcpy(jx, device_jx, sizeof(float)* num, cudaMemcpyDeviceToHost);
	cudaMemcpy(jy, device_jy, sizeof(float)* num, cudaMemcpyDeviceToHost);
	cudaMemcpy(jz, device_jz, sizeof(float)* num, cudaMemcpyDeviceToHost);

	cudaFree(device_px);
	cudaFree(device_py);
	cudaFree(device_pz);
	cudaFree(device_vx);
	cudaFree(device_vy);
	cudaFree(device_vz);
	cudaFree(device_ax);
	cudaFree(device_ay);
	cudaFree(device_az);
	cudaFree(device_jx);
	cudaFree(device_jy);
	cudaFree(device_jz);
	cudaFree(device_m);


	//処理時間をqueueに入れる
	cudaEventRecord(stop, 0);
	cudaEventSynchronize(stop);
	cudaEventElapsedTime(&elapsed_time_ms, start, stop);
	queue_gpu.push_back(elapsed_time_ms);
	queue_gpu.pop_front();
	cudaEventDestroy(start);
	cudaEventDestroy(stop);
}

void calcEulerMethod(float *px, float *py, float *pz, float *vx, float *vy, float *vz, float *ax, float *ay, float *az, float *m, int num, float eps2){
	calcAcceleration(px, py, pz, vx, vy, vz, ax, ay, az, m, num, eps2);
	for (int i = 0; i < num; i++){
		px[i] += vx[i] * dt;
		py[i] += vy[i] * dt;
		pz[i] += vz[i] * dt;

		vx[i] += ax[i] * dt;
		vy[i] += ay[i] * dt;
		vz[i] += az[i] * dt;

	}
}

void calcRungeKutta(float *px, float *py, float *pz, float *vx, float *vy, float *vz, float *ax, float *ay, float *az, float *m, int num, float eps2){
	calcAcceleration(px, py, pz, vx, vy, vz, ax, ay, az, m, num, eps2);

	for (int i = 0; i < num; i++){
		float kx1, kx2, kx3, kx4, ky1, ky2, ky3, ky4, kz1, kz2, kz3, kz4;

		vx[i] += ax[i] * dt;
		vy[i] += ay[i] * dt;
		vz[i] += az[i] * dt;

		kx1 = vx[i];
		kx2 = (vx[i] + kx1 / 2.0f  * dt);
		kx3 = (vx[i] + kx2 / 2.0f * dt);
		kx4 = (vx[i] + kx3 * dt);

		px[i] += (kx1 + 2.0f * kx2 + 2.0f * kx3 + kx4) / 6.0f * dt;

		ky1 = vy[i];
		ky2 = (vy[i] + ky1 / 2.0f * dt);
		ky3 = (vy[i] + ky2 / 2.0f * dt);
		ky4 = (vy[i] + ky3 * dt);

		py[i] += (ky1 + 2.0f * ky2 + 2.0f * ky3 + ky4) / 6.0f * dt;

		kz1 = vz[i];
		kz2 = (vz[i] + kz1 / 2.0f * dt);
		kz3 = (vz[i] + kz2 / 2.0f * dt);
		kz4 = (vz[i] + kz3 * dt);

		pz[i] += (kz1 + 2.0f * kz2 + 2.0f * kz3 + kz4) / 6.0f * dt;
	}
}

void calcLeapfrog(float *px, float *py, float *pz, float *vx, float *vy, float *vz, float *ax, float *ay, float *az, float *m, int num, float eps2, float *preax, float *preay, float *preaz){
	for (int i = 0; i < num; i++){
		px[i] += vx[i] * dt + ax[i] * dt * dt * 0.5f;
		py[i] += vy[i] * dt + ay[i] * dt * dt * 0.5f;
		pz[i] += vz[i] * dt + az[i] * dt * dt * 0.5f;
	}

	memcpy(preax, ax, sizeof(float)* num);
	memcpy(preay, ay, sizeof(float)* num);
	memcpy(preaz, az, sizeof(float)* num);

	calcAcceleration(px, py, pz, vx, vy, vz, ax, ay, az, m, num, eps2);

	for (int i = 0; i < num; i++){
		vx[i] += (preax[i] + ax[i]) * dt * 0.5f;
		vy[i] += (preay[i] + ay[i]) * dt * 0.5f;
		vz[i] += (preaz[i] + az[i]) * dt * 0.5f;
	}
}

void calcHermiteIntegration(float *px, float *py, float *pz, float *vx, float *vy, float *vz, float *ax, float *ay, float *az, float *jx, float *jy, float *jz, float *preax, float *preay, float *preaz, float *prejx, float *prejy, float *prejz, float *m, int num){
	memcpy(preax, ax, sizeof(float)* num);
	memcpy(preay, ay, sizeof(float)* num);
	memcpy(preaz, az, sizeof(float)* num);
	memcpy(prejx, jx, sizeof(float)* num);
	memcpy(prejy, jy, sizeof(float)* num);
	memcpy(prejz, jz, sizeof(float)* num);
	calcAccelerationJerk(px, py, pz, vx, vy, vz, ax, ay, az, jx, jy, jz, m, num);

	for (int i = 0; i < num; i++){
		px[i] += vx[i] * dt + ax[i] * dt * dt / 2.0f + jx[i] * dt * dt * dt / 6.0f;
		py[i] += vy[i] * dt + ay[i] * dt * dt / 2.0f + jy[i] * dt * dt * dt / 6.0f;
		pz[i] += vz[i] * dt + az[i] * dt * dt / 2.0f + jz[i] * dt * dt * dt / 6.0f;
		pz[i] = 0;

		vx[i] += ax[i] * dt + jx[i] * dt * dt / 2.0f;
		vy[i] += ay[i] * dt + jy[i] * dt * dt / 2.0f;
		vz[i] += az[i] * dt + jz[i] * dt * dt / 2.0f;

		/*float a2x = (-6.0f * (preax[i] - ax[i]) - (4.0f * prejx[i] + 2.0f * jx[i]) * dt) / (dt * dt);
		float a3x = (12.0f * (preax[i] - ax[i]) + 6.0f * (prejx[i] + 2.0f * jx[i]) * dt) / (dt * dt * dt);
		float a2y = (-6.0f * (preay[i] - ay[i]) - (4.0f * prejy[i] + 2.0f * jy[i]) * dt) / (dt * dt);
		float a3y = (12.0f * (preay[i] - ay[i]) + 6.0f * (prejy[i] + 2.0f * jy[i]) * dt) / (dt * dt * dt);
		float a2z = (-6.0f * (preaz[i] - az[i]) - (4.0f * prejz[i] + 2.0f * jz[i]) * dt) / (dt * dt);
		float a3z = (12.0f * (preaz[i] - az[i]) + 6.0f * (prejz[i] + 2.0f * jz[i]) * dt) / (dt * dt * dt);

		px[i] += vx[i] * dt + preax[i] / 2.0f * dt * dt + prejx[i] / 6.0f * dt *dt * dt;
		py[i] += vy[i] * dt + preay[i] / 2.0f * dt * dt + prejy[i] / 6.0f * dt *dt * dt;
		pz[i] += vz[i] * dt + preaz[i] / 2.0f * dt * dt + prejz[i] / 6.0f * dt *dt * dt;
		//printf("ax:%f ay:%f az:%f\n", preax[i], preay[i], preaz[i]);

		vx[i] += preax[i] * dt + prejx[i] / 2.0f * dt * dt;
		vy[i] += preay[i] * dt + prejy[i] / 2.0f * dt * dt;
		vz[i] += preaz[i] * dt + prejz[i] / 2.0f * dt * dt;




		px[i] += a2x / 24.0f * dt * dt * dt * dt + a3x / 120.0f * dt * dt * dt * dt * dt;
		py[i] += a2y / 24.0f * dt * dt * dt * dt + a3y / 120.0f * dt * dt * dt * dt * dt;
		pz[i] += a2z / 24.0f * dt * dt * dt * dt + a3z / 120.0f * dt * dt * dt * dt * dt;

		vx[i] += a2x / 6.0f * dt * dt * dt * dt + a3x / 24.0f * dt * dt * dt * dt;
		vy[i] += a2y / 6.0f * dt * dt * dt * dt + a3y / 24.0f * dt * dt * dt * dt;
		vz[i] += a2z / 6.0f * dt * dt * dt * dt + a3z / 24.0f * dt * dt * dt * dt;*/
		//printf("%f\n", ax[i] - (preax[i] + prejx[i] * dt + a2x * dt * dt / 2.0f + a3x * dt * dt * dt / 6.0f));
	}


}