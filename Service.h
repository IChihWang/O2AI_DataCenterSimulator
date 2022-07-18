//Service.h
#ifndef SERVICE_H
#define SERVICE_H

#include <iostream>
#include <random>
#include <vector>

class Service{
private:
	int id;
	int type;
	int server_id;	// -1 means unserved
	double cpu;
	double memory;
	double sumCPUMemory;
public:
	Service* next_service;
	bool lock;
	bool served;


	Service(){ ; }
	Service(int argID, int argType);
	int GetID(){ return this->id; }
	int GetType(){ return this->type; }
	int GetServerID(){ return this->server_id; }
	double GetCPU(){ return this->cpu; }
	double GetMemory(){ return this->memory; }
	double GetSumCPUMemory(){ return this->sumCPUMemory; }
	void SetCPU(double argCPU){ this->cpu = argCPU; }
	void SetMemory(double argMemory){ this->memory = argMemory; }
	void SetSumCPUMemory(){ this->sumCPUMemory = this->cpu + this->memory; }
	void SetServerID(int argServerID){ this->server_id = argServerID; }
	void Print();
};

#endif