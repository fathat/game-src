#include <Python.h>
#include <iostream>
#include <vector>
#include "astar.h"
using namespace std;

Node::Node()
{
	x = 0;
	y = 0;
	f = 0;
	g = 0;
	h = 0;
	impassable = false;
}

Node::Node( int _x, int _y, bool _impassable) {
	x = _x;
	y = _y;
	impassable = _impassable;
}

AStarMap::AStarMap( PyObject* mask, int w, int h ) {
	width = w;
	height = h;
	nodes = new Node[w*h];
	for(int y=0; y<height; y++) {
		for(int x=0; x<width; x++) {
			PyObject *o = PyList_GetItem(mask, x+y*width);
			bool masked = PyInt_AsLong( o ) != 0  ? true : false ;
			nodes[x+y*width] = Node(x, y, masked );
			nodes[x+y*width].index = x+y*width;
		}
	}

}

AStarMap::~AStarMap() {
}

void AStarMap::computeAdjacency() {
	Node* n=NULL;
	int west;
	int east;
	int north;
	int south;

	for(int y=0; y<height; y++) {
		for(int x=0; x<width; x++) {
			n = &nodes[x+y*width];
			west  = (x-1)+y*width;
			east  = (x+1)+y*width;
			north = x+(y-1)*width;
			south = x+(y+1)*width;
			if( x-1 > 0 && !nodes[west].impassable )
				n->adjacent.push_back( &nodes[west] );
			if( x+1 < width && !nodes[east].impassable )
				n->adjacent.push_back( &nodes[east] );
			if( y-1 > 0 && !nodes[north].impassable )
				n->adjacent.push_back( &nodes[north] );
			if( y+1 < width && !nodes[south].impassable )
				n->adjacent.push_back( &nodes[south] );
		}
	}

}

void AStarMap::printMask() {
	for(int y=0; y<height; y++) {
		for(int x=0; x<width; x++) {
			cout << ( nodes[x+y*width].impassable ? "1" : "0" );
		}
		cout << endl;
	}
}

void AStarMap::path( int start_x, int start_y, int end_x, int end_y, PyObject* path ) {
	//declarations
	/*vector<Node*> open;
    Node* current_node;
	open.push_back( this->nodes[x+y*width]);

	while( !open.empty() ) {
		//remove lowest f-cost node from the open list
		current_node = open[0];
		int current_i = 0;
		for(unsigned int i=0; i<open.size(); i++) {
			if(open[i]->f < current_node->f ) {
				current_node = open[i];
				current_i = i;
			}			
		}

		open.erase( open.begin() + current_i );
		openMask[current_node->index] = false;
		closedMask[current_node->index] = true;

		//test for goal
		if( current_node->x == end_x && current_node->y == end_y )
			break;

		//open adjacent nodes
	}*/
}

