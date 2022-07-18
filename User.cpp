//User.cpp

#include "User.h"
#include <iostream>
#include <vector>

using namespace std;

//constructor : User
User::User(int argID){
	this->id = argID;
	this->hopsViolation = 0;
	this->served = UNSERVE;
	this->cpu = 0.0;
	this->memory = 0.0;
	//this->sumCPUMemory = 0.0;
	//this->penalty = 0.0;
	for (int i = 0; i < 4; i++){
		defaultHops[i] = 2 * i;
	}
}

double User::CountCPU(){
	
	double sumcpu = 0;
	for (unsigned i = 0; i < service_chain.size(); i++){
		sumcpu += service_chain.at(i)->GetCPU();
	}
	this->cpu = sumcpu;
	return sumcpu;
}

double User::CountMemory(){

	double sumMemory = 0;
	for (unsigned i = 0; i < service_chain.size(); i++){
		sumMemory += service_chain.at(i)->GetMemory();
	}
	this->memory = sumMemory;
	return sumMemory;
}

void User::CountCPU_and_Memory(){
	double sumcpu = 0;
	double sumMemory = 0;
	for (unsigned int i = 0; i < service_chain.size(); i++){
		sumcpu += service_chain.at(i)->GetCPU();
		sumMemory += service_chain.at(i)->GetMemory();
	}
	this->cpu = sumcpu;
	this->memory = sumMemory;
}

void User::SetServed(Served argServed){
	this->served = argServed;
}

int User::CountUnServedService(){
	int count = 0;
	for (auto s : service_chain){
		if (s->GetServerID() == -1){
			count++;
		}
	}
	return count;
}

void User::CountHops(vector<Server> &server){
	
	this->hops[2] = 0;

	if (service_chain.size() > 1){
		for (unsigned int i = 1; i < service_chain.size(); i++){
			int preServerID = service_chain.at(i-1)->GetServerID();
			int proServerID = service_chain.at(i)->GetServerID();

			if (preServerID == proServerID){
				this->hops[2] += 0;
			}
			else if (server.at(preServerID).GetEdgeID() == server.at(proServerID).GetEdgeID()){
				this->hops[2] += 2;
			}
			else if (server.at(preServerID).GetPod() == server.at(proServerID).GetPod()){
				this->hops[2] += 4;
			}
			else{
				this->hops[2] += 6;
			}
		}
	}
	else{ ; }
}

int User::CountHopsNew(std::vector<Server> &server, int K, int start_point, int win_size)
{
	this->hops[2] = 0;
	
	int preServerID = -1;
	int proServerID = -1;

	int* re_serv_pod_val = new int[K]();
	int* serv_pod_val = new int[K]();
	int* lock_pod_val = new int[K]();
	while (1) {
		for (int i = 0; i < K; i++) {
			re_serv_pod_val[i] = 0;
			serv_pod_val[i] = 0;
			lock_pod_val[i] = 0;
		}

		int max_pod_id = -1;
		int max_pod_val = -1;

		// Calculate re-serv number
		for (int i = start_point; i < start_point + win_size; i++) {
			int cur_pod = server.at(i).GetPod();
			re_serv_pod_val[cur_pod] += server.at(i).GetReServNum();
			serv_pod_val[cur_pod] += server.at(i).GetUnServNum();
			if (server.at(i).GetIsLocked()) {
				lock_pod_val[cur_pod]++;
			}
		}
		// Find maximum re-serv number
		for (int i = 0; i < K; i++) {
			if (re_serv_pod_val[i] > max_pod_val) {
				max_pod_val = re_serv_pod_val[i];
			}
		}
		// Find max val as starting point
		for (int i = 0; i < K; i++) {
			if (max_pod_val > re_serv_pod_val[i])
				continue;

			if (max_pod_id == -1) {
				max_pod_id = i;
			}
			else {
				// Found 2 max val, choose one!
				if (serv_pod_val[max_pod_id] < serv_pod_val[i]) {
					max_pod_id = i;
				}
				else if (serv_pod_val[max_pod_id] == serv_pod_val[i]) {
					if (lock_pod_val[max_pod_id] > lock_pod_val[i]) {
						max_pod_id = i;
					}
				}
			}
		}

		// Nothing to be served
		if (serv_pod_val[max_pod_id] == 0)
			break;
		
		// Pod have chosen, deal with edge
		// Pick servers in the pod
		vector <Server*> tar_edge_server;
		for (int i = start_point; i < start_point + win_size; i++) {
			if (server.at(i).GetPod() == max_pod_id)
				tar_edge_server.push_back(&(server.at(i)));
		}

		int edge_total_num = K*K / 2;
		int* re_serv_edge_val = new int[edge_total_num]();
		int* serv_edge_val = new int[edge_total_num]();
		int* lock_edge_val = new int[edge_total_num]();
		while (1) {
			for (int i = 0; i < edge_total_num; i++) {
				re_serv_edge_val[i] = 0;
				serv_edge_val[i] = 0;
				lock_edge_val[i] = 0;
			}

			int max_edge_id = -1;
			int max_edge_val = -1;

			// Calculate re-serv number
			for (unsigned int i = 0; i < tar_edge_server.size(); i++) {
				int cur_edge = tar_edge_server[i]->GetEdgeID();
				re_serv_edge_val[cur_edge] += tar_edge_server[i]->GetReServNum();
				serv_edge_val[cur_edge] += tar_edge_server[i]->GetUnServNum();
				
				if (tar_edge_server[i]->GetIsLocked())
					lock_edge_val[cur_edge]++;
			}
			// Find maximum re-serv number
			for (int i = 0; i < edge_total_num; i++) {
				if (re_serv_edge_val[i] > max_edge_val) {
					max_edge_val = re_serv_edge_val[i];
				}
			}

			// Find max val as starting point
			for (int i = 0; i < edge_total_num; i++) {
				if (max_edge_val > re_serv_edge_val[i])
					continue;

				if (max_edge_id == -1) {
					max_edge_id = i;
				}
				else {
					// Found 2 max val, choose one!
					if (serv_edge_val[max_edge_id] < serv_edge_val[i]) {
						max_edge_id = i;
					}
					else if (serv_edge_val[max_edge_id] == serv_edge_val[i]) {
						if (lock_edge_val[max_edge_id] > lock_edge_val[i]) {
							max_edge_id = i;
						}
					}
				}
			}

			// Nothing to be served
			if (serv_edge_val[max_edge_id] == 0) {
				break;
			}

			vector <Server*> tar_server;
			for (unsigned int i = 0; i < tar_edge_server.size(); i++) {
				
				if (tar_edge_server[i]->GetEdgeID() == max_edge_id)
					tar_server.push_back(tar_edge_server[i]);
			}

			int server_total_num = K*K*K/4;
			int* re_serv_server_val = new int[server_total_num]();
			int* serv_server_val = new int[server_total_num];
			int* lock_server_val = new int[server_total_num];
			while (1) {
				for (int i = 0; i < server_total_num; i++) {
					re_serv_server_val[i] = 0;
					serv_server_val[i] = 0;
					lock_server_val[i] = 0;
				}

				int max_server_id = -1;
				int max_server_val = -1;

				// Calculate re-serv number
				for (unsigned int i = 0; i < tar_server.size(); i++) {
					int cur_serv = tar_server[i]->GetID();
					re_serv_server_val[cur_serv] += tar_server[i]->GetReServNum();
					serv_server_val[cur_serv] += tar_server[i]->GetUnServNum();
					if (tar_server[i]->GetIsLocked())
						lock_server_val[cur_serv]++;
				}
				// Find maximum re-serv number
				for (int i = 0; i < server_total_num; i++) {
					if (re_serv_server_val[i] > max_server_val) {
						max_server_val = re_serv_server_val[i];
					}
				}

				// Find max val as starting point
				for (int i = 0; i < server_total_num; i++) {
					if (max_server_val > re_serv_server_val[i])
						continue;

					if (max_server_id == -1) {
						max_server_id = i;
					}
					else {
						// Found 2 max val, choose one!
						if (serv_server_val[max_server_id] < serv_server_val[i]) {
							max_server_id = i;
						}
						else if (serv_server_val[max_server_id] == serv_server_val[i]) {
							if (lock_server_val[max_server_id] > lock_server_val[i]) {
								max_server_id = i;
							}
						}
					}
				}

				// Nothing to be served
				if (serv_server_val[max_server_id] == 0)
					break;
				else {
					// Decide hop count
					if (proServerID == -1) {
					}
					else if (preServerID = -1) {
					}
					else {
						if (preServerID == proServerID) {
							cout << "Shouldn't choose same server!" << endl;
							exit(-1);
						}
						else if (server.at(preServerID).GetEdgeID() == server.at(proServerID).GetEdgeID()) {
							this->hops[2] += 2;
						}
						else if (server.at(preServerID).GetPod() == server.at(proServerID).GetPod()) {
							this->hops[2] += 4;
						}
						else {
							this->hops[2] += 6;
						}
					}

					preServerID = proServerID;
					proServerID = max_server_id;

					// Update data
					server.at(max_server_id).ServeCurrent();
				}
			}

			delete[] re_serv_server_val;
			delete[] serv_server_val;
			delete[] lock_server_val;
		}

		delete[] re_serv_edge_val;
		delete[] serv_edge_val;
		delete[] lock_edge_val;
	}

	delete[] re_serv_pod_val;
	delete[] serv_pod_val;
	delete[] lock_pod_val;

	return this->hops[2];
}

int User::PredictHopsNew(std::vector<Server>& server, int K, int start_point, int win_size)
{
	int hops = 0;

	int preServerID = -1;
	int proServerID = -1;

	int* re_serv_pod_val = new int[K]();
	int* serv_pod_val = new int[K]();
	int* lock_pod_val = new int[K]();
	while (1) {
		for (int i = 0; i < K; i++) {
			re_serv_pod_val[i] = 0;
			serv_pod_val[i] = 0;
			lock_pod_val[i] = 0;
		}

		int max_pod_id = -1;
		int max_pod_val = -1;

		// Calculate re-serv number
		for (int i = start_point; i < start_point + win_size; i++) {
			int cur_pod = server.at(i).GetPod();
			re_serv_pod_val[cur_pod] += server.at(i).GetReServNum();
			serv_pod_val[cur_pod] += server.at(i).GetUnServNum();
			if (server.at(i).GetIsLocked()) {
				lock_pod_val[cur_pod]++;
			}
		}
		// Find maximum re-serv number
		for (int i = 0; i < K; i++) {
			if (re_serv_pod_val[i] > max_pod_val) {
				max_pod_val = re_serv_pod_val[i];
			}
		}
		// Find max val as starting point
		for (int i = 0; i < K; i++) {
			if (max_pod_val > re_serv_pod_val[i])
				continue;

			if (max_pod_id == -1) {
				max_pod_id = i;
			}
			else {
				// Found 2 max val, choose one!
				if (serv_pod_val[max_pod_id] < serv_pod_val[i]) {
					max_pod_id = i;
				}
				else if (serv_pod_val[max_pod_id] == serv_pod_val[i]) {
					if (lock_pod_val[max_pod_id] > lock_pod_val[i]) {
						max_pod_id = i;
					}
				}
			}
		}

		// Nothing to be served
		if (serv_pod_val[max_pod_id] == 0)
			break;

		// Pod have chosen, deal with edge
		// Pick servers in the pod
		vector <Server*> tar_edge_server;
		for (int i = start_point; i < start_point + win_size; i++) {
			if (server.at(i).GetPod() == max_pod_id)
				tar_edge_server.push_back(&(server.at(i)));
		}

		int edge_total_num = K*K / 2;
		int* re_serv_edge_val = new int[edge_total_num]();
		int* serv_edge_val = new int[edge_total_num]();
		int* lock_edge_val = new int[edge_total_num]();
		while (1) {
			for (int i = 0; i < edge_total_num; i++) {
				re_serv_edge_val[i] = 0;
				serv_edge_val[i] = 0;
				lock_edge_val[i] = 0;
			}

			int max_edge_id = -1;
			int max_edge_val = -1;

			// Calculate re-serv number
			for (unsigned int i = 0; i < tar_edge_server.size(); i++) {
				int cur_edge = tar_edge_server[i]->GetEdgeID();
				re_serv_edge_val[cur_edge] += tar_edge_server[i]->GetReServNum();
				serv_edge_val[cur_edge] += tar_edge_server[i]->GetUnServNum();
				if (tar_edge_server[i]->GetIsLocked())
					lock_edge_val[cur_edge]++;
			}
			// Find maximum re-serv number
			for (int i = 0; i < edge_total_num; i++) {
				if (re_serv_edge_val[i] > max_edge_val) {
					max_edge_val = re_serv_edge_val[i];
				}
			}

			// Find max val as starting point
			for (int i = 0; i < edge_total_num; i++) {
				if (max_edge_val > re_serv_edge_val[i])
					continue;

				if (max_edge_id == -1) {
					max_edge_id = i;
				}
				else {
					// Found 2 max val, choose one!
					if (serv_edge_val[max_edge_id] < serv_edge_val[i]) {
						max_edge_id = i;
					}
					else if (serv_edge_val[max_edge_id] == serv_edge_val[i]) {
						if (lock_edge_val[max_edge_id] > lock_edge_val[i]) {
							max_edge_id = i;
						}
					}
				}
			}

			// Nothing to be served
			if (serv_edge_val[max_edge_id] == 0)
				break;

			vector <Server*> tar_server;
			for (unsigned int i = 0; i < tar_edge_server.size(); i++) {
				if (tar_edge_server[i]->GetEdgeID() == max_edge_id)
					tar_server.push_back(&(server.at(i)));
			}

			int server_total_num = K*K*K / 4;
			int* re_serv_server_val = new int[server_total_num]();
			int* serv_server_val = new int[server_total_num]();
			int* lock_server_val = new int[server_total_num]();
			while (1) {
				for (int i = 0; i < server_total_num; i++) {
					re_serv_server_val[i] = 0;
					serv_server_val[i] = 0;
					lock_server_val = 0;
				}

				int max_server_id = -1;
				int max_server_val = -1;

				// Calculate re-serv number
				for (unsigned int i = 0; i < tar_server.size(); i++) {
					int cur_serv = tar_server[i]->GetEdgeID();
					re_serv_server_val[cur_serv] += tar_server[i]->GetReServNum();
					serv_server_val[cur_serv] += tar_server[i]->GetUnServNum();
					if (tar_server[i]->GetIsLocked())
						lock_server_val[cur_serv]++;
				}
				// Find maximum re-serv number
				for (int i = 0; i < server_total_num; i++) {
					if (re_serv_server_val[i] > max_server_val) {
						max_server_val = re_serv_server_val[i];
					}
				}

				// Find max val as starting point
				for (int i = 0; i < server_total_num; i++) {
					if (max_server_val > re_serv_server_val[i])
						continue;

					if (max_server_id == -1) {
						max_server_id = i;
					}
					else {
						// Found 2 max val, choose one!
						if (serv_server_val[max_server_id] < serv_server_val[i]) {
							max_server_id = i;
						}
						else if (serv_server_val[max_server_id] == serv_server_val[i]) {
							if (lock_server_val[max_server_id] > lock_server_val[i]) {
								max_server_id = i;
							}
						}
					}
				}

				// Nothing to be served
				if (serv_server_val[max_server_id] == 0)
					break;
				else {
					// Decide hop count
					if (proServerID == -1) {
					}
					else if (preServerID = -1) {
					}
					else {
						if (preServerID == proServerID) {
							cout << "Shouldn't choose same server!" << endl;
							exit(-1);
						}
						else if (server.at(preServerID).GetEdgeID() == server.at(proServerID).GetEdgeID()) {
							hops += 2;
						}
						else if (server.at(preServerID).GetPod() == server.at(proServerID).GetPod()) {
							hops += 4;
						}
						else {
							hops += 6;
						}
					}

					preServerID = proServerID;
					proServerID = max_server_id;

					// Update data
					server.at(max_server_id).ServeCurrent();
				}
			}

			delete[] re_serv_server_val;
			delete[] serv_server_val;
			delete[] lock_server_val;
		}

		delete[] re_serv_edge_val;
		delete[] serv_edge_val;
		delete[] lock_edge_val;
	}

	delete[] re_serv_pod_val;
	delete[] serv_pod_val;
	delete[] lock_pod_val;

	return hops;
}

void User::CheckServeTime(int &immediately_Allocated_Request){
	if (servedTime == startTime){ 
        immediately_Allocated_Request++;
	}
	/*else if (servedTime > startTime&&servedTime < endTime){
		endTime += (servedTime - startTime);
	}*/
	else{
		endTime = servedTime + (endTime - startTime);
	}
}

void User::Print(){
	cout << "user id : " << this->GetID() << endl;
	cout << "cpu : " << this->GetCPU() << "\t"
		<< "memory : " << this->GetMemory() << endl;
		//<< "sum: " << this->GetSumCPUMemory() << endl;
		//<< "penalty : " << this->GetPanalty() << endl;
	cout << "bw : " << this->bandwidth << "\t"
		<< "SL : " << this->service_length << "\t"
		<< "hop[0] : " << this->hops[0] << "\t"
		<< "hop[1] : " << this->hops[1] << "\t"
		<< "start : " << this->startTime << "\t"
		<< "end : " << this->endTime << endl;
	//vector<Service>::iterator it_sv;
	for (auto it_sv: service_chain){
		cout << "id : " << it_sv->GetID() << "\t"
			<< "type : " << it_sv->GetType() << "\t"
			<< "cpu : " << it_sv->GetCPU() << "\t"
			<< "memory : " << it_sv->GetMemory() << "\t"
			<< "serverID: " << it_sv->GetServerID() << endl;
	}
	cout << endl;
}