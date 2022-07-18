//FatTree.cpp

#include "FatTree.h"
#include <iostream>
#include <vector>
#include <map>

using namespace std;

//constructor
FatTree::FatTree(int K){
	this->K = K;
	this->Create_Topology();
};

void FatTree::Link_Create(int link_id, NodeType source_type, int source_id, NodeType destination_type, int destination_id, int type){
	Link link_temp;
	link_temp.id = link_id;
	link_temp.BW = 512.00;// 1G bps = 1024M bps
	link_temp.source_type = source_type;
	link_temp.source_id = source_id;
	link_temp.destination_type = destination_type;
	link_temp.destination_id = destination_id;
	link_temp.type = type;
	this->link.push_back(link_temp);
}

void FatTree::Create_Topology(){
	double server_cpu = 100.0;		//100%
	double server_momery = 100.0;	//100%
	double server_bandwidth = 512.0;//512M bps
	int half_of_K = K / 2;
	int link_id = 0;

	for (int i = 0; i < K; i++){
		for (int j = 0; j < half_of_K; j++){
			Agg_Switch agg_temp;
			agg_temp.id = i*(K / 2) + j;
			agg_temp.pod = i;
			this->agg.push_back(agg_temp);
		}
	}
	//cout << agg.size() << endl;
	for (int i = 0; i < half_of_K; i++){
		for (int j = 0; j < half_of_K; j++){
			Core_Switch core_temp;
			core_temp.id = i*(K / 2) + j;
			for (unsigned int m = 0; m < agg.size(); m++){
				if (core_temp.id * 2 / K == agg[m].id % (K / 2)){
					Link_Create(link_id, agg_node, agg[m].id, core_node, core_temp.id, 3);
					agg[m].core_link.insert(pair<int, int>(core_temp.id, link_id)); //uplink: agg -> core
					link_id++;
					Link_Create(link_id, core_node, core_temp.id, agg_node, agg[m].id, 3);
					core_temp.agg_link.insert(pair<int, int>(agg[m].id, link_id)); //downlink: core ->agg
					link_id++;
				}
				else{ ; }
			}
			this->core.push_back(core_temp);
		}
	}

	for (int i = 0; i < K; i++){
		for (int j = 0; j < half_of_K; j++){
			Edge_Switch edge_temp;
			edge_temp.id = i*(K / 2) + j;
			edge_temp.pod = i;

			for (int m = 0; m < half_of_K; m++){
				//the link between edge SW and agg SW
				int id_agg = i*(K / 2) + m;
				Link_Create(link_id, edge_node, edge_temp.id, agg_node, agg[id_agg].id, 2);
				edge_temp.agg_link.insert(pair<int, int>(agg[id_agg].id, link_id)); //uplink : edge -> agg
				link_id++;
				Link_Create(link_id, agg_node, agg[id_agg].id, edge_node, edge_temp.id, 2);
				agg[id_agg].edge_link.insert(pair<int, int>(edge_temp.id, link_id)); //downlink : agg -> edge
				link_id++;

				//the link between edge SW and server
				Server server_temp(edge_temp.id*(K / 2) + m, edge_temp.id, i, server_cpu, server_momery, server_bandwidth);
				Link_Create(link_id, server_node, server_temp.GetID(), edge_node, edge_temp.id, 1);
				server_temp.edge_link.insert(pair<int, int>(edge_temp.id, link_id)); //uplink: server -> edge
				link_id++;
				Link_Create(link_id, edge_node, edge_temp.id, server_node, server_temp.GetID(), 1);
				edge_temp.server_link.insert(pair<int, int>(server_temp.GetID(), link_id)); //downlink: edge->server
				link_id++;
				this->server.push_back(server_temp);
			}
			this->edge.push_back(edge_temp);
		}
	}
}

void FatTree::Print_Link(){
	for (unsigned int i = 0; i < link.size(); i++){
		switch (link[i].source_type){
		case 0:
			cout << "server\t";
			break;
		case 1:
			cout << "edge\t";
			break;
		case 2:
			cout << "agg\t";
			break;
		case 3:
			cout << "core\t";
			break;
		default:
			break;
		}
		cout << link[i].source_id << " -> ";

		switch (link[i].destination_type){
		case 0:
			cout << "server\t";
			break;
		case 1:
			cout << "edge\t";
			break;
		case 2:
			cout << "agg\t";
			break;
		case 3:
			cout << "core\t";
			break;
		default:
			break;
		}
		cout << link[i].destination_id << endl;
	}
}

void FatTree::Print_Server(){
	for (unsigned int i = 0; i < server.size(); i++){
		cout << "server id : " << server.at(i).GetID() << endl;
		//cout << "egde : " << server.at(i).GetEdgeID() << "\t"
		//	<< "pod : " << server.at(i).GetPod() << "\t"
		cout	<< "cpu : " << server.at(i).GetCPU() << "\t"
			<< "memory : " << server.at(i).GetMemory() << "\t"
			<< "bandwidth : " << server.at(i).GetBandwidth() << "\t"
			<< "active : " << server.at(i).GetActive() << endl;
		cout << "service : " << endl;
		for (auto s : server.at(i).serviceInServer){
			cout << s->GetID() << ", ";
		}
		cout << endl;
	}
}