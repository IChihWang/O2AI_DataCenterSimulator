//Topology.cpp

#include "Topology.h"
#include <iostream>
#include <vector>
#include <map>

using namespace std;

#define SERVER_BANDWIDTH 512.0
#define SERVER_CPU 100.0
#define SERVER_MEMORY 100.0

Server::Server(int argID, int argEdgeID, int argPod, double argCPU, double argMemory, double argBandwidth){
	this->id = argID;
	this->edge_id = argEdgeID;
	this->pod = argPod;
	this->cpu = argCPU;
	this->memory = argMemory;
	this->bandwidth = argBandwidth;
	this->active = 0;
}

void Server::UpdateServerBandwidth(){
	this->bandwidth = 0.0;
	for (auto u : this->userBandwidth){
		this->bandwidth -= u.second;
	}
	this->bandwidth += SERVER_BANDWIDTH;
}

void Server::UpdateServerCPUandMemory(){
	this->cpu = 0.0;
	this->memory = 0.0;
	for (auto s : serviceInServer){
		this->cpu -= s->GetCPU();
		this->memory -= s->GetMemory();
	}
	this->cpu += SERVER_CPU;
	this->memory += SERVER_MEMORY;
}

int Server::GetReServNum()
{
	int reserv_num = 0;
	for (unsigned i = 0; i < services.size(); i++) {
		if (!services[i]->served && !services[i]->lock) {
			Service* serv_ptr = services[i]->next_service;
			bool is_hop_out = false;
			bool is_hop_back = false;
			while (serv_ptr != nullptr) {
				if (serv_ptr->served) {
					cout << "Error" << endl;
					exit(0);
				}

				if (serv_ptr->GetServerID() != services[i]->GetServerID())
					is_hop_out = true;
				else if (is_hop_out) {
					is_hop_back = true;
					reserv_num++;
					break;
				}
				serv_ptr = serv_ptr->next_service;
			}
		}
	}

	return reserv_num;
}

int Server::GetUnServNum()
{
	int reserv_num = 0;
	for (unsigned i = 0; i < services.size(); i++) {
		if (!services[i]->served && !services[i]->lock) {
			reserv_num++;
		}
	}
	return reserv_num;
}

bool Server::GetIsLocked()
{
	for (unsigned i = 0; i < services.size(); i++) {
		if (!services[i]->served && services[i]->lock) {
			return true;
		}
	}
	return false;
}

void Server::ServeCurrent()
{
	while (1) {
		bool is_do_anything = false;
		for (unsigned i = 0; i < services.size(); i++) {
			if (!services[i]->served && !services[i]->lock) {
				is_do_anything = true;
				services[i]->served = true;
				if (services[i]->next_service != nullptr) {
					services[i]->next_service->lock = false;
				}
			}
		}

		if (!is_do_anything) {
			break;
		}
	}
}
