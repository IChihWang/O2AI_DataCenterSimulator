//Topology.h

#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include "Service.h"
#include <iostream>
#include <vector>
#include <map>
#include <memory>

//Construting topology

enum NodeType
{
	server_node, edge_node, agg_node, core_node
};

class Link{ //direct
public:
	int id;
	double BW;
	int source_type;
	int source_id;
	int destination_type;
	int destination_id;
	int type; //1 = layer_1, 2 = layer_2, 3 = layer_3
	Link(){};
};

class Edge_Switch{
public:
	int id;
	int pod;
	std::map<int, int> server_link;
	std::map<int, int> agg_link;
	Edge_Switch(){};
};

class Agg_Switch{
public:
	int id;
	int pod;
	std::map<int, int> edge_link;
	std::map<int, int> core_link;
	Agg_Switch(){};
};

class Core_Switch{
public:
	int id;
	std::map<int, int> agg_link;
	Core_Switch(){};
};

class Server{
private:
	int id;
	int edge_id;
	int pod;
	int active;	//0:shutdown, 1:active
	double cpu;
	double memory;
	double bandwidth;
public:
	std::map<int, double> userBandwidth;
	std::map<int, int> edge_link;
	std::map<int, int> service_inside; //serice_type, service_id
	//std::vector<Service> serviceInServer;
	std::vector<std::shared_ptr<Service>> serviceInServer;
	std::vector<Service*> services;
	std::vector<int> serviceID;
	Server(){};
	Server(int argID, int argEdgeID, int argPod, double argCPU, double argMemory, double argBandwidth);
	int GetID(){ return this->id; }
	int GetEdgeID(){ return this->edge_id; }
	int GetPod(){ return this->pod; }
	int GetActive(){ return this->active; }
	double GetCPU(){ return this->cpu; }
	double GetMemory(){ return this->memory; }
	double GetBandwidth(){ return this->bandwidth; }
	void SetActive(int argActive){ this->active = argActive; }
	void SetCPU(double argCPU){ this->cpu = argCPU; }
	void SetMemory(double argMemory){ this->memory = argMemory; }
	void UpdateServerBandwidth();
	void UpdateServerCPUandMemory();

	int GetReServNum();
	int GetUnServNum();
	bool GetIsLocked();
	void ServeCurrent();
};

class Path{
public:
	int id;
	double BW;
	int source_server;
	int destination_server;
	std::vector <Link> link_set;
	Path(){};
};

#endif
