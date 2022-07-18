//Main.cpp

#include "FatTree.h"
#include "UserSet.h"
#include "Simulation.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <cstring>
#include <time.h>
#include <omp.h>

using namespace std;


//#define K 8
//#define numOfServiceType 5

#define START_TIME 0
#define END_TIME 100

//#define MIN_USER_PERTIME 28
//#define MAX_USER_PERTIME 28

#define UPPER_BOND 90
#define LOWER_BOND 0

//#define NUMOFITERATION 1

int K;
int numOfServiceType;
int MIN_USER_PERTIME;
int MAX_USER_PERTIME;
int NUMOFITERATION;

void DefineOutputPath(PlacementAlgo argAlgo, ServiceOrder argSO, char argAlgorithm[], char argServiceOrder[]);
void DefineFileName(int argK, int argTime, int minUser, int maxUser, int argServiceType, char argFileName[]);

int main(int argc, char* argv[]){
	int seed = 0;
	srand(seed);

	/*
	K = atoi(argv[1]);
	numOfServiceType = atoi(argv[2]);
	MIN_USER_PERTIME = atoi(argv[3]);
	MAX_USER_PERTIME = atoi(argv[4]);
	NUMOFITERATION   = atoi(argv[5]);
	*/
	K = 4;
	numOfServiceType = 4;
	MIN_USER_PERTIME = 1;
	MAX_USER_PERTIME = 5;
	NUMOFITERATION = 100;

	int SEED[100];
	for (int i = 0; i < NUMOFITERATION; i++)
	{
		SEED[i] = i + 1;
	}

	char tmp_Time[150] = "Experiment/Compare_RunTime_";
	char fileName_Time[50];
	DefineFileName(K, (END_TIME - START_TIME), MIN_USER_PERTIME, MAX_USER_PERTIME, numOfServiceType, fileName_Time);
	strcat(tmp_Time, fileName_Time);
	ofstream output_Time(tmp_Time);

	vector<PlacementAlgo> algo;
	//algo.push_back(UD_FirstFit);
	//algo.push_back(UD_BestFit);
	//algo.push_back(UD_Order_FirstFit);
	//algo.push_back(UD_Order_BestFit);
	algo.push_back(MW_SW_FirstFit);
	//algo.push_back(MW_SW_BestFit);

	//algo.push_back(OneQ_SW_FirstFit);
	//algo.push_back(HT_SW_FirstFit);
	//algo.push_back(ThreeQ_SW_FirstFit);
	//algo.push_back(FT_SW_FirstFit);

	//algo.push_back(FT_SW_BestFit);

	for (unsigned index = 0; index < algo.size(); index++)
	{
		double startTime, endTime;
		char tmp_Output[150] = "Experiment/";
		char algorithm[30], serviceOrder[20], filename[50];

		//PlacementAlgo algo = UD_Order_FirstFit;
		ServiceOrder so = LargeToSmall;

		DefineOutputPath(algo.at(index), so, algorithm, serviceOrder);
		strcat(tmp_Output, algorithm);
		strcat(tmp_Output, serviceOrder);

		DefineFileName(K, (END_TIME - START_TIME), MIN_USER_PERTIME, MAX_USER_PERTIME, numOfServiceType, filename);
		strcat(tmp_Output, filename);
		//cout << outputPath << endl;

		startTime = clock();
		#pragma omp parallel for
		for (int i = 0; i < NUMOFITERATION; i++)
		{
			cout << "Iterate " << i << endl;

			char outputPath[150], number[10];

			//snprintf(outputPath, 150, tmp_Output);
			snprintf(outputPath, 150, tmp_Output);
			sprintf(number, "%d", i);
			strcat(outputPath, "_data");
			strcat(outputPath, ".csv");
			//cout << number << endl;
			//cout << outputPath << endl;

			FatTree *fattree = new FatTree(K);
			//FatTree fattree(K);
			Simulation simulation(fattree, START_TIME, END_TIME, MIN_USER_PERTIME, MAX_USER_PERTIME, numOfServiceType, SEED[i], outputPath);
			simulation.StartSimulation(algo.at(index), so, i);

			//cout << "Hop_violation: " << simulation.GetHops() << endl;
			//cout << "User: " << simulation.GetViolateUser() << "\tHops: " << simulation.GetViolateHops() << endl;
			//cout << "Rejected_Request: " << simulation.GetRejected_Request() << endl;
			//cout << "Immediately_Allocated_Request: " << simulation.GetImmediately_Allocated_Request() << endl;
			//cout << "Total_Allocated_Request: " << simulation.GetTotal_Allocated_Request() << endl;
			
			delete fattree;
		}
		endTime = clock();
		output_Time << "Algorithm: " << algo[index] << "\tRun_time: " << (endTime - startTime) / ((END_TIME - START_TIME)*MAX_USER_PERTIME*NUMOFITERATION) << " ms." << endl;
	}

	
	output_Time.close();

	system("Pause");
	return 0;
}

void DefineOutputPath(PlacementAlgo argAlgo, ServiceOrder argSO, char argAlgorithm[], char argServiceOrder[]){
	
	switch (argAlgo)
	{
	case UD_FirstFit:
		//snprintf(argAlgorithm, 30, "0. UD_First-Fit/");
		snprintf(argAlgorithm, 30, "0. UD_First-Fit/");
		break;
	case UD_BestFit:
		snprintf(argAlgorithm, 30, "1. UD_Best-Fit/");
		break;
	case UD_Order_FirstFit:
		snprintf(argAlgorithm, 30, "2. UD_First-Fit_Order/");
		break;
	case UD_Order_BestFit:
		snprintf(argAlgorithm, 30, "3. UD_Best-Fit_Order/");
		break;
	case MW_SW_FirstFit:
		snprintf(argAlgorithm, 30, "4. MW_SW_FirstFit/");
		break;
	case MW_SW_BestFit:
		snprintf(argAlgorithm, 30, "5. MW_SW_BestFit/");
		break;
	case OneQ_SW_FirstFit:
		snprintf(argAlgorithm, 30, "5-1. OneQ_SW_FirstFit/");
		break;
	case HT_SW_FirstFit:
		snprintf(argAlgorithm, 30, "5-2. HT_SW_FirstFit/");
		break;
	case ThreeQ_SW_FirstFit:
		snprintf(argAlgorithm, 30, "5-3. ThreeQ_SW_FirstFit/");
		break;
	case FT_SW_FirstFit:
		snprintf(argAlgorithm, 30, "5-4. FT_SW_FirstFit/");
		break;
	case FT_SW_BestFit:
		snprintf(argAlgorithm, 30, "6. FT_SW_BestFit/");
		break;
	case SW_BestFit:
		snprintf(argAlgorithm, 30, "SW_BestFit/");
		break;
	case SW_Complete:
		snprintf(argAlgorithm, 30, "SW_Complete/");
		break;
	default:
		break;
	}
	switch (argSO)
	{
	case SmallToLarge:
		snprintf(argServiceOrder, 20, "SmallToLarge/");
		break;
	case LargeToSmall:
		snprintf(argServiceOrder, 20, "LargeToSmall/");
		break;
	case Random:
		snprintf(argServiceOrder, 20, "Random/");
		break;
	case UnOrder:
		snprintf(argServiceOrder, 20, "UnOrder/");
		break;
	default:
		break;
	}
}

void DefineFileName(int argK, int argTime, int minUser, int maxUser, int argServiceType, char argFileName[]){
	
	char number[10];
	int index;
	
	index = snprintf(argFileName, 50, "K=");
	sprintf(number, "%d", argK);
	index += snprintf(argFileName + index, 50 - index, number);

	index += snprintf(argFileName + index, 50 - index, "_S=");
	sprintf(number, "%d", argServiceType);
	index += snprintf(argFileName + index, 50 - index, number);

	index += snprintf(argFileName + index, 50 - index, "_U=");
	sprintf(number, "%d", minUser);
	index += snprintf(argFileName + index, 50 - index, number);
	index += snprintf(argFileName + index, 50 - index, "-");
	sprintf(number, "%d", maxUser);
	index += snprintf(argFileName + index, 50 - index, number);

	index += snprintf(argFileName + index, 50 - index, "_T=");
	sprintf(number, "%d", argTime);
	index += snprintf(argFileName + index, 50 - index, number);

	index += snprintf(argFileName + index, 50 - index, "-");
}
