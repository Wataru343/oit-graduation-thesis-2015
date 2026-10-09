#include "node.h"
#include <float.h>
#include <stdio.h>
#include <thread>
int count = 0;
int maxnum = 0;

array_num aNum[8];
node_array nodeArray;

node::node(){
	node1 = node2 = node3 = node4 = node5 = node6 = node7 = node8= NULL;
	posx = posy = posz = mass = NULL;
	depth = 0;
	x = y = z = distance = 0;
	center_x = center_y = center_z = sum_mass = 0;
	size = 0;
	capacity = 0;
}

node::~node(){
	if (node1 == NULL)return;
	if (depth != 0 && size > 0){
		_mm_free(posx);
		_mm_free(num);

	}
	delete node1;
	delete node2;
	delete node3;
	delete node4;
	delete node5;
	delete node6;
	delete node7;
	delete node8;
}

void node::createCell(particle *cParticle){
	//for (int i = 0; i < 33; i++)posx.push_back(0.0f);
	size = cParticle->size;
	depth = 0;

	float xmin = FLT_MAX, xmax = -FLT_MAX, ymin = FLT_MAX, ymax = -FLT_MAX, zmin = FLT_MAX, zmax = -FLT_MAX;

	if (node1 == NULL){
		node1 = new node;
		node1->depth = depth + 1;
		node1->node_num = 1;
		node2 = new node;
		node2->depth = depth + 1;
		node2->node_num = 2;
		node3 = new node;
		node3->depth = depth + 1;
		node3->node_num = 3;
		node4 = new node;
		node4->depth = depth + 1;
		node4->node_num = 4;
		node5 = new node;
		node5->depth = depth + 1;
		node5->node_num = 5;
		node6 = new node;
		node6->depth = depth + 1;
		node6->node_num = 6;
		node7 = new node;
		node7->depth = depth + 1;
		node7->node_num = 7;
		node8 = new node;
		node8->depth = depth + 1;
		node8->node_num = 8;
	}


	for (unsigned int i = 0; i < cParticle->size; i++){
		xmin = xmin < cParticle->px[i] ? xmin : cParticle->px[i];
		xmax = xmax > cParticle->px[i] ? xmax : cParticle->px[i];
		ymin = ymin < cParticle->py[i] ? ymin : cParticle->py[i];
		ymax = ymax > cParticle->py[i] ? ymax : cParticle->py[i];
		zmin = zmin < cParticle->pz[i] ? zmin : cParticle->pz[i];
		zmax = zmax > cParticle->pz[i] ? zmax : cParticle->pz[i];
	}

	x = xmin;
	y = ymin;
	z = zmin;
	distance = xmax - xmin > ymax - ymin ? xmax - xmin : ymax - ymin;
	distance = distance > zmax - zmin ? distance : zmax - zmin;


	setCellVertex(x, y, z, distance * 0.5f);

	node1->memAlloc(size);
	node2->memAlloc(size);
	node3->memAlloc(size);
	node4->memAlloc(size);
	node5->memAlloc(size);
	node6->memAlloc(size);
	node7->memAlloc(size);
	node8->memAlloc(size);

	for (unsigned int i = 0; i < cParticle->size; i++){
		if (cParticle->m[i] <= 0.0){
			//printf("%f\n", cParticle->m[i]);
			continue;
		}
		if (cParticle->px[i] <= node2->x){
			if (cParticle->py[i] <= node3->y){
				if (cParticle->pz[i] <= node5->z){
					node1->posx[node1->size] = cParticle->px[i];
					node1->posy[node1->size] = cParticle->py[i];
					node1->posz[node1->size] = cParticle->pz[i];
					node1->mass[node1->size] = cParticle->m[i];
					node1->num[node1->size] = i;
					node1->size++;
				}
				else{
					node5->posx[node5->size] = cParticle->px[i];
					node5->posy[node5->size] = cParticle->py[i];
					node5->posz[node5->size] = cParticle->pz[i];
					node5->mass[node5->size] = cParticle->m[i];
					node5->num[node5->size] = i;
					node5->size++;
				}				
			}
			else{
				if (cParticle->pz[i] <= node5->z){
					node4->posx[node4->size] = cParticle->px[i];
					node4->posy[node4->size] = cParticle->py[i];
					node4->posz[node4->size] = cParticle->pz[i];
					node4->mass[node4->size] = cParticle->m[i];
					node4->num[node4->size] = i;
					node4->size++;
				}
				else{
					node8->posx[node8->size] = cParticle->px[i];
					node8->posy[node8->size] = cParticle->py[i];
					node8->posz[node8->size] = cParticle->pz[i];
					node8->mass[node8->size] = cParticle->m[i];
					node8->num[node8->size] = i;
					node8->size++;
				}
			}
		}
		else{
			if (cParticle->py[i] <= node3->y){
				if (cParticle->pz[i] <= node5->z){
					node2->posx[node2->size] = cParticle->px[i];
					node2->posy[node2->size] = cParticle->py[i];
					node2->posz[node2->size] = cParticle->pz[i];
					node2->mass[node2->size] = cParticle->m[i];
					node2->num[node2->size] = i;
					node2->size++;
				}
				else{
					node6->posx[node6->size] = cParticle->px[i];
					node6->posy[node6->size] = cParticle->py[i];
					node6->posz[node6->size] = cParticle->pz[i];
					node6->mass[node6->size] = cParticle->m[i];
					node6->num[node6->size] = i;
					node6->size++;
				}
			}
			else{
				if (cParticle->pz[i] <= node5->z){
					node3->posx[node3->size] = cParticle->px[i];
					node3->posy[node3->size] = cParticle->py[i];
					node3->posz[node3->size] = cParticle->pz[i];
					node3->mass[node3->size] = cParticle->m[i];
					node3->num[node3->size] = i;
					node3->size++;
				}
				else{
					node7->posx[node7->size] = cParticle->px[i];
					node7->posy[node7->size] = cParticle->py[i];
					node7->posz[node7->size] = cParticle->pz[i];
					node7->mass[node7->size] = cParticle->m[i];
					node7->num[node7->size] = i;
					node7->size++;
				}
			}
		}
	}
	//printf("\rnodes:%d  depth:%d   ", count, maxnum);
	count = 1;
	maxnum = 0;
	aNum[0].psize = aNum[1].psize = aNum[2].psize = aNum[3].psize = aNum[4].psize = aNum[5].psize = aNum[6].psize = aNum[7].psize = 0;
	nodeArray.nsize = 0;
	std::thread t1(&node::divideCell, node1, 0);
	std::thread t2(&node::divideCell, node2, 1);
	std::thread t3(&node::divideCell, node3, 2);
	std::thread t4(&node::divideCell, node4, 3);
	std::thread t5(&node::divideCell, node5, 4);
	std::thread t6(&node::divideCell, node6, 5);
	std::thread t7(&node::divideCell, node7, 6);
	std::thread t8(&node::divideCell, node8, 7);
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	t6.join();
	t7.join();
	t8.join();
	//getNodeNum();
	/*node1->divideCell();
	node2->divideCell();
	node3->divideCell();
	node4->divideCell();
	node5->divideCell();
	node6->divideCell();
	node7->divideCell();
	node8->divideCell();*/
}

void node::startThread(int a){
	divideCell(a);
}

void node::divideCell(int a){
	if (size != 0)count++;
	//printf("%d\n", size);
	if (size <= 1){		
		if (size == 0){
			_mm_free(posx);
			_mm_free(num);
			posx = posy = posz = mass = NULL;
			return;
		}

		for (unsigned int i = 0; i < size; i++){
			center_x += posx[i] * mass[i];
			center_y += posy[i] * mass[i];
			center_z += posz[i] * mass[i];
			sum_mass += mass[i];
			aNum[a].pnum[aNum[a].psize] = num[i];
			aNum[a].nnum[aNum[a].psize] = node_num;
			aNum[a].psize++;			
		}
		
		//aNum[a].nsize++;

		/*for (unsigned int i = 0; i < 32 - size; i++){
			aNum[a].pnum[aNum[a].psize] = -1;
			aNum[a].nnum[aNum[a].psize] = node_num;
			aNum[a].psize++;
		}*/

		center_x /= sum_mass;
		center_y /= sum_mass;
		center_z /= sum_mass;

		_mm_free(posx);
		_mm_free(num);
		posx = posy = posz = mass = NULL;
		//printf("%f %f %f %f\n", center_x, center_y, center_z, sum_mass);
		return;
	}

	if (node1 == NULL){
		node1 = new node;
		node1->depth = depth + 1;
		node1->node_num = node_num * 8 + 1;
		node2 = new node;
		node2->depth = depth + 1;
		node2->node_num = node1->node_num + 1;
		node3 = new node;
		node3->depth = depth + 1;
		node3->node_num = node1->node_num + 2;
		node4 = new node;
		node4->depth = depth + 1;
		node4->node_num = node1->node_num + 3;
		node5 = new node;
		node5->depth = depth + 1;
		node5->node_num = node1->node_num + 4;
		node6 = new node;
		node6->depth = depth + 1;
		node6->node_num = node1->node_num + 5;
		node7 = new node;
		node7->depth = depth + 1;
		node7->node_num = node1->node_num + 6;
		node8 = new node;
		node8->depth = depth + 1;
		node8->node_num = node1->node_num + 7;
	}

	maxnum = maxnum > depth ? maxnum : depth;

	setCellVertex(x, y, z, distance * 0.5f);

	
	node1->memAlloc(size);
	node2->memAlloc(size);
	node3->memAlloc(size);
	node4->memAlloc(size);
	node5->memAlloc(size);
	node6->memAlloc(size);
	node7->memAlloc(size);
	node8->memAlloc(size);

	for (register unsigned int i = 0; i < size; i++){
		if (posx[i] <= node2->x){
			if (posy[i] <= node3->y){
				if (posz[i] <= node5->z){
					node1->posx[node1->size] = posx[i];
					node1->posy[node1->size] = posy[i];
					node1->posz[node1->size] = posz[i];
					node1->mass[node1->size] = mass[i];
					node1->num[node1->size] = num[i];
					node1->size++;
				}
				else{
					node5->posx[node5->size] = posx[i];
					node5->posy[node5->size] = posy[i];
					node5->posz[node5->size] = posz[i];
					node5->mass[node5->size] = mass[i];
					node5->num[node5->size] = num[i];
					node5->size++;
				}
			}
			else{
				if (posz[i] <= node5->z){
					node4->posx[node4->size] = posx[i];
					node4->posy[node4->size] = posy[i];
					node4->posz[node4->size] = posz[i];
					node4->mass[node4->size] = mass[i];
					node4->num[node4->size] = num[i];
					node4->size++;
				}
				else{
					node8->posx[node8->size] = posx[i];
					node8->posy[node8->size] = posy[i];
					node8->posz[node8->size] = posz[i];
					node8->mass[node8->size] = mass[i];
					node8->num[node8->size] = num[i];
					node8->size++;
				}
			}
		}
		else{
			if (posy[i] <= node3->y){
				if (posz[i] <= node5->z){
					node2->posx[node2->size] = posx[i];
					node2->posy[node2->size] = posy[i];
					node2->posz[node2->size] = posz[i];
					node2->mass[node2->size] = mass[i];
					node2->num[node2->size] = num[i];
					node2->size++;
				}
				else{
					node6->posx[node6->size] = posx[i];
					node6->posy[node6->size] = posy[i];
					node6->posz[node6->size] = posz[i];
					node6->mass[node6->size] = mass[i];
					node6->num[node6->size] = num[i];
					node6->size++;
				}
			}
			else{
				if (posz[i] <= node5->z){
					node3->posx[node3->size] = posx[i];
					node3->posy[node3->size] = posy[i];
					node3->posz[node3->size] = posz[i];
					node3->mass[node3->size] = mass[i];
					node3->num[node3->size] = num[i];
					node3->size++;
				}
				else{
					node7->posx[node7->size] = posx[i];
					node7->posy[node7->size] = posy[i];
					node7->posz[node7->size] = posz[i];
					node7->mass[node7->size] = mass[i];
					node7->num[node7->size] = num[i];
					node7->size++;
				}
			}
		}
	}
	
	node1->divideCell(a);
	node2->divideCell(a);
	node3->divideCell(a);
	node4->divideCell(a);
	node5->divideCell(a);
	node6->divideCell(a);
	node7->divideCell(a);
	node8->divideCell(a);

	register double sumx = 0, sumy = 0, sumz = 0, summ = 0;

	summ += (double)node1->sum_mass;
	summ += (double)node2->sum_mass;
	summ += (double)node3->sum_mass;
	summ += (double)node4->sum_mass;
	summ += (double)node5->sum_mass;
	summ += (double)node6->sum_mass;
	summ += (double)node7->sum_mass;
	summ += (double)node8->sum_mass;
	sum_mass = (float)summ;

	sumx += (double)node1->center_x * (double)node1->sum_mass;
	sumx += (double)node2->center_x * (double)node2->sum_mass;
	sumx += (double)node3->center_x * (double)node3->sum_mass;
	sumx += (double)node4->center_x * (double)node4->sum_mass;
	sumx += (double)node5->center_x * (double)node5->sum_mass;
	sumx += (double)node6->center_x * (double)node6->sum_mass;
	sumx += (double)node7->center_x * (double)node7->sum_mass;
	sumx += (double)node8->center_x * (double)node8->sum_mass;
	center_x = (float)(sumx / summ);

	sumy += (double)node1->center_y * (double)node1->sum_mass;
	sumy += (double)node2->center_y * (double)node2->sum_mass;
	sumy += (double)node3->center_y * (double)node3->sum_mass;
	sumy += (double)node4->center_y * (double)node4->sum_mass;
	sumy += (double)node5->center_y * (double)node5->sum_mass;
	sumy += (double)node6->center_y * (double)node6->sum_mass;
	sumy += (double)node7->center_y * (double)node7->sum_mass;
	sumy += (double)node8->center_y * (double)node8->sum_mass;
	center_y = (float)(sumy / summ);


	sumz += (double)node1->center_z * (double)node1->sum_mass;
	sumz += (double)node2->center_z * (double)node2->sum_mass;
	sumz += (double)node3->center_z * (double)node3->sum_mass;
	sumz += (double)node4->center_z * (double)node4->sum_mass;
	sumz += (double)node5->center_z * (double)node5->sum_mass;
	sumz += (double)node6->center_z * (double)node6->sum_mass;
	sumz += (double)node7->center_z * (double)node7->sum_mass;
	sumz += (double)node8->center_z * (double)node8->sum_mass;
	center_z = (float)(sumz / summ);

	/*nodeArray.center_x[node_num] = center_x;
	nodeArray.center_y[node_num] = center_y;
	nodeArray.center_z[node_num] = center_z;
	nodeArray.sum_mass[node_num] = sum_mass;*/

	_mm_free(posx);
	_mm_free(num);

	posx = posy = posz = mass = NULL;
	capacity = 0;
}

void node::clearCell(){
	size = 0;
	center_x = center_y = center_z = sum_mass = 0;

	if (node1 != NULL){
		node1->clearCell();
		node2->clearCell();
		node3->clearCell();
		node4->clearCell();
		node5->clearCell();
		node6->clearCell();
		node7->clearCell();
		node8->clearCell();
	}
	else{
		posx = posy = posz = mass = NULL;
	}
}

inline void node::setCellVertex(float x, float y, float z, float distance){
	node1->x = x;
	node1->y = y;
	node1->z = z;
	node1->distance = distance;

	node2->x = x + distance;
	node2->y = y;
	node2->z = z;
	node2->distance = distance;

	node3->x = x + distance;
	node3->y = y + distance;
	node3->z = z;
	node3->distance = distance;

	node4->x = x;
	node4->y = y + distance;
	node4->z = z;
	node4->distance = distance;

	node5->x = x;
	node5->y = y;
	node5->z = z + distance;
	node5->distance = distance;

	node6->x = x + distance;
	node6->y = y;
	node6->z = z + distance;
	node6->distance = distance;

	node7->x = x + distance;
	node7->y = y + distance;
	node7->z = z + distance;
	node7->distance = distance;

	node8->x = x;
	node8->y = y + distance;
	node8->z = z + distance;
	node8->distance = distance;
}

inline void node::memAlloc(unsigned int _num){
	if (posx == NULL){
		float *tmp = (float*)_mm_malloc(sizeof(float)* _num * 4, 32);
		posx = tmp;
		posy = tmp + _num;
		posz = tmp + _num * 2;
		mass = tmp + _num * 3;
		capacity = _num;

		num = (unsigned int*)_mm_malloc(sizeof(unsigned int)* _num, 32);
	}
}

void node::sort(particle *cParticle){
	unsigned int psize = aNum[0].psize + aNum[1].psize + aNum[2].psize + aNum[3].psize + aNum[4].psize + aNum[5].psize + aNum[6].psize + aNum[7].psize;

	//printf("%d ", psize);
	float *tmp_px = (float*)_aligned_malloc(sizeof(float)* (psize), 32);
	float *tmp_py = (float*)_aligned_malloc(sizeof(float)* (psize), 32);
	float *tmp_pz = (float*)_aligned_malloc(sizeof(float)* (psize), 32);
	float *tmp_vx = (float*)_aligned_malloc(sizeof(float)* (psize), 32);
	float *tmp_vy = (float*)_aligned_malloc(sizeof(float)* (psize), 32);
	float *tmp_vz = (float*)_aligned_malloc(sizeof(float)* (psize), 32);
	float *tmp_ax = (float*)_aligned_malloc(sizeof(float)* (psize), 32);
	float *tmp_ay = (float*)_aligned_malloc(sizeof(float)* (psize), 32);
	float *tmp_az = (float*)_aligned_malloc(sizeof(float)* (psize), 32);
	float *tmp_jx = (float*)_aligned_malloc(sizeof(float)* (psize), 32);
	float *tmp_jy = (float*)_aligned_malloc(sizeof(float)* (psize), 32);
	float *tmp_jz = (float*)_aligned_malloc(sizeof(float)* (psize), 32);
	float *tmp_preax = (float*)_aligned_malloc(sizeof(float)* (psize), 32);
	float *tmp_preay = (float*)_aligned_malloc(sizeof(float)* (psize), 32);
	float *tmp_preaz = (float*)_aligned_malloc(sizeof(float)* (psize), 32);
	float *tmp_prejx = (float*)_aligned_malloc(sizeof(float)* (psize), 32);
	float *tmp_prejy = (float*)_aligned_malloc(sizeof(float)* (psize), 32);
	float *tmp_prejz = (float*)_aligned_malloc(sizeof(float)* (psize), 32);
	float *tmp_m = (float*)_aligned_malloc(sizeof(float)* (psize), 32);
	unsigned char *tmp_texid = (unsigned char*)_aligned_malloc(sizeof(unsigned char)* (psize), 8);
	unsigned int *tmp_node_num = (unsigned int*)_aligned_malloc(sizeof(unsigned int)* (psize), 8);

	int k = 0;
	
	for (unsigned int i = 0; i < 8; i++){
		for (unsigned int j = 0; j < aNum[i].psize; j++, k++){
			//printf("%d\n", aNum[i].nnum[j]);
			if (aNum[i].pnum[j] == -1){
				tmp_m[k] = 0;
				tmp_node_num[k] = aNum[i].nnum[j];
				continue;
			}
			tmp_px[k] = cParticle->px[aNum[i].pnum[j]];
			tmp_py[k] = cParticle->py[aNum[i].pnum[j]];
			tmp_pz[k] = cParticle->pz[aNum[i].pnum[j]];
			tmp_vx[k] = cParticle->vx[aNum[i].pnum[j]];
			tmp_vy[k] = cParticle->vy[aNum[i].pnum[j]];
			tmp_vz[k] = cParticle->vz[aNum[i].pnum[j]];
			tmp_ax[k] = cParticle->ax[aNum[i].pnum[j]];
			tmp_ay[k] = cParticle->ay[aNum[i].pnum[j]];
			tmp_az[k] = cParticle->az[aNum[i].pnum[j]];
			tmp_jx[k] = cParticle->jx[aNum[i].pnum[j]];
			tmp_jy[k] = cParticle->jy[aNum[i].pnum[j]];
			tmp_jz[k] = cParticle->jz[aNum[i].pnum[j]];
			tmp_preax[k] = cParticle->preax[aNum[i].pnum[j]];
			tmp_preay[k] = cParticle->preay[aNum[i].pnum[j]];
			tmp_preaz[k] = cParticle->preaz[aNum[i].pnum[j]];
			tmp_prejx[k] = cParticle->prejx[aNum[i].pnum[j]];
			tmp_prejy[k] = cParticle->prejy[aNum[i].pnum[j]];
			tmp_prejz[k] = cParticle->prejz[aNum[i].pnum[j]];
			tmp_m[k] = cParticle->m[aNum[i].pnum[j]];
			tmp_texid[k] = cParticle->texid[aNum[i].pnum[j]];
			tmp_node_num[k] = aNum[i].nnum[j];
		}
	}

	_aligned_free(cParticle->px);
	_aligned_free(cParticle->py);
	_aligned_free(cParticle->pz);
	_aligned_free(cParticle->vx);
	_aligned_free(cParticle->vy);
	_aligned_free(cParticle->vz);
	_aligned_free(cParticle->ax);
	_aligned_free(cParticle->ay);
	_aligned_free(cParticle->az);
	_aligned_free(cParticle->jx);
	_aligned_free(cParticle->jy);
	_aligned_free(cParticle->jz);
	_aligned_free(cParticle->preax);
	_aligned_free(cParticle->preay);
	_aligned_free(cParticle->preaz);
	_aligned_free(cParticle->prejx);
	_aligned_free(cParticle->prejy);
	_aligned_free(cParticle->prejz);
	_aligned_free(cParticle->m);
	_aligned_free(cParticle->texid);
	_aligned_free(cParticle->node_num);

	cParticle->px = tmp_px;
	cParticle->py = tmp_py;
	cParticle->pz = tmp_pz;
	cParticle->vx = tmp_vx;
	cParticle->vy = tmp_vy;
	cParticle->vz = tmp_vz;
	cParticle->ax = tmp_ax;
	cParticle->ay = tmp_ay;
	cParticle->az = tmp_az;
	cParticle->jx = tmp_jx;
	cParticle->jy = tmp_jy;
	cParticle->jz = tmp_jz;
	cParticle->preax = tmp_preax;
	cParticle->preay = tmp_preay;
	cParticle->preaz = tmp_preaz;
	cParticle->prejx = tmp_prejx;
	cParticle->prejy = tmp_prejy;
	cParticle->prejz = tmp_prejz;
	cParticle->m = tmp_m;
	cParticle->texid = tmp_texid;
	cParticle->node_num = tmp_node_num;
	cParticle->size = psize;	
}


void node::getNodeNum(){	
	if (node1 == NULL)return;
	if (size == 0){
		//printf("%d,", node_num);
		return;
	}

	/*nodeArray.center_x[node_num] = center_x;
	nodeArray.center_y[node_num] = center_y;
	nodeArray.center_z[node_num] = center_z;
	nodeArray.sum_mass[node_num] = sum_mass;*/
	//printf("%d,", node_num);
	nodeArray.nsize = nodeArray.nsize > node_num ? nodeArray.nsize : node_num;
	//if (depth >= 7)printf("%d  %d....\n", depth, node_num);

	node1->getNodeNum();
	node2->getNodeNum();
	node3->getNodeNum();
	node4->getNodeNum();
	node5->getNodeNum();
	node6->getNodeNum();
	node7->getNodeNum();
	node8->getNodeNum();
}
