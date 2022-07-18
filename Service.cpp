//Service.cpp

#include "Service.h"
#include <iostream>
#include <random>
#include <vector>

using namespace std;

//constructor
Service::Service(int argID, int argType){
	this->id = argID;
	this->type = argType;
	this->server_id = -1;
	this->next_service = nullptr;
	this->lock = false;
	this->served = false;
}

void Service::Print(){
	cout << this->id << ": " << this->server_id << endl;
	//cout << "service id : " << this->id << endl;
	/*cout << "type : " << this->type << "\t"
		<< "server id : " << this->server_id << "\t"
		<< "cpu : " << this->cpu << "\t"
		<< "memory : " << this->memory << endl;*/
}