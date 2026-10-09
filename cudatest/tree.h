struct _node{
	float x;
	float y;
	float z;
	float distans;
	float center_x;
	float center_y;
	float center_z;
	float mass;
	unsigned int next;
	unsigned int size = 0;
};

class tree{
public:
	_node *nodeArray;
	float xmin = FLT_MAX, xmax = -FLT_MAX, ymin = FLT_MAX, ymax = -FLT_MAX, zmin = FLT_MAX, zmax = -FLT_MAX;
	unsigned int size = 0;

	tree(unsigned int size);
	tree();

	void createCell(particle *srcParticle, particle *dstParticle);
};