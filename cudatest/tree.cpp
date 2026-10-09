#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "particle.h"
#include "tree.h"



tree::tree(unsigned int size){
	nodeArray = new _node[size];
}

tree::tree(){}

void tree::createCell(particle *srcParticle, particle *dstParticle){
	for (unsigned int i = 0; i < srcParticle->size; i++){
		xmin = xmin < srcParticle->px[i] ? xmin : srcParticle->px[i];
		xmax = xmax > srcParticle->px[i] ? xmax : srcParticle->px[i];
		ymin = ymin < srcParticle->py[i] ? ymin : srcParticle->py[i];
		ymax = ymax > srcParticle->py[i] ? ymax : srcParticle->py[i];
		zmin = zmin < srcParticle->pz[i] ? zmin : srcParticle->pz[i];
		zmax = zmax > srcParticle->pz[i] ? zmax : srcParticle->pz[i];
	}

	unsigned int num = srcParticle->size;

	int j = 0;
	//float centerx = nodeArray[j].x + nodeArray[j].distans * 0.5f;
	//float centery = nodeArray[j].y + nodeArray[j].distans * 0.5f;
	nodeArray[0].distans = (xmax - xmin) * 0.5f;
	nodeArray[0].center_x = xmax - nodeArray[0].distans;
	nodeArray[0].center_y = ymax - nodeArray[0].distans;
	//printf("%f ,%f, %", nodeArray[0].distans, nodeArray[0].center_x, nodeArray[0].center_y);
	unsigned int cellNum[100000];
	unsigned int particleNum[100000];
	for (int i = 0; i < 100000; i++)particleNum[i] = i;

	for (unsigned int i = 0; i < num; i++){
		if (srcParticle->px[i] <= nodeArray[0].center_x){
			if (srcParticle->py[i] <= nodeArray[0].center_y){//1
				cellNum[i] = cellNum[i] * 4 + 1;
				nodeArray[cellNum[i]].size++;
				//printf("%d ,%.3f, %.3f\n%.3f, %.3f\n", cellNum[i], srcParticle->px[i], srcParticle->py[i], nodeArray[0].center_x - nodeArray[0].distans * 0.5f, nodeArray[0].center_y - nodeArray[0].distans * 0.5f);
			}
			else{//4
				cellNum[i] = cellNum[i] * 4 + 4;
				nodeArray[cellNum[i]].size++;
			}
		}
		else{
			if (srcParticle->py[i] > nodeArray[0].center_y){//2
				cellNum[i] = cellNum[i] * 4 + 2;
				nodeArray[cellNum[i]].size++;
			}
			else{//3
				cellNum[i] = cellNum[i] * 4 + 3;
				nodeArray[cellNum[i]].size++;
			}
		}
	}

	nodeArray[1].distans = nodeArray[0].distans * 0.5f;
	nodeArray[1].center_x = nodeArray[0].center_x - nodeArray[1].distans;
	nodeArray[1].center_y = nodeArray[0].center_y - nodeArray[1].distans;

	nodeArray[2].distans = nodeArray[0].distans * 0.5f;
	nodeArray[2].center_x = nodeArray[0].center_x + nodeArray[1].distans;
	nodeArray[2].center_y = nodeArray[0].center_y - nodeArray[1].distans;

	nodeArray[3].distans = nodeArray[0].distans * 0.5f;
	nodeArray[3].center_x = nodeArray[0].center_x + nodeArray[1].distans;
	nodeArray[3].center_y = nodeArray[0].center_y + nodeArray[1].distans;

	nodeArray[4].distans = nodeArray[0].distans * 0.5f;
	nodeArray[4].center_x = nodeArray[0].center_x - nodeArray[1].distans;
	nodeArray[4].center_y = nodeArray[0].center_y + nodeArray[1].distans;

	for (unsigned int j = 0; j <num; j++){
		for (unsigned int i = 0; i < num; i++){
			if (cellNum[i] > cellNum[i + 1]){
				unsigned int tmp = cellNum[i];
				cellNum[i] = cellNum[i + 1];
				cellNum[i + 1] = tmp;

				tmp = particleNum[i];
				particleNum[i] = particleNum[i + 1];
				particleNum[i + 1] = tmp;
			}
		}
	}

	while (1){

	}

	for (unsigned int i = nodeArray[1].size; i < nodeArray[1].size + nodeArray[2].size; i++){
		printf("%d   %f,%f  %f,%f\n", i, srcParticle->px[particleNum[i]], srcParticle->py[particleNum[i]], nodeArray[0].center_x, nodeArray[0].center_y);
	}
}