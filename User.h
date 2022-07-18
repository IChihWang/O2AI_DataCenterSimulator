//User.h

#ifndef USER_H
#define USER_H

#include "Service.h"
#include "FatTree.h"
#include <iostream>
#include <vector>
#include <map>
#include <memory>

enum Served{
	SERVE_DONE = -1, UNSERVE, SERVING
};

class User{
private:
	Served served;
	int id;
	int hopsViolation;
	double cpu;
	double memory;
	//double sumCPUMemory;
	//double penalty;
public:
	int hops[3];
	int defaultHops[4];
	int startTime;
	int endTime;
	int servedTime;
	int service_length;
	double bandwidth;
	//vector <Chain> selection;
	std::vector <std::shared_ptr<Service>> service_chain;
	std::map<int, double> server;
	User(){ ; }
	User(int argID);
	int GetID(){ return this->id; }
	int GetServed(){ return this->served; }
	int GetHopBetweenService(int argHopType){ return defaultHops[argHopType]; }
	int GetHopsViolation(){ return hopsViolation; }
	double GetCPU(){ return this->cpu; }
	double GetMemory(){ return this->memory; }
	double GetCPU_and_Memory(){ return (this->cpu + this->memory); }
	//double GetSumCPUMemory(){ return this->sumCPUMemory; }
	//double GetPanalty(){ return this->penalty; }
	double CountCPU();
	double CountMemory();
	void CountCPU_and_Memory();
	void SetServed(Served argServed);
	void AddHopsViolation(int argHopsVilation){ this->hopsViolation += argHopsVilation; }
	int CountUnServedService();
	void CountHops(std::vector<Server> &server);

	int CountHopsNew(std::vector<Server> &server, int K, int start_point, int win_size);
	int PredictHopsNew(std::vector<Server> &server, int K, int start_point, int win_size);

	bool CheckHops(){ return (this->hops[1] >= this->hops[2]); }
	void CheckServeTime(int &argOnTimeServedUser);
	void Print();
};

#endif