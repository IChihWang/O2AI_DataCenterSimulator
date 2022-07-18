//FatTree.h
#ifndef FATTREE_H
#define FATTREE_H

#include "Topology.h"
#include <iostream>
#include <vector>
#include <map>

class FatTree{
private:

	void Link_Create(int link_id, NodeType source_type, int source_id, NodeType destination_type, int destination_id, int type);
	void Create_Topology();
public:
	int K;
	std::vector<Core_Switch>	core;
	std::vector<Agg_Switch>		agg;
	std::vector<Edge_Switch>	edge;
	std::vector<Server>			server;
	std::vector<Link>			link;

	FatTree(){ ; }
	FatTree(int K);
	//~FatTree(){ ; }
	int GetK(){ return K; }
	void Print_Link();
	void Print_Server();
};

#endif // !FATTREE_H