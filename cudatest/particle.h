#ifndef particle_h
#define particle_h

//#define G 6.67408e-11
#define G 1.0f
#define M 1.0f
#define EPS 0.03125f

//#define dt 1.0f / 1024.0f
#define dt 1.0f / 1024.0f
#define PARTICLE_NUM 20480
#define THREADS 1024
#define rv 0.0

class particle{
public:
	float *px;
	float *py;
	float *pz;
	float *vx;
	float *vy;
	float *vz;
	float *ax;
	float *ay;
	float *az;
	float *jx;
	float *jy;
	float *jz;
	float *preax;
	float *preay;
	float *preaz;
	float *prejx;
	float *prejy;
	float *prejz;
	float *m;
	float eps2;
	unsigned char *texid;
	unsigned int *node_num;
	unsigned int size;
	bool bFirstCalculation;

	particle();
	~particle();
	void makeUniformSphere(unsigned int num, float radius, float mass, float vr, float shift_px, float shift_py, float shift_pz, float shift_vx, float shift_vy, float shift_vz);
	void makePlummer(unsigned int num, float a, float mass, float shift_px, float shift_py, float shift_pz, float shift_vx, float shift_vy, float shift_vz);
	void makeMiyamotoNagai(unsigned int num, float a, float b, float mass, float shift_px, float shift_py, float shift_pz, float shift_vx, float shift_vy, float shift_vz);
	inline float calcPlummerDensity(float r, float a);
	inline float calcMiyamotoNagaiDensity(float a, float b, float y, float r);
	void paramShift(unsigned int num, float shift_px, float shift_py, float shift_pz, float shift_vx, float shift_vy, float shift_vz);
	void deleteAllParticle();
	void calcNbodyCuda();
	void addParticle(unsigned int num);

private:
	void ptrShift(int offset);
	void quickSort(unsigned int num, float *r, float *_px, float *_py, float *_pz);
};
#endif