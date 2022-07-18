//Simulation.h

#ifndef SIMULATION_H
#define SIMULATION_H

//#define ORIG

#include "FatTree.h"
#include "UserSet.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <string>

enum PlacementAlgo{
	UD_FirstFit, UD_BestFit, UD_Order_FirstFit, UD_Order_BestFit, 
	MW_SW_FirstFit, OneQ_SW_FirstFit, HT_SW_FirstFit, ThreeQ_SW_FirstFit, FT_SW_FirstFit,
	MW_SW_BestFit, FT_SW_BestFit, SW_BestFit, SW_Complete
};

enum ServiceOrder{
	SmallToLarge, LargeToSmall, Random, UnOrder
};

class Simulation{
private:
	FatTree *fattree;
	UserSet *us;
	std::ofstream output;
	char* outputPath;
	int startTime;
	int endTime;
	int minUser_perTime;
	int maxUser_perTime;
	int numOfServiceType;
	int totalHops;
	int rejected_Request;
	int immediately_Allocated_Request;
	int total_Allocated_Request;
	int violateUser, violateHops;
	std::vector<int> numOfServices;
	int Seed;
	double runTime;
	double server_Load;
	int Examine_Stage(int argUserID, int argServerID);
	bool IsNotServiceServed(Service &s);
	bool CheckCPU(double argServiceCPU, double argServerCPU){ return (argServerCPU >= argServiceCPU); }
	bool CheckMemory(double argServiceMemory, double argServerMemory){ return (argServerMemory >= argServiceMemory); }
	bool CheckBandwidth(double argUserBandwidth, double argServerBandwidth){ return (argServerBandwidth - argUserBandwidth > 0); }
	bool CheckService(std::shared_ptr<Service> argService, std::vector<Server>::iterator argServer);
	//bool CheckService(Service *argService, std::vector<Server>::iterator argServer);
	void ResetService(std::shared_ptr<User> argUser);
	bool CheckAllConstraintIsOkay(std::shared_ptr<User> argUser, std::vector<int> currVec, int numOfCheckService);
	void PutServiceIntoServer(std::shared_ptr<User> argUser, std::vector<int> argServiceVec);
	int CountHops(std::vector<int> currVec);

	
	int DecideWindowSize(std::shared_ptr<User> argUser);
	bool FindWindowToServe_SlidingFirstFit(std::shared_ptr<User> argUser, std::vector<int> argReOderVec, std::vector<int> &argAnswerVec, unsigned int argS, int argWindowSize);
	bool FindAnswer_FirstFit(std::shared_ptr<User> argUser, std::vector<int> argReOderVec, std::vector<int> &argAnswerVec, int argWindowSize);
	void FreeEndTimeUser(int argTime);
	void CountHopsViolation(int argMethod);
	void CountHopsViolation_Method1();
	void CountHopsViolation_Method2();
	void CountHopsViolation_Method3();
	void AddHops(int argHops){ totalHops += argHops; }
	void AddViolateUser(int argHops){ violateUser += argHops; }
	void AddViolateHops(int argHops){ violateHops += argHops; }
	bool FindWindowToServe_SlidingBesFit(std::shared_ptr<User> argUser, std::vector<int> argReOderVec, std::vector<int> &argAnswerVec, unsigned int argS, int argWindowSize);
	bool FindWindowToServe_SLA(std::shared_ptr<User> argUser, std::vector<int> argReOderVec, std::vector<int> &argAnswerVec, unsigned int argS, int argWindowSize);
public:
	Simulation(FatTree *argFatTree, int argStartTime, int argEndTime, int argMinUser, int argMaxUser, int argNumOfServiceType, int argSeed, char* argOuputPath);
	~Simulation();
	void StartSimulation(PlacementAlgo argAlgo, ServiceOrder argOrder, int iter);
	void UtiliDriven_FirstFitAlgorithm(int argTime);
	void UtiliDriven_Ordering_FirstFitAlgorithm(int argTime, ServiceOrder argOrder);
	void UtiliDriven_BestFitAlgorithm(int argTime);
	void UtiliDriven_Ordering_BestFitAlgorithm(int argTime, ServiceOrder argOrder);
	void MaxWindow_Sliding_FirstFit(int argTime, ServiceOrder argOrder);
	void MaxWindow_Sliding_BestFit(int argTime, ServiceOrder argOrder);
	void FullTopology_Sliding_BestFit(int argTime, ServiceOrder argOrder);

	void OneQuarter_Sliding_FirstFit(int argTime, ServiceOrder argOrder);
	void HalfTopology_Sliding_FirstFit(int argTime, ServiceOrder argOrder);
	void ThreeQuarter_Sliding_FirstFit(int argTime, ServiceOrder argOrder);
	void FullTopology_Sliding_FirstFit(int argTime, ServiceOrder argOrder);

	void SlidingWindow_BestFit(int argTime, ServiceOrder argOrder);
	void SlidingWindow_Complete(int argTime, ServiceOrder argOrder);

	int CountNumToServe(std::shared_ptr<User> argUser, std::vector<Server>::iterator argServer);
	bool FindHopRecursive(int currService, int currServer, const std::shared_ptr<User> argUser, const int maxServer, std::vector<int> &argCurrVec);
	bool MaximizeHopRecursive(int currService, int currServer, const std::shared_ptr<User> argUser, const int maxServer, std::vector<int> &argCurrVec, int &maxHop);
	int GetHops(){ return totalHops; }
	int GetViolateUser(){ return violateUser; }
	int GetViolateHops(){ return violateHops; }
	int GetRejected_Request(){ return rejected_Request; }
	int GetImmediately_Allocated_Request(){ return immediately_Allocated_Request; }
	int GetTotal_Allocated_Request(){ return total_Allocated_Request; }
	void Add_A_Count_Of_Total_Allocated_Request(){ total_Allocated_Request += 1; }
	void Compute_Server_Load(int argTime);
	double Get_Average_Server_Load(){ return server_Load / (endTime - startTime); }
	//void Set_Server_Load(int index, double value){ server_Load[index] = value; }
	void OutputRunTime(double argRunTime);

	int CountHops_new(std::vector<int> currVec, int start_server, int end_server);
};

#endif // !SIMULATION_H
