#ifndef ASTAR_H
#define ASTAR_H

#include <vector>

struct Node
{
public:
	int x, y;
	int index;
	bool impassable;
	int f, g, h;
	
	std::vector<Node*> adjacent;

	Node( int _x, int _y, bool _impassable);
	Node();
};

class AStarMap
{
	Node* nodes;
	int width, height;
	bool* closedMask;
	bool* openMask;
public:
	AStarMap( PyObject* mask, int w, int h );
	~AStarMap();


	void computeAdjacency();
    void path( int start_x, int start_y, int end_x, int end_y, PyObject* path );
	void printMask();
	

};

#endif

