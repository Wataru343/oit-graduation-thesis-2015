#include <vector>
#include <nmmintrin.h>
#include "particle.h"

struct array_num
{
	unsigned int pnum[100000];
	unsigned int psize;
	unsigned int nnum[100000];
	unsigned int nsize;
};

struct node_array
{
	float center_x[4000000];
	float center_y[4000000];
	float center_z[4000000];
	float sum_mass[4000000];
	unsigned int nsize;
};

class node{
public:
	//float depth[15];
	float *posx;
	float *posy;
	float *posz;
	float *mass;
	unsigned int *num;
	unsigned int size;
	unsigned int capacity;
	node *node1;
	node *node2;
	node *node3;
	node *node4;
	node *node5;
	node *node6;
	node *node7;
	node *node8;
	float x, y, z, distance;
	float center_x, center_y, center_z, sum_mass;
	int depth;
	unsigned int node_num;
	unsigned int *particle_num;

	node();
	~node();
	void createCell(particle *cParticle);
	void divideCell(int a);
	void clearCell();
	void startThread(int a);
	inline void setCellVertex(float x, float y, float z, float distance);
	inline void node::memAlloc(unsigned int num);
	void sort(particle *cParticle);
	void getNodeNum();
};