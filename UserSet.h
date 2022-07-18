//UserSet.h

#ifndef USERSET_H
#define USERSET_H

#include "Service.h"
#include "User.h"
#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <time.h>
#include <fstream>
using namespace std;

class UserSet{
private:
	//std::random_device rd;
	//std::default_random_engine engine();
	std::default_random_engine generator;
	std::vector<std::pair<double, double>> cpu_Value;
	std::vector<std::pair<double, double>> memory_Value;
	std::vector<std::pair<double, double>> hop_Value;
	std::map<int, int> hop_map;
	//vector<normal_distribution<double>> cpu_distributions;
	//vector<normal_distribution<double>> memory_distributions;
	int K;
	int serviceCount;
	int userCount;
	int numOfServiceType;
	int sumHopsViolation;
	int UniformIntRandom(int argMinValue, int argMaxValue);
	unsigned Seed;
	double NormalRandom(double argMean, double argStddev, double argMinValue, double argMaxValue);
	void ReOrderUser_Merge(int first, int mid, int last);
	void Shift(int first, int last);
public:
	std::vector<std::shared_ptr<User>> servingUser;
	std::vector<std::shared_ptr<User>> waitingUser;
	ofstream *user_data;
	int user_count;
	UserSet(){ ; }
	UserSet(int argNumOfServiceType, int argK, unsigned argSeed, ofstream &in_user_data);
	int GetServiceCount();
	int GetHopsViolation(){ return sumHopsViolation; }
	int GenerateHopConstraint(User argUser);
	void GenerateUser(int argMinValue, int argMaxValue, int argTime);
	void ReOrderUser_Recursive(int first, int last);
	void SumHopsViolation();
	void PrintWaitingUser();
	void PrintServingUser();
	void PrintHops();
};

#endif