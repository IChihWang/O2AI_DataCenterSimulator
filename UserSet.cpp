//UserSet.cpp

#include "UserSet.h"
#include "Service.h"
#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <memory>

using namespace std;

#define MEAN_BW		100.0
#define STDDEV_BW	5.0
#define RUN_TIME	10

#define MIN_CAPACITY 5.0
#define MAX_CAPACITY 50.0

//constructor : UserSet
UserSet::UserSet(int argNumOfServiceType, int argK, unsigned argSeed, ofstream &in_user_data){
	this->numOfServiceType = argNumOfServiceType;
	this->serviceCount = 0;
	this->userCount = 0;
	this->sumHopsViolation = 0;
	this->K = argK;
	this->Seed = argSeed;
	this->user_data = &in_user_data;
	user_count = 0;

	//initial cpu and memory distribution type
	double step = (MAX_CAPACITY - MIN_CAPACITY) / argNumOfServiceType;
	for (int i = 0; i < argNumOfServiceType; i++)
	{
		cpu_Value.push_back(make_pair(MIN_CAPACITY + i*step, 3.0));
		memory_Value.push_back(make_pair(MIN_CAPACITY + i*step, 3.0));
	}

	/*for (int i = 0; i < argNumOfServiceType; i++)
	{
		cout << cpu_Value.at(i).first << "\t" << cpu_Value.at(i).second << endl;
	}*/

	//count hops
	for (int i = 0; i < argNumOfServiceType; i++){
		hop_map.insert(make_pair(i + 1, 6 * i));

	}

	//initial hop distribution type
	for (auto& it : hop_map){
		hop_Value.push_back(make_pair(double(it.second / 2), double(it.second / 3)));
	}
	//random_device rd;
	//unsigned seed = 1000;
	this->generator = default_random_engine(this->Seed);
}

int UserSet::GetServiceCount(){
	return this->serviceCount;
}

int UserSet::UniformIntRandom(int argMinValue, int argMaxValue){
	//every random will be different
	/*random_device rd;
	default_random_engine generator(rd());*/

	//default_random_engine generator;
	uniform_int_distribution<int> distribution(argMinValue, argMaxValue);

	int number = distribution(generator);
	return number;
}

double UserSet::NormalRandom(double argMean, double argStddev, double argMinValue, double argMaxValue){
	//every random will be differnt
	/*random_device rd;
	default_random_engine generator(rd());*/

	if (argMean == 0.0&&argStddev == 0.0){
		return 0.0;
	}

	if (argMinValue == argMaxValue){
		return argMinValue;
	}

	normal_distribution<double> distribution(argMean, argStddev);
	double number;
	do{
		number = distribution(generator);
	} while (number<argMinValue || number>argMaxValue);
	return number;
}

void UserSet::GenerateUser(int argMinValue, int argMaxValue, int argTime){

	uniform_int_distribution<int> userNumDSTB(argMinValue, argMaxValue);
	uniform_int_distribution<int> runTimeDSTB(1, RUN_TIME);
	int numOfUser = userNumDSTB(generator);
	//int userSize = unServedUser.size();

	for (int i = 0; i < numOfUser; i++){
		User userTemp(this->userCount + i);
		userTemp.bandwidth = NormalRandom(MEAN_BW, STDDEV_BW, 80.0, 120.0);
		userTemp.service_length = UniformIntRandom(1, numOfServiceType);
		userTemp.startTime = argTime;
		userTemp.servedTime = -1;
		userTemp.endTime = argTime + runTimeDSTB(generator);
		userTemp.hops[0] = hop_map.find(userTemp.service_length)->second;
		userTemp.hops[2] = 0;

		default_random_engine generator(rand());
		bernoulli_distribution distribution(0.5);

		vector<int> scTemp;
		vector<int>::iterator it;
		for (int j = 0; j < userTemp.service_length; j++){
			// Find acclocated service type
			while (1){
				int serviceType = UniformIntRandom(1, numOfServiceType);
				it = find(scTemp.begin(), scTemp.end(), serviceType);
				
				if (it != scTemp.end()){ continue; }
				else{
					Service serviceTemp(serviceCount, serviceType);
					serviceTemp.SetCPU(NormalRandom(cpu_Value.at(serviceType - 1).first, cpu_Value.at(serviceType - 1).second, 10.0, 40.0));
					serviceTemp.SetMemory(NormalRandom(memory_Value.at(serviceType - 1).first, memory_Value.at(serviceType - 1).second, 10.0, 40.0));
					//serviceTemp.SetCPU(25.1);
					//serviceTemp.SetMemory(25.1);
					serviceTemp.SetSumCPUMemory();

					userTemp.service_chain.push_back(make_shared<Service>(serviceTemp));
					scTemp.push_back(serviceType);
					serviceCount++;

					if (j > 0) {
						if (distribution(generator)) {
							userTemp.service_chain[j]->lock = true;
							userTemp.service_chain[j - 1]->next_service = &(*(userTemp.service_chain[j]));
						}
					}

					break;
				}
			}
		}
		
		userTemp.hops[1] = GenerateHopConstraint(userTemp);
		/*if (userTemp.hops[1] > userTemp.hops[0]){
			cout << userTemp.hops[0] << "\t" << userTemp.hops[1] << endl;
		}*/
		//userTemp.CountCPU();
		//userTemp.CountMemory();
		userTemp.CountCPU_and_Memory();
		waitingUser.push_back(make_shared<User>(userTemp));

		(*user_data) << "User " << user_count++ << endl;
		(*user_data) << "     CPU_usage " << userTemp.GetCPU() << endl;
		(*user_data) << "     MEM_usage " << userTemp.bandwidth << endl;
		(*user_data) << "     BAN_usage " << userTemp.bandwidth << endl;
		(*user_data) << "     Service_Num " << userTemp.service_length << endl;
		(*user_data) << "     Service_Time " << userTemp.endTime- userTemp.startTime << endl;
		(*user_data) << "     Hop_Request " << userTemp.hops[1] << endl;
		(*user_data) << "     Show_up_at " << argTime << endl;
		for (int j = 0; j < userTemp.service_length; j++) {
			(*user_data) << "          Service " << j << endl;
			(*user_data) << "               CPU " << userTemp.service_chain[j]->GetCPU() << endl;
			(*user_data) << "               MEM " << userTemp.service_chain[j]->GetMemory() << endl;
			(*user_data) << "               BAN " << userTemp.bandwidth << endl;
			(*user_data) << "               LOCK " << (int)(userTemp.service_chain[j]->lock) << endl;
		}

	}
	userCount += numOfUser;
}

void UserSet::ReOrderUser_Recursive(int first, int last){
	if (first < last)
	{
		int mid = (int)(((double)first + (double)last) / 2);
		ReOrderUser_Recursive(first, mid);
		ReOrderUser_Recursive(mid + 1, last);
		ReOrderUser_Merge(first, mid, last);
	}
}

void UserSet::ReOrderUser_Merge(int first, int mid, int last){
	int i = first;
	int j = mid + 1;

	while (j != last + 1)
	{
		if (this->waitingUser.at(i)->GetCPU_and_Memory() >= this->waitingUser.at(j)->GetCPU_and_Memory())
		{
			if (i == j)
			{
				j++;
			}
		}
		else
		{
			Shift(i, j);
			j++;
		}
		i++;
	}
}

void UserSet::Shift(int first, int last){
	shared_ptr<User> tmpUser = this->waitingUser.at(last);

	for (int i = last - 1; i >= first; i--)
	{
		this->waitingUser.at(i + 1) = this->waitingUser.at(i);
	}
	this->waitingUser.at(first) = tmpUser;
}

void UserSet::PrintWaitingUser(){
	for (auto it : waitingUser){
		cout << "user id : " << it->GetID() << endl;
		cout << "cpu : " << it->GetCPU() << "\t"
			<< "memory : " << it->GetMemory() << "\t"
			<< "bw : " << it->bandwidth << endl;
			//<< "penalty : " << it->GetPanalty() << endl;
		//cout << "bw : " << it->bandwidth << "\t"
		cout<< "SL : " << it->service_length << "\t"
			//<< "hop[0] : " << it->hops[0] << "\t"
			<< "hop[1] : " << it->hops[1] << "\t"
			<< "hop[2] : " << it->hops[2] << "\t"
			<< "start : " << it->startTime << "\t"
			<< "sv : " << it->servedTime << "\t"
			<< "end : " << it->endTime << endl;
		for (unsigned int i = 0; i < it->service_chain.size(); i++){
			it->service_chain.at(i)->GetID();
			cout << "id : " << it->service_chain.at(i)->GetID() << "\t"
				//<< "type : " << it->service_chain.at(i)->GetType() << "\t"
				<< "cpu : " << it->service_chain.at(i)->GetCPU() << "\t"
				<< "memory : " << it->service_chain.at(i)->GetMemory() << "\t"
				<< "serverID: " << it->service_chain.at(i)->GetServerID() << endl;
		}
		cout << endl;
		/*vector<shared_ptr<Service>>::iterator it_sv;
		for (it_sv = it->service_chain.begin(); it_sv != it->service_chain.end(); ++it_sv){
			cout << "id : " << it_sv->GetID() << "\t"
				<< "type : " << it_sv->GetType() << "\t"
				<< "cpu : " << it_sv->GetCPU() << "\t"
				<< "memory : " << it_sv->GetMemory() << "\t"
				<< "serverID: " << it_sv->GetServerID() << endl;
		}
		cout << endl;*/
	}
}

void UserSet::PrintServingUser(){
	for (auto it : servingUser){
		cout << "user id : " << it->GetID() << endl;
		cout << "cpu : " << it->GetCPU() << "\t"
			<< "memory : " << it->GetMemory() << "\t"
			<< "bw : " << it->bandwidth << endl;
		cout<< "SL : " << it->service_length << "\t"
			//<< "hop[0] : " << it->hops[0] << "\t"
			<< "hop[1] : " << it->hops[1] << "\t"
			<< "hop[2] : " << it->hops[2] << "\t"
			<< "start : " << it->startTime << "\t"
			<< "sv : " << it->servedTime << "\t"
			<< "end : " << it->endTime << endl;
		for (unsigned int i = 0; i < it->service_chain.size(); i++){
			it->service_chain.at(i)->GetID();
			cout << "id : " << it->service_chain.at(i)->GetID() << "\t"
				//<< "type : " << it->service_chain.at(i)->GetType() << "\t"
				<< "cpu : " << it->service_chain.at(i)->GetCPU() << "\t"
				<< "memory : " << it->service_chain.at(i)->GetMemory() << "\t"
				<< "serverID: " << it->service_chain.at(i)->GetServerID() << endl;
		}
		cout << endl;
		/*vector<shared_ptr<Service>>::iterator it_sv;
		for (it_sv = it->service_chain.begin(); it_sv != it->service_chain.end(); ++it_sv){
		cout << "id : " << it_sv->GetID() << "\t"
		<< "type : " << it_sv->GetType() << "\t"
		<< "cpu : " << it_sv->GetCPU() << "\t"
		<< "memory : " << it_sv->GetMemory() << "\t"
		<< "serverID: " << it_sv->GetServerID() << endl;
		}
		cout << endl;*/
	}
}

void UserSet::PrintHops(){
	//cout << hop_map.size() << endl;
	/*for (auto& x : hop_map){
		cout << x.first << "\t" << x.second << endl;
	}*/
	for (int i = 0; i < numOfServiceType; i++){
		cout << i << "\t";
		cout << "hopValue : " << hop_Value.at(i).first << "\t" << hop_Value.at(i).second << endl;
		cout << endl;
	}
}

void UserSet::SumHopsViolation(){
	this->sumHopsViolation = 0;
	for (auto u : this->waitingUser){
		sumHopsViolation += u->GetHopsViolation();
	}
}

int UserSet::GenerateHopConstraint(User argUser){

	int numOfServer = int(ceil(max(argUser.GetCPU(), argUser.GetMemory()) / 100));
	int hopConstraint, edge = 0, pod = 0;
	double minValue;
	
	if (numOfServer == 1){
		minValue = 0.0;
	}
	else if (numOfServer > 1 && numOfServer <= (K / 2)){
		minValue = 2.0*(numOfServer - 1);
	}
	else if (numOfServer > (K / 2) && numOfServer <= (K / 2)*(K / 2)){
		int temp = numOfServer;
		while (temp > (K / 2)){
			temp -= K / 2;
			edge++;
		}
		minValue = 4.0*edge + 2.0*(edge*(K / 2 - 1) + temp - 1);
	}
	else{
		int pod_temp = numOfServer;
		while (pod_temp > (K / 2)*(K / 2)){
			pod_temp -= (K / 2)*(K / 2);
			pod++;
		}
		int temp = pod_temp;
		while (temp > (K / 2)){
			temp -= K / 2;
			edge++;
		}
		minValue = 6.0*pod + 4.0*(pod*(K / 2 - 1) + edge) + 2.0*(pod*(K / 2)*(K / 2 - 1) + edge*(K / 2 - 1) + temp - 1);
	}

	hopConstraint = int(round(NormalRandom(hop_Value.at(argUser.service_length - 1).first, hop_Value.at(argUser.service_length - 1).second, minValue, argUser.hops[0])));

	if (hopConstraint%2 == 1){
		hopConstraint += 1;
	}

	return max(hopConstraint, 0);
}