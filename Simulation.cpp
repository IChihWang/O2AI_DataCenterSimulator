//Simulation.cpp

#include "Simulation.h"
#include "UserSet.h"
#include "MergeSort.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <vector>
#include <map>
#include <memory>
#include <cmath>
#include <chrono>
#include <omp.h>

using namespace std;

//constructor
Simulation::Simulation(FatTree *argFatTree, int argStartTime, int argEndTime, int argMinUser, int argMaxUser, int argNumOfServiceType, int argSeed, char* argOuputPath){
    fattree = argFatTree;
    startTime = argStartTime;
    endTime = argEndTime;
    minUser_perTime = argMinUser;
    maxUser_perTime = argMaxUser;
    numOfServiceType = argNumOfServiceType;
    Seed = argSeed;
    output.open(argOuputPath, ios_base::app);
    for (int i = 0; i < argNumOfServiceType; i++){
        this->numOfServices.push_back(i);
    }
    totalHops = 0;
    rejected_Request = 0;
    immediately_Allocated_Request = 0;
    total_Allocated_Request = 0;
    violateHops = 0;
    violateUser = 0;

    server_Load = 0.0;
}

Simulation::~Simulation(){
    //delete fattree;
    //output.close();
    delete us;
}

void Simulation::StartSimulation(PlacementAlgo argAlgo, ServiceOrder argOrder, int iter){

    //output << "Algorithm: " << argAlgo << "\tSeed: " << Seed << endl;
   /* output << "\"Total_Hop_Violation\","
        << "\"User_Violation\","
        << "\"Hops_Violation\","
        << "\"Rejected_Request\","
        << "\"Immediately_Allocated_Request\","
        << "\"Total_Allocated_Request\","
        << "\"Average_Server_Load\""
        << endl;*/

	ofstream user_data;
	string file_name = "DataSet/";
	file_name = file_name + "UserSet_" + to_string(iter) + ".txt";
	user_data.open(file_name);

    this->us = new UserSet(numOfServiceType, fattree->GetK(), Seed, user_data);

    for (int time = startTime; time < endTime; time++){

        //output << "############\tTime: " << time << "\t############" << endl;
        us->GenerateUser(this->minUser_perTime, this->maxUser_perTime, time);


        //us->PrintWaitingUser();

        //us->ReOrderUser_Recursive(0, us->waitingUser.size() - 1);
        //us->PrintWaitingUser();

        /*for (auto it : us->waitingUser){
            output << "user id : " << it->GetID() << endl;
            output << "cpu : " << it->GetCPU() << "\t"
                << "memory : " << it->GetMemory() << "\t"
                << "bw : " << it->bandwidth << endl;
            output << "SL : " << it->service_length << "\t"
                << "hop[1] : " << it->hops[1] << "\t"
                << "hop[2] : " << it->hops[2] << "\t"
                << "start : " << it->startTime << "\t"
                << "sv : " << it->servedTime << "\t"
                << "end : " << it->endTime << endl;
            for (unsigned int i = 0; i < it->service_chain.size(); i++){
                it->service_chain.at(i)->GetID();
                output << "id : " << it->service_chain.at(i)->GetID() << "\t"
                    << "cpu : " << it->service_chain.at(i)->GetCPU() << "\t"
                    << "memory : " << it->service_chain.at(i)->GetMemory() << "\t"
                    << "serverID: " << it->service_chain.at(i)->GetServerID() << endl;
            }
            output << endl;
        }*/

        //int preTotalHops = totalHops;
        //int preViolateUser = violateUser;
        //int preViolateHops = violateHops;
        //output << "User: ";

        switch (argAlgo)
        {
        case UD_FirstFit:
            UtiliDriven_FirstFitAlgorithm(time);
            break;
        case UD_BestFit:
            UtiliDriven_BestFitAlgorithm(time);
            break;
        case UD_Order_FirstFit:
            us->ReOrderUser_Recursive(0, us->waitingUser.size() - 1);
            UtiliDriven_Ordering_FirstFitAlgorithm(time, argOrder);
            break;
        case UD_Order_BestFit:
            us->ReOrderUser_Recursive(0, us->waitingUser.size() - 1);
            UtiliDriven_Ordering_BestFitAlgorithm(time, argOrder);
            break;
        case MW_SW_FirstFit:
            us->ReOrderUser_Recursive(0, us->waitingUser.size() - 1);
            MaxWindow_Sliding_FirstFit(time, argOrder);
            break;
        case MW_SW_BestFit:
            us->ReOrderUser_Recursive(0, us->waitingUser.size() - 1);
            MaxWindow_Sliding_BestFit(time, argOrder);
            break;
        case FT_SW_BestFit:
            us->ReOrderUser_Recursive(0, us->waitingUser.size() - 1);
            FullTopology_Sliding_BestFit(time, argOrder);
            break;
        case OneQ_SW_FirstFit:
            us->ReOrderUser_Recursive(0, us->waitingUser.size() - 1);
            OneQuarter_Sliding_FirstFit(time, argOrder);
            break;
        case HT_SW_FirstFit:
            us->ReOrderUser_Recursive(0, us->waitingUser.size() - 1);
            HalfTopology_Sliding_FirstFit(time, argOrder);
            break;
        case ThreeQ_SW_FirstFit:
            us->ReOrderUser_Recursive(0, us->waitingUser.size() - 1);
            ThreeQuarter_Sliding_FirstFit(time, argOrder);
            break;
        case FT_SW_FirstFit:
            us->ReOrderUser_Recursive(0, us->waitingUser.size() - 1);
            FullTopology_Sliding_FirstFit(time, argOrder);
            break;
        case SW_BestFit:
            SlidingWindow_BestFit(time, argOrder);
            break;
        case SW_Complete:
            SlidingWindow_Complete(time, argOrder);
            break;
        default:
            break;
        }

        //us->PrintWaitingUser();
        //us->PrintServingUser();

        /*for (unsigned int i = 0; i < fattree->server.size(); i++)
        {
            Server server = fattree->server.at(i);
            output << "server id : " << server.GetID() << " ,";
            output << "cpu : " << server.GetCPU() << " ,"
                << "memory : " << server.GetMemory() << " ,"
                << "bandwidth : " << server.GetBandwidth() << " ,";
            output << "service : ";
            for (auto s : server.serviceInServer){
                output << s->GetID() << ", ";
            }
            output << endl;
        }
        output << endl;*/
        

        //fattree->Print_Server();
        //cout << "Hop violation: " << totalHops << endl;
        //cout << us->waitingUser.size() << endl;
        rejected_Request += us->waitingUser.size();
        CountHopsViolation(3);
        Compute_Server_Load(time);
        FreeEndTimeUser(time);
        

        /*for (unsigned int i = 0; i < fattree->server.size(); i++)
        {
            Server server = fattree->server.at(i);
            output << "server id : " << server.GetID() << " ,";
            output << "cpu : " << server.GetCPU() << " ,"
                << "memory : " << server.GetMemory() << " ,"
                << "bandwidth : " << server.GetBandwidth() << " ,";
            output << "service : ";
            for (auto s : server.serviceInServer){
                output << s->GetID() << ", ";
            }
            output << endl;
        }
        output << endl;*/

        /*for (auto it : us->servingUser){
            output << "user id : " << it->GetID() << endl;
            output << "cpu : " << it->GetCPU() << "\t"
                << "memory : " << it->GetMemory() << "\t"
                << "bw : " << it->bandwidth << endl;
            output << "SL : " << it->service_length << "\t"
                << "hop[1] : " << it->hops[1] << "\t"
                << "hop[2] : " << it->hops[2] << "\t"
                << "start : " << it->startTime << "\t"
                << "sv : " << it->servedTime << "\t"
                << "end : " << it->endTime << endl;
            for (unsigned int i = 0; i < it->service_chain.size(); i++){
                it->service_chain.at(i)->GetID();
                output << "id : " << it->service_chain.at(i)->GetID() << "\t"
                    << "cpu : " << it->service_chain.at(i)->GetCPU() << "\t"
                    << "memory : " << it->service_chain.at(i)->GetMemory() << "\t"
                    << "serverID: " << it->service_chain.at(i)->GetServerID() << endl;
            }
            output << endl;
        }*/

        //output << "\tHops: " << (totalHops - preTotalHops) << "\t" << (violateUser - preViolateUser) << "\t" << (violateHops - preViolateHops) << endl;
    }

	user_data << "end" << endl;
	user_data.close();
    /*output << "Hop_violation: " << GetHops() << endl;
    output << "User: " << GetViolateUser() << "\tHops: " << GetViolateHops() << endl;
    output << "Rejected_Request: " << GetRejected_Request() << endl;
    output << "Immediately_Allocated_Request: " << GetImmediately_Allocated_Request() << endl;
    output << "Total_Allocated_Request: " << GetTotal_Allocated_Request() << endl;*/
    output << GetHops() << ","
        << GetViolateUser() << ","
        << GetViolateHops() << ","
        << GetRejected_Request() << ","
        << GetImmediately_Allocated_Request() << ","
        << GetTotal_Allocated_Request() << ","
        << Get_Average_Server_Load() << endl;
    output.close();
    //cout << Get_Average_Server_Load() << endl;
}

//void Simulation::UtiliDriven_FirstFitAlgorithm(int argTime, ServiceOrder argOrder){
//
//    for (unsigned int i = 0; i < us->waitingUser.size(); i++){
//        shared_ptr<User> user_it = us->waitingUser.at(i);
//
//        for (unsigned int j = 0; j < user_it->service_chain.size(); j++){
//            shared_ptr<Service> service_it = user_it->service_chain.at(j);
//
//            vector<Server>::iterator server_it = fattree->server.begin();
//            while (server_it != fattree->server.end()){
//
//                if (CheckBandwidth(user_it->bandwidth, server_it->GetBandwidth()) && CheckService(service_it, server_it)){//server has enough bandwidth, cpu, memory
//
//                    server_it->SetActive(1);	//start server
//
//                    //put user into server
//                    server_it->userBandwidth.insert(pair<int, double>(user_it->GetID(), user_it->bandwidth));
//
//                    //put service into server
//                    service_it->SetServerID(server_it->GetID());
//                    server_it->serviceInServer.push_back(service_it);
//
//                    //update info(CPU,Memory,Bandwidth) of server
//                    server_it->UpdateServerBandwidth();
//                    server_it->UpdateServerCPUandMemory();
//
//                    //cout << "user id: " << user_it->GetID() << "\tservice id: " << service_it->GetID() << "\tserver id: " << server_it->GetID() << endl;
//                    //cout << server_it->GetCPU() << "\t" << server_it->GetMemory() << "\t" << server_it->GetBandwidth() << endl;
//                    break;
//                }//end if checkbandwidth and checkservice
//                else{
//                    server_it++;
//                }
//            }//end for server
//
//            if (server_it == fattree->server.end()){//this service cannot put into any server
//                //cout << "This service cannot put into any server!!!" << endl;
//                break;
//            }
//        }//end for service
//
//        //check services of user are served at the same time, if not => reset services
//        int countUnServedService = user_it->CountUnServedService();
//        if (countUnServedService == 0){//all service are served
//
//            user_it->CountHops(fattree->server);
//            user_it->servedTime = argTime;
//            user_it->CheckServeTime(onTimeServedUser);
//            us->servingUser.push_back(user_it);
//            us->waitingUser.erase(us->waitingUser.begin() + i);
//            i--;
//        }
//        else{//user is unserved
//            if (countUnServedService != user_it->service_length){
//                ResetService(user_it);
//            }
//        }
//    }
//}

void Simulation::UtiliDriven_FirstFitAlgorithm(int argTime){

    for (unsigned int i = 0; i < us->waitingUser.size(); i++){

        shared_ptr<User> user_it = us->waitingUser.at(i);
        bool answer = false;

        vector<int> reOrderVec(numOfServices.begin(), numOfServices.begin() + user_it->service_chain.size());
        vector<shared_ptr<Service>> copyServiceChain(user_it->service_chain);
        vector<int> answerVec(user_it->service_chain.size(), 0);

        /*for (unsigned index = 0; index <reOrderVec.size(); index++)
        {
            cout << reOrderVec.at(index) << "\t";
        }*/

        answer = FindAnswer_FirstFit(user_it, reOrderVec, answerVec, fattree->server.size());

        if (answer){
            //put the answer into server
            PutServiceIntoServer(user_it, answerVec);
            Add_A_Count_Of_Total_Allocated_Request();

#ifdef ORIG
            user_it->CountHops(fattree->server);
#else
			user_it->CountHopsNew(fattree->server, fattree->K, 0, fattree->server.size());
#endif
            //cout << "user: " << user_it->GetID() << "\thops[1]: " << user_it->hops[1] << "\thops[2]: " << user_it->hops[2] << endl;
            user_it->servedTime = argTime;
            user_it->CheckServeTime(immediately_Allocated_Request);
            //user_it->SetServed(SERVING);
            us->servingUser.push_back(user_it);
            us->waitingUser.erase(us->waitingUser.begin() + i);
            i--;

			for (unsigned int i = 0; i < answerVec.size(); i++) {
				vector<Server>::iterator it = fattree->server.begin() + answerVec.at(i);
				it->services.clear();
			}
        }
        else{
            continue;
        }
    }
}

void Simulation::UtiliDriven_Ordering_FirstFitAlgorithm(int argTime, ServiceOrder argOrder){

    for (unsigned int i = 0; i < us->waitingUser.size(); i++){

        shared_ptr<User> user_it = us->waitingUser.at(i);
        bool answer = false;

        vector<int> reOrderVec(numOfServices.begin(), numOfServices.begin() + user_it->service_chain.size());
        vector<shared_ptr<Service>> copyServiceChain(user_it->service_chain);
        vector<int> answerVec(user_it->service_chain.size(), 0);

        switch (argOrder)
        {
        case SmallToLarge:
            SmallToLarge_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case LargeToSmall:
            LargeToSmall_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case Random:
            Random_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case UnOrder:
            break;
        default:
            break;
        }

        answer = FindAnswer_FirstFit(user_it, reOrderVec, answerVec, fattree->server.size());

        //for (unsigned int j = 0; j < user_it->service_chain.size(); j++){
        //    shared_ptr<Service> service_it = user_it->service_chain.at(j);
        //    vector<Server>::iterator server_it = fattree->server.begin();
        //    while (server_it != fattree->server.end()){
        //        if (CheckBandwidth(user_it->bandwidth, server_it->GetBandwidth()) && CheckService(service_it, server_it)){//server has enough bandwidth, cpu, memory
        //            server_it->SetActive(1);	//start server
        //            //put user into server
        //            server_it->userBandwidth.insert(pair<int, double>(user_it->GetID(), user_it->bandwidth));
        //            //put service into server
        //            service_it->SetServerID(server_it->GetID());
        //            server_it->serviceInServer.push_back(service_it);
        //            //update info(CPU,Memory,Bandwidth) of server
        //            server_it->UpdateServerBandwidth();
        //            server_it->UpdateServerCPUandMemory();
        //            //cout << "user id: " << user_it->GetID() << "\tservice id: " << service_it->GetID() << "\tserver id: " << server_it->GetID() << endl;
        //            //cout << server_it->GetCPU() << "\t" << server_it->GetMemory() << "\t" << server_it->GetBandwidth() << endl;
        //            break;
        //        }//end if checkbandwidth and checkservice
        //        else{
        //            server_it++;
        //        }
        //    }//end for server
        //    if (server_it == fattree->server.end()){//this service cannot put into any server
        //        //cout << "This service cannot put into any server!!!" << endl;
        //        break;
        //    }
        //}//end for service

        if (answer){

            //put the answer into server
            PutServiceIntoServer(user_it, answerVec);
            Add_A_Count_Of_Total_Allocated_Request();
#ifdef ORIG
			user_it->CountHops(fattree->server);
#else
			user_it->CountHopsNew(fattree->server, fattree->K, 0, fattree->server.size());
#endif
            //cout << "user: " << user_it->GetID() << "\thops[1]: " << user_it->hops[1] << "\thops[2]: " << user_it->hops[2] << endl;
            user_it->servedTime = argTime;
            user_it->CheckServeTime(immediately_Allocated_Request);
            //user_it->SetServed(SERVING);
            us->servingUser.push_back(user_it);
            us->waitingUser.erase(us->waitingUser.begin() + i);
            i--;

			for (unsigned int i = 0; i < answerVec.size(); i++) {
				vector<Server>::iterator it = fattree->server.begin() + answerVec.at(i);
				it->services.clear();
			}
        }
        else{
            continue;
        }
    }
}

void Simulation::UtiliDriven_BestFitAlgorithm(int argTime){
    //unsigned windowSize = 1;
    for (unsigned int i = 0; i < us->waitingUser.size(); i++)
    {
        shared_ptr<User> user_it = us->waitingUser.at(i);
        bool answer = false;

        //unsigned int windowSize = (fattree->GetK() / 2)*(fattree->GetK() / 2);  //Intra Pod
        //unsigned int windowSize = fattree->server.size() / 2;                   //Half Topology
        unsigned int windowSize = fattree->server.size();                       //Full Topology

        vector<int> reOrderVec(numOfServices.begin(), numOfServices.begin() + user_it->service_chain.size());
        vector<shared_ptr<Service>> copyServiceChain(user_it->service_chain);
        vector<int> answerVec(user_it->service_chain.size(), 0);

        unsigned int maxServerNumber = fattree->server.size() - windowSize + 1;
		int start_serv = -1;
        for (unsigned int s = 0; s < maxServerNumber; s++)
        {
            answer = FindWindowToServe_SLA(user_it, reOrderVec, answerVec, s, windowSize);
            if (answer){
				start_serv = s;
                break;
            }
        }

        if (answer){

            //put the answer into server
            PutServiceIntoServer(user_it, answerVec);
            Add_A_Count_Of_Total_Allocated_Request();
#ifdef ORIG
			user_it->CountHops(fattree->server);
#else
			user_it->CountHopsNew(fattree->server, fattree->K, start_serv, windowSize);
#endif
			
            //cout << "user: " << user_it->GetID() << "\thops[1]: " << user_it->hops[1] << "\thops[2]: " << user_it->hops[2] << endl;
            user_it->servedTime = argTime;
            user_it->CheckServeTime(immediately_Allocated_Request);
            //user_it->SetServed(SERVING);
            us->servingUser.push_back(user_it);
            us->waitingUser.erase(us->waitingUser.begin() + i);
            i--;

			for (unsigned int i = 0; i < answerVec.size(); i++) {
				vector<Server>::iterator it = fattree->server.begin() + answerVec.at(i);
				it->services.clear();
			}
        }
        else{
            continue;
        }
    }
}

void Simulation::UtiliDriven_Ordering_BestFitAlgorithm(int argTime, ServiceOrder argOrder){
    //unsigned windowSize = 1;
    for (unsigned int i = 0; i < us->waitingUser.size(); i++)
    {
        shared_ptr<User> user_it = us->waitingUser.at(i);
        bool answer = false;

        //unsigned int windowSize = (fattree->GetK() / 2)*(fattree->GetK() / 2);  //Intra Pod
        //unsigned int windowSize = fattree->server.size() / 2;                   //Half Topology
        unsigned int windowSize = fattree->server.size();                       //Full Topology

        vector<int> reOrderVec(numOfServices.begin(), numOfServices.begin() + user_it->service_chain.size());
        vector<shared_ptr<Service>> copyServiceChain(user_it->service_chain);
        vector<int> answerVec(user_it->service_chain.size(), 0);

        switch (argOrder)
        {
        case SmallToLarge:
            SmallToLarge_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case LargeToSmall:
            LargeToSmall_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case Random:
            Random_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case UnOrder:
            break;
        default:
            break;
        }

        unsigned int maxServerNumber = fattree->server.size() - windowSize + 1;
		int start_serv = -1;
        for (unsigned int s = 0; s < maxServerNumber; s++)
        {
            answer = FindWindowToServe_SLA(user_it, reOrderVec, answerVec, s, windowSize);
            if (answer){
				start_serv = s;
                break;
            }
        }

        if (answer){
            /*for (unsigned qq = 0; qq < answerVec.size(); qq++)
            {
            cout << answerVec.at(qq) << "\t";
            }
            cout << endl;*/

            //put the answer into server
            PutServiceIntoServer(user_it, answerVec);
            Add_A_Count_Of_Total_Allocated_Request();
            
#ifdef ORIG
			user_it->CountHops(fattree->server);
#else
			user_it->CountHopsNew(fattree->server, fattree->K, start_serv, windowSize);
#endif

            //cout << "user: " << user_it->GetID() << "\thops[1]: " << user_it->hops[1] << "\thops[2]: " << user_it->hops[2] << endl;
            user_it->servedTime = argTime;
            user_it->CheckServeTime(immediately_Allocated_Request);
            //user_it->SetServed(SERVING);
            us->servingUser.push_back(user_it);
            us->waitingUser.erase(us->waitingUser.begin() + i);
            i--;

			for (unsigned int i = 0; i < answerVec.size(); i++) {
				vector<Server>::iterator it = fattree->server.begin() + answerVec.at(i);
				it->services.clear();
			}
        }
        else{
            continue;
        }
    }
}

void Simulation::MaxWindow_Sliding_FirstFit(int argTime, ServiceOrder argOrder){

    for (unsigned int i = 0; i < us->waitingUser.size(); i++){

        shared_ptr<User> user_it = us->waitingUser.at(i);
        bool answer = false;
        int maxWindowSize = DecideWindowSize(user_it);
        int serverNumForUser_CPU = int(ceil(user_it->GetCPU() / 100.0));
        int serverNumForUser_Memory = int(ceil(user_it->GetMemory() / 100.0));
        int windowSize = max(serverNumForUser_CPU, serverNumForUser_Memory);
        //int windowSize = maxWindowSize;

        vector<int> reOrderVec(numOfServices.begin(), numOfServices.begin() + user_it->service_chain.size());
        vector<shared_ptr<Service>> copyServiceChain(user_it->service_chain);
        vector<int> tmpVec(user_it->service_chain.size(), 0);
        vector<int> answerVec;

        switch (argOrder)
        {
        case SmallToLarge:
            SmallToLarge_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case LargeToSmall:
            LargeToSmall_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case Random:
            Random_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case UnOrder:
            break;
        default:
            break;
        }
		int start_serv = -1;
        while (windowSize <= maxWindowSize)
        {
            int minimumHop = 10000;
            unsigned int maxServerNumber = fattree->server.size() - windowSize + 1;
            for (unsigned int s = 0; s < maxServerNumber; s++)
            {
                bool as = FindWindowToServe_SlidingFirstFit(user_it, reOrderVec, tmpVec, s, windowSize);
                if (as)
                {
#ifdef ORIG
					int tmpHopValue = CountHops(tmpVec);
#else
					int tmpHopValue = user_it->PredictHopsNew(fattree->server, fattree->K, s, windowSize);
#endif
                    if (minimumHop > tmpHopValue)
                    {
                        answerVec = tmpVec;
                        minimumHop = tmpHopValue;
						start_serv = s;
                        answer = true;
                    }
                }
            }

            if (answer){
                break;
            }
            else{
                windowSize++;
            }
        }

        if (answer){
            //put the answer into server
            PutServiceIntoServer(user_it, answerVec);
            Add_A_Count_Of_Total_Allocated_Request();
			
#ifdef ORIG
			user_it->CountHops(fattree->server);
#else
			user_it->CountHopsNew(fattree->server, fattree->K, start_serv, windowSize);
#endif

            //cout << "user: " << user_it->GetID() << "\thops[1]: " << user_it->hops[1] << "\thops[2]: " << user_it->hops[2] << endl;
            user_it->servedTime = argTime;
            user_it->CheckServeTime(immediately_Allocated_Request);
            //user_it->SetServed(SERVING);
            us->servingUser.push_back(user_it);
            us->waitingUser.erase(us->waitingUser.begin() + i);
            i--;

			for (unsigned int j = 0; j < answerVec.size(); j++) {
				vector<Server>::iterator it = fattree->server.begin() + answerVec.at(j);
				it->services.clear();
			}
        }
        else{
            continue;
        }
    }
}

void Simulation::MaxWindow_Sliding_BestFit(int argTime, ServiceOrder argOrder){
    for (unsigned int i = 0; i < us->waitingUser.size(); i++)
    {
        shared_ptr<User> user_it = us->waitingUser.at(i);
        bool answer = false;
        int maxWindowSize = DecideWindowSize(user_it);
        //cout << maxWindowSize << endl;
        //output << maxWindowSize << endl;

        int serverNumForUser_CPU = int(ceil(user_it->GetCPU() / 100.0));
        int serverNumForUser_Memory = int(ceil(user_it->GetMemory() / 100.0));
        int windowSize = max(serverNumForUser_CPU, serverNumForUser_Memory);
        //int windowSize = maxWindowSize / 2;

        vector<int> reOrderVec(numOfServices.begin(), numOfServices.begin() + user_it->service_chain.size());
        vector<shared_ptr<Service>> copyServiceChain(user_it->service_chain);
        vector<int> tmpVec(user_it->service_chain.size(), 0);
        vector<int> answerVec;

        switch (argOrder)
        {
        case SmallToLarge:
            SmallToLarge_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case LargeToSmall:
            LargeToSmall_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case Random:
            Random_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case UnOrder:
            break;
        default:
            break;
        }
		int start_serv = -1;
        while (windowSize <= maxWindowSize)
        {
            int minimumHop = 10000;
            unsigned int maxServerNumber = fattree->server.size() - windowSize + 1;
			
            for (unsigned int s = 0; s < maxServerNumber; s++)
            {
                bool as = FindWindowToServe_SLA(user_it, reOrderVec, tmpVec, s, windowSize);
                if (as)
                {
#ifdef ORIG
					int tmpHopValue = CountHops(tmpVec);
#else
					int tmpHopValue = user_it->PredictHopsNew(fattree->server, fattree->K, s, windowSize);
#endif
                    if (minimumHop > tmpHopValue)
                    {
                        answerVec = tmpVec;
                        minimumHop = tmpHopValue;
                        answer = true;
						start_serv = s;
                    }
                }
            }

            if (answer){
                break;
            }
            else{
                windowSize++;
            }
        }
        //cout << user_it->hops[1] << "\t" << minimumHop << endl;

        if (answer){
            //put the answer into server
            PutServiceIntoServer(user_it, answerVec);
            Add_A_Count_Of_Total_Allocated_Request();

#ifdef ORIG
			user_it->CountHops(fattree->server);
#else
			user_it->CountHopsNew(fattree->server, fattree->K, start_serv, windowSize);
#endif
            
            //cout << "user: " << user_it->GetID() << "\thops[1]: " << user_it->hops[1] << "\thops[2]: " << user_it->hops[2] << endl;
            user_it->servedTime = argTime;
            user_it->CheckServeTime(immediately_Allocated_Request);
            //user_it->SetServed(SERVING);
            us->servingUser.push_back(user_it);
            us->waitingUser.erase(us->waitingUser.begin() + i);
            i--;

			for (unsigned int i = 0; i < answerVec.size(); i++) {
				vector<Server>::iterator it = fattree->server.begin() + answerVec.at(i);
				it->services.clear();
			}
        }
        else{
            continue;
        }
    }
}

void Simulation::FullTopology_Sliding_BestFit(int argTime, ServiceOrder argOrder){
    for (unsigned int i = 0; i < us->waitingUser.size(); i++)
    {
        shared_ptr<User> user_it = us->waitingUser.at(i);
        bool answer = false;

        int maxWindowSize = fattree->server.size();
        int serverNumForUser_CPU = int(ceil(user_it->GetCPU() / 100.0));
        int serverNumForUser_Memory = int(ceil(user_it->GetMemory() / 100.0));
        int windowSize = max(serverNumForUser_CPU, serverNumForUser_Memory);

        vector<int> reOrderVec(numOfServices.begin(), numOfServices.begin() + user_it->service_chain.size());
        vector<shared_ptr<Service>> copyServiceChain(user_it->service_chain);
        vector<int> tmpVec(user_it->service_chain.size(), 0);
        vector<int> answerVec;

        switch (argOrder)
        {
        case SmallToLarge:
            SmallToLarge_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case LargeToSmall:
            LargeToSmall_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case Random:
            Random_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case UnOrder:
            break;
        default:
            break;
        }

		int start_serv = -1;
        while (windowSize <= maxWindowSize)
        {
            int minimumHop = 10000;
            unsigned int maxServerNumber = fattree->server.size() - windowSize + 1;
            for (unsigned int s = 0; s < maxServerNumber; s++)
            {
                bool as = FindWindowToServe_SLA(user_it, reOrderVec, tmpVec, s, windowSize);
                if (as)
                {
#ifdef ORIG
					int tmpHopValue = CountHops(tmpVec);
#else
					int tmpHopValue = user_it->PredictHopsNew(fattree->server, fattree->K, s, windowSize);
#endif
					
                    if (minimumHop > tmpHopValue)
                    {
                        answerVec = tmpVec;
                        minimumHop = tmpHopValue;
                        answer = true;
						start_serv = s;
                    }
                }
            }

            if (answer){
                break;
            }
            else{
                windowSize++;
            }
        }

        if (answer){
            //put the answer into server
            PutServiceIntoServer(user_it, answerVec);
            Add_A_Count_Of_Total_Allocated_Request();

#ifdef ORIG
			user_it->CountHops(fattree->server);
#else
			user_it->CountHopsNew(fattree->server, fattree->K, start_serv, windowSize);
#endif

            //cout << "user: " << user_it->GetID() << "\thops[1]: " << user_it->hops[1] << "\thops[2]: " << user_it->hops[2] << endl;
            user_it->servedTime = argTime;
            user_it->CheckServeTime(immediately_Allocated_Request);
            //user_it->SetServed(SERVING);
            us->servingUser.push_back(user_it);
            us->waitingUser.erase(us->waitingUser.begin() + i);
            i--;

			for (unsigned int i = 0; i < answerVec.size(); i++) {
				vector<Server>::iterator it = fattree->server.begin() + answerVec.at(i);
				it->services.clear();
			}
        }
        else{
            continue;
        }
    }
}


void Simulation::OneQuarter_Sliding_FirstFit(int argTime, ServiceOrder argOrder){

    for (unsigned int i = 0; i < us->waitingUser.size(); i++){

        shared_ptr<User> user_it = us->waitingUser.at(i);
        bool answer = false;
        int maxWindowSize = fattree->server.size() / 4;
        int windowSize = maxWindowSize;

        vector<int> reOrderVec(numOfServices.begin(), numOfServices.begin() + user_it->service_chain.size());
        vector<shared_ptr<Service>> copyServiceChain(user_it->service_chain);
        vector<int> tmpVec(user_it->service_chain.size(), 0);
        vector<int> answerVec;

        switch (argOrder)
        {
        case SmallToLarge:
            SmallToLarge_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case LargeToSmall:
            LargeToSmall_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case Random:
            Random_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case UnOrder:
            break;
        default:
            break;
        }

		int start_serv = -1;
        while (windowSize <= maxWindowSize)
        {
            int minimumHop = 10000;
            unsigned int maxServerNumber = fattree->server.size() - windowSize + 1;
            for (unsigned int s = 0; s < maxServerNumber; s++)
            {
                bool as = FindWindowToServe_SlidingFirstFit(user_it, reOrderVec, tmpVec, s, windowSize);
                if (as)
                {
#ifdef ORIG
					int tmpHopValue = CountHops(tmpVec);
#else
					int tmpHopValue = user_it->PredictHopsNew(fattree->server, fattree->K, s, windowSize);
#endif
                    if (minimumHop > tmpHopValue)
                    {
                        answerVec = tmpVec;
                        minimumHop = tmpHopValue;
                        answer = true;
						start_serv = s;
                    }
                }
            }

            if (answer){
                break;
            }
            else{
                windowSize++;
            }
        }

        if (answer){
            //put the answer into server
            PutServiceIntoServer(user_it, answerVec);
            Add_A_Count_Of_Total_Allocated_Request();

#ifdef ORIG
			user_it->CountHops(fattree->server);
#else
			user_it->CountHopsNew(fattree->server, fattree->K, start_serv, windowSize);
#endif
            //cout << "user: " << user_it->GetID() << "\thops[1]: " << user_it->hops[1] << "\thops[2]: " << user_it->hops[2] << endl;
            user_it->servedTime = argTime;
            user_it->CheckServeTime(immediately_Allocated_Request);
            //user_it->SetServed(SERVING);
            us->servingUser.push_back(user_it);
            us->waitingUser.erase(us->waitingUser.begin() + i);
            i--;

			for (unsigned int i = 0; i < answerVec.size(); i++) {
				vector<Server>::iterator it = fattree->server.begin() + answerVec.at(i);
				it->services.clear();
			}
        }
        else{
            continue;
        }
    }
}

void Simulation::HalfTopology_Sliding_FirstFit(int argTime, ServiceOrder argOrder){

    for (unsigned int i = 0; i < us->waitingUser.size(); i++){

        shared_ptr<User> user_it = us->waitingUser.at(i);
        bool answer = false;
        int maxWindowSize = fattree->server.size() / 2;
        int windowSize = maxWindowSize;

        vector<int> reOrderVec(numOfServices.begin(), numOfServices.begin() + user_it->service_chain.size());
        vector<shared_ptr<Service>> copyServiceChain(user_it->service_chain);
        vector<int> tmpVec(user_it->service_chain.size(), 0);
        vector<int> answerVec;

        switch (argOrder)
        {
        case SmallToLarge:
            SmallToLarge_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case LargeToSmall:
            LargeToSmall_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case Random:
            Random_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case UnOrder:
            break;
        default:
            break;
        }

		int start_serv = -1;
        while (windowSize <= maxWindowSize)
        {
            int minimumHop = 10000;
            unsigned int maxServerNumber = fattree->server.size() - windowSize + 1;
            for (unsigned int s = 0; s < maxServerNumber; s++)
            {
                bool as = FindWindowToServe_SlidingFirstFit(user_it, reOrderVec, tmpVec, s, windowSize);
                if (as)
                {
#ifdef ORIG
					int tmpHopValue = CountHops(tmpVec);
#else
					int tmpHopValue = user_it->PredictHopsNew(fattree->server, fattree->K, s, windowSize);
#endif
					
                    if (minimumHop > tmpHopValue)
                    {
                        answerVec = tmpVec;
                        minimumHop = tmpHopValue;
                        answer = true;
						start_serv = s;
                    }
                }
            }

            if (answer){
                break;
            }
            else{
                windowSize++;
            }
        }

        if (answer){
            //put the answer into server
            PutServiceIntoServer(user_it, answerVec);
            Add_A_Count_Of_Total_Allocated_Request();
#ifdef ORIG
			user_it->CountHops(fattree->server);
#else
			user_it->CountHopsNew(fattree->server, fattree->K, start_serv, windowSize);
#endif

            //cout << "user: " << user_it->GetID() << "\thops[1]: " << user_it->hops[1] << "\thops[2]: " << user_it->hops[2] << endl;
            user_it->servedTime = argTime;
            user_it->CheckServeTime(immediately_Allocated_Request);
            //user_it->SetServed(SERVING);
            us->servingUser.push_back(user_it);
            us->waitingUser.erase(us->waitingUser.begin() + i);
            i--;

			for (unsigned int i = 0; i < answerVec.size(); i++) {
				vector<Server>::iterator it = fattree->server.begin() + answerVec.at(i);
				it->services.clear();
			}
        }
        else{
            continue;
        }
    }
}

void Simulation::ThreeQuarter_Sliding_FirstFit(int argTime, ServiceOrder argOrder){

    for (unsigned int i = 0; i < us->waitingUser.size(); i++){

        shared_ptr<User> user_it = us->waitingUser.at(i);
        bool answer = false;
        int maxWindowSize = fattree->server.size() / 4 * 3;
        int windowSize = maxWindowSize;

        vector<int> reOrderVec(numOfServices.begin(), numOfServices.begin() + user_it->service_chain.size());
        vector<shared_ptr<Service>> copyServiceChain(user_it->service_chain);
        vector<int> tmpVec(user_it->service_chain.size(), 0);
        vector<int> answerVec;

        switch (argOrder)
        {
        case SmallToLarge:
            SmallToLarge_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case LargeToSmall:
            LargeToSmall_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case Random:
            Random_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case UnOrder:
            break;
        default:
            break;
        }

		int start_serv = -1;
        while (windowSize <= maxWindowSize)
        {
            int minimumHop = 10000;
            unsigned int maxServerNumber = fattree->server.size() - windowSize + 1;
            for (unsigned int s = 0; s < maxServerNumber; s++)
            {
                bool as = FindWindowToServe_SlidingFirstFit(user_it, reOrderVec, tmpVec, s, windowSize);
                if (as)
                {
#ifdef ORIG
					int tmpHopValue = CountHops(tmpVec);
#else
					int tmpHopValue = user_it->PredictHopsNew(fattree->server, fattree->K, s, windowSize);
#endif
                    if (minimumHop > tmpHopValue)
                    {
                        answerVec = tmpVec;
                        minimumHop = tmpHopValue;
                        answer = true;
						start_serv = s;
                    }
                }
            }

            if (answer){
                break;
            }
            else{
                windowSize++;
            }
        }

        if (answer){
            //put the answer into server
            PutServiceIntoServer(user_it, answerVec);
            Add_A_Count_Of_Total_Allocated_Request();

#ifdef ORIG
			user_it->CountHops(fattree->server);
#else
			user_it->CountHopsNew(fattree->server, fattree->K, start_serv, windowSize);
#endif
			

            //cout << "user: " << user_it->GetID() << "\thops[1]: " << user_it->hops[1] << "\thops[2]: " << user_it->hops[2] << endl;
            user_it->servedTime = argTime;
            user_it->CheckServeTime(immediately_Allocated_Request);
            //user_it->SetServed(SERVING);
            us->servingUser.push_back(user_it);
            us->waitingUser.erase(us->waitingUser.begin() + i);
            i--;

			for (unsigned int i = 0; i < answerVec.size(); i++) {
				vector<Server>::iterator it = fattree->server.begin() + answerVec.at(i);
				it->services.clear();
			}
        }
        else{
            continue;
        }
    }
}

void Simulation::FullTopology_Sliding_FirstFit(int argTime, ServiceOrder argOrder){

    for (unsigned int i = 0; i < us->waitingUser.size(); i++){

        shared_ptr<User> user_it = us->waitingUser.at(i);
        bool answer = false;
        int maxWindowSize = fattree->server.size();
        int windowSize = maxWindowSize;

        vector<int> reOrderVec(numOfServices.begin(), numOfServices.begin() + user_it->service_chain.size());
        vector<shared_ptr<Service>> copyServiceChain(user_it->service_chain);
        vector<int> tmpVec(user_it->service_chain.size(), 0);
        vector<int> answerVec;

        switch (argOrder)
        {
        case SmallToLarge:
            SmallToLarge_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case LargeToSmall:
            LargeToSmall_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case Random:
            Random_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case UnOrder:
            break;
        default:
            break;
        }

		int start_serv = -1;
        while (windowSize <= maxWindowSize)
        {
            int minimumHop = 10000;
            unsigned int maxServerNumber = fattree->server.size() - windowSize + 1;
            for (unsigned int s = 0; s < maxServerNumber; s++)
            {
                bool as = FindWindowToServe_SlidingFirstFit(user_it, reOrderVec, tmpVec, s, windowSize);
                if (as)
				{
#ifdef ORIG
					int tmpHopValue = CountHops(tmpVec);
#else
					int tmpHopValue = user_it->PredictHopsNew(fattree->server, fattree->K, s, windowSize);
#endif
                   
                    if (minimumHop > tmpHopValue)
                    {
                        answerVec = tmpVec;
                        minimumHop = tmpHopValue;
                        answer = true;
						start_serv = s;
                    }
                }
            }

            if (answer){
                break;
            }
            else{
                windowSize++;
            }
        }

        if (answer){
            //put the answer into server
            PutServiceIntoServer(user_it, answerVec);
            Add_A_Count_Of_Total_Allocated_Request();
          
#ifdef ORIG
			user_it->CountHops(fattree->server);
#else
			user_it->CountHopsNew(fattree->server, fattree->K, start_serv, windowSize);
#endif
            //cout << "user: " << user_it->GetID() << "\thops[1]: " << user_it->hops[1] << "\thops[2]: " << user_it->hops[2] << endl;
            user_it->servedTime = argTime;
            user_it->CheckServeTime(immediately_Allocated_Request);
            //user_it->SetServed(SERVING);
            us->servingUser.push_back(user_it);
            us->waitingUser.erase(us->waitingUser.begin() + i);
            i--;

			for (unsigned int i = 0; i < answerVec.size(); i++) {
				vector<Server>::iterator it = fattree->server.begin() + answerVec.at(i);
				it->services.clear();
			}
        }
        else{
            continue;
        }
    }
}


void Simulation::SlidingWindow_BestFit(int argTime, ServiceOrder argOrder){
    for (unsigned int i = 0; i < us->waitingUser.size(); i++)
    {
        shared_ptr<User> user_it = us->waitingUser.at(i);
        bool answer = false;
        int maxWindowSize = DecideWindowSize(user_it);
        int serverNumForUser_CPU = int(ceil(user_it->GetCPU() / 100.0));
        int serverNumForUser_Memory = int(ceil(user_it->GetMemory() / 100.0));
        int windowSize = max(serverNumForUser_CPU, serverNumForUser_Memory);

        vector<int> reOrderVec(numOfServices.begin(), numOfServices.begin() + user_it->service_chain.size());
        vector<shared_ptr<Service>> copyServiceChain(user_it->service_chain);
        vector<int> answerVec(user_it->service_chain.size(), 0);

        /*for (unsigned int pp = 0; pp < reOrderVec.size(); pp++)
        {
        cout << reOrderVec.at(pp) << "\t";
        }
        cout << endl;
        for (unsigned int pp = 0; pp < user_it->service_chain.size(); pp++)
        {
        cout << user_it->service_chain.at(pp)->GetSumCPUMemory() << "\t";
        }
        cout << endl;*/

        switch (argOrder)
        {
        case SmallToLarge:
            SmallToLarge_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case LargeToSmall:
            LargeToSmall_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case Random:
            Random_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case UnOrder:
            break;
        default:
            break;
        }

        /*for (unsigned int pp = 0; pp < reOrderVec.size(); pp++)
        {
            cout << reOrderVec.at(pp) << "\t";
        }
        cout << endl;
        for (unsigned int pp = 0; pp < user_it->service_chain.size(); pp++)
        {
            cout << user_it->service_chain.at(pp)->GetSumCPUMemory() << "\t";
        }
        cout << endl;*/

		int start_serv = -1;
        while (windowSize <= maxWindowSize)
        {
            //cout << windowSize << "\t" << maxWindowSize << endl;
            unsigned int maxServerNumber = fattree->server.size() - windowSize + 1;
            //cout << maxServerNumber << endl;
            for (unsigned int s = 0; s < maxServerNumber; s++)
            {
                answer = FindWindowToServe_SlidingBesFit(user_it, reOrderVec, answerVec, s, windowSize);
                if (answer){
					start_serv = s;
                    break;
                }
            }
            if (answer){
                break;
            }
            else{
                windowSize++;
            }
        }
        if (answer){
            //put the answer into server
            PutServiceIntoServer(user_it, answerVec);
#ifdef ORIG
			user_it->CountHops(fattree->server);
#else
			user_it->CountHopsNew(fattree->server, fattree->K, start_serv, windowSize);
#endif

            //cout << "user: " << user_it->GetID() << "\thops[1]: " << user_it->hops[1] << "\thops[2]: " << user_it->hops[2] << endl;
            user_it->servedTime = argTime;
            user_it->CheckServeTime(immediately_Allocated_Request);
            //user_it->SetServed(SERVING);
            us->servingUser.push_back(user_it);
            us->waitingUser.erase(us->waitingUser.begin() + i);
            i--;

			for (unsigned int i = 0; i < answerVec.size(); i++) {
				vector<Server>::iterator it = fattree->server.begin() + answerVec.at(i);
				it->services.clear();
			}
        }
        else{
            continue;
        }
    }
}

void Simulation::SlidingWindow_Complete(int argTime, ServiceOrder argOrder){
    for (unsigned int i = 0; i < us->waitingUser.size(); i++)
    {
        shared_ptr<User> user_it = us->waitingUser.at(i);
        bool answer = false;
        //int maxWindowSize = DecideWindowSize(user_it);
        int maxWindowSize = fattree->server.size();
        int serverNumForUser_CPU = int(ceil(user_it->GetCPU() / 100.0));
        int serverNumForUser_Memory = int(ceil(user_it->GetMemory() / 100.0));
        int windowSize = max(serverNumForUser_CPU, serverNumForUser_Memory);

        vector<int> reOrderVec(numOfServices.begin(), numOfServices.begin() + user_it->service_chain.size());
        vector<shared_ptr<Service>> copyServiceChain(user_it->service_chain);
        vector<int> answerVec(user_it->service_chain.size(), 0);
        vector<int> minHopAnswerVec;
        int answerHops = 10000;

        switch (argOrder)
        {
        case SmallToLarge:
            SmallToLarge_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case LargeToSmall:
            LargeToSmall_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        case Random:
            Random_Sort_Recursive(copyServiceChain, reOrderVec, 0, user_it->service_chain.size() - 1);
            break;
        default:
            break;
        }

		int start_serv = -1;
        while (windowSize < maxWindowSize)
        {
            //cout << windowSize << "\t" << maxWindowSize << endl;
            unsigned int maxServerNumber = fattree->server.size() - windowSize + 1;
            for (unsigned int s = 0; s < maxServerNumber; s++)
            {
                answer = FindWindowToServe_SlidingBesFit(user_it, reOrderVec, answerVec, s, windowSize);
                if (answer){
					start_serv = s;
#ifdef ORIG
					int temp = CountHops(answerVec);
#else
					int temp = user_it->PredictHopsNew(fattree->server, fattree->K, s, windowSize);
#endif
                    /*for (unsigned index = 0; index < answerVec.size(); index++)
                    {
                    cout << answerVec.at(index) << "\t";
                    }
                    cout << "hops: " << temp << endl;*/

                    if (temp < answerHops){
                        answerHops = temp;
                        minHopAnswerVec = answerVec;
                        /*for (unsigned index = 0; index < answerVec.size(); index++)
                        {
                        cout << answerVec.at(index) << "\t";
                        }
                        cout << "hops: " << temp << endl;*/
                    }
                }
            }
            windowSize++;
        }
        if (answerHops < 10000){
            //put the answer into server
            PutServiceIntoServer(user_it, minHopAnswerVec);
#ifdef ORIG
			user_it->CountHops(fattree->server);
#else
			user_it->CountHopsNew(fattree->server, fattree->K, start_serv, windowSize);
#endif

            //cout << "user: " << user_it->GetID() << "\thops[1]: " << user_it->hops[1] << "\thops[2]: " << user_it->hops[2] << endl;
            user_it->servedTime = argTime;
            user_it->CheckServeTime(immediately_Allocated_Request);
            //user_it->SetServed(SERVING);
            us->servingUser.push_back(user_it);
            us->waitingUser.erase(us->waitingUser.begin() + i);
            i--;

			for (unsigned int i = 0; i < answerVec.size(); i++) {
				vector<Server>::iterator it = fattree->server.begin() + answerVec.at(i);
				it->services.clear();
			}
        }
        else{
            continue;
        }
    }
}

bool Simulation::CheckService(shared_ptr<Service> argService, vector<Server>::iterator argServer){
    if (CheckCPU(argService->GetCPU(), argServer->GetCPU()) && CheckMemory(argService->GetMemory(), argServer->GetMemory())){
        return true;
    }
    else{
        return false;
    }
}

bool Simulation::CheckAllConstraintIsOkay(shared_ptr<User> argUser, vector<int> currVec, int numOfCheckService){

    int hops = 0;
    map<int, int> serviceMap;
    map<int, int>::iterator it;
    for (int i = 0; i < numOfCheckService; i++){
        it = serviceMap.find(currVec.at(i));
        if (it != serviceMap.end()){
            it->second++;
        }
        else{
            serviceMap.insert(pair<int, int>(currVec.at(i), 1));
        }
        //count hops
        if (i != 0){
            Server preS = fattree->server.at(currVec.at(i - 1));
            Server s = fattree->server.at(currVec.at(i));
            if (preS.GetID() == s.GetID()){
                hops += 0;
            }
            else if (preS.GetEdgeID() == s.GetEdgeID()){
                hops += 2;
            }
            else if (preS.GetPod() == s.GetPod()){
                hops += 4;
            }
            else{
                hops += 6;
            }
        }
    }

    //bool chkcpuMemory = true, chkhops = true;
    int index = 0;

    for (auto &m : serviceMap){

        Server s = fattree->server.at(m.first);
        if (CheckBandwidth(argUser->bandwidth, s.GetBandwidth())){//check bandwidth
            double sumCPU = 0.0;
            double sumMemory = 0.0;

            for (int i = 0; i < m.second; i++){
                sumCPU += argUser->service_chain.at(index + i)->GetCPU();
                sumMemory += argUser->service_chain.at(index + i)->GetMemory();
            }

            if (CheckCPU(sumCPU, s.GetCPU()) && CheckMemory(sumMemory, s.GetMemory())){ ; }//check cpu and memory
            else{
                return false;
            }

            index += m.second;
        }
        else{
            return false;
        }
    }//end for serviceMap

    /*if (argUser->service_length == numOfCheckService){
        if (argUser->hops[1] >= hops){
        return true;
        }
        else{
        return false;
        }
        }*/

    if (argUser->hops[1] >= hops){
        return true;
    }
    else{
        return false;
    }
}

//int Simulation::CountNumToServe(vector<User>::iterator argUser, vector<Server>::iterator argServer){
//	if (argUser->bandwidth > argServer->GetBandwidth()){	//server bandwidth is not enough
//		cout << "server bandwidth is not enough: u " << argUser->bandwidth << "\ts " << argServer->GetBandwidth() << endl;
//		return 0;
//	}
//	else{
//		int count = 0;
//		double server_cpu = argServer->GetCPU();
//		double server_memory = argServer->GetMemory();
//
//		const int myint = -1;
//		vector<shared_ptr<Service>>::iterator it = find_if(argUser->service_chain.begin(), argUser->service_chain.end(), [myint](shared_ptr<Service> &s)->bool{return s->GetServerID() == myint; });
//		//vector<Service>::iterator it = find_if(argUser->service_chain.begin(), argUser->service_chain.end(), [myint](Service &s)->bool{return s.GetServerID() == myint; });
//		if (it == argUser->service_chain.end()){//services of user are all served
//			return -1;
//		}
//		
//		while (it != argUser->service_chain.end()){
//			cout << "server: " << server_cpu << "\t" << server_memory << endl;
//			cout << "service: " << it->GetCPU() << "\t" << it->GetMemory() << endl;
//
//			if (CheckCPU(it->GetCPU(), server_cpu) && CheckMemory(it->GetMemory(), server_memory)){
//				server_cpu = server_cpu - it->GetCPU();
//				server_memory = server_memory - it->GetMemory();
//				count++;
//				it++;
//			}
//			else{
//				break;
//			}
//		}
//		return count;
//	}
//}

bool Simulation::IsNotServiceServed(Service &s){
    return (s.GetServerID() == -1);
}

void Simulation::ResetService(shared_ptr<User> argUser){

    map<int, int> serviceMap;
    for (auto service : argUser->service_chain)
    {
        if (service->GetServerID() == -1)
        {
            break;
        }
        else
        {
            serviceMap.insert(pair<int, int>(service->GetID(), service->GetServerID()));
        }
    }

    for (auto &m : serviceMap){

        //cout << m.first << "\t" << m.second << endl;
        //Server server = fattree->server.at(m.second);
        int index = 0;
        for (auto service : fattree->server.at(m.second).serviceInServer)
        {
            if (m.first == service->GetID())
            {
                break;
            }
            index++;
        }
        fattree->server.at(m.second).serviceInServer.erase(fattree->server.at(m.second).serviceInServer.begin() + index);

        map<int, double>::iterator uit = fattree->server.at(m.second).userBandwidth.find(argUser->GetID());
        if (uit != fattree->server.at(m.second).userBandwidth.end())
        {
            fattree->server.at(m.second).userBandwidth.erase(uit);
        }
        fattree->server.at(m.second).UpdateServerBandwidth();
        fattree->server.at(m.second).UpdateServerCPUandMemory();
    }
}

//void Simulation::ResetService(shared_ptr<User> argUser){
//
//    map<int, int> serviceMap;
//    map<int, int>::iterator it;
//    for (auto s : argUser->service_chain){
//        if (s->GetServerID() == -1){
//            break;
//        }
//        else{
//            it = serviceMap.find(s->GetServerID());
//            if (it != serviceMap.end()){
//                it->second++;
//            }
//            else{
//                serviceMap.insert(pair<int, int>(s->GetServerID(), 1));
//            }
//
//            s->SetServerID(-1);
//        }
//    }
//
//    for (auto &m : serviceMap){
//
//        for (int i = 0; i < m.second; i++){
//            fattree->server.at(m.first).serviceInServer.pop_back();//the service which we want to remove must be the last element.
//        }
//        map<int, double>::iterator uit = fattree->server.at(m.first).userBandwidth.find(argUser->GetID());
//        fattree->server.at(m.first).userBandwidth.erase(uit);
//        fattree->server.at(m.first).UpdateServerBandwidth();
//        fattree->server.at(m.first).UpdateServerCPUandMemory();
//    }
//
//    //cout << "ResetService." << endl;
//}

bool Simulation::FindHopRecursive(int currService, int currServer, const shared_ptr<User> argUser, const int maxServer, vector<int> &argCurrVec){

    argCurrVec.at(currService) = currServer;
    //cout << currService << "\t" << currServer << endl;

    /*if (currService == argUser->service_length - 1){
        if (CheckAllConstraintIsOkay(argUser, argCurrVec, currService)){
        return true;
        }
        else{
        return false;
        }
        }
        else{
        if (CheckAllConstraintIsOkay(argUser, argCurrVec, currService)){
        ;
        }
        else{
        return false;
        }
        }*/

    if (CheckAllConstraintIsOkay(argUser, argCurrVec, currService + 1)){
        if (currService == argUser->service_length - 1){
            return true;
        }
    }
    else{
        return false;
    }

    for (int i = 0; i < maxServer; i++){
        bool answer = FindHopRecursive(currService + 1, i, argUser, maxServer, argCurrVec);
        if (answer){
            //cout << "get the correct answer!!!" << endl;
            return true;
        }
        else{
            continue;
        }
    }
    return false;

}

bool Simulation::MaximizeHopRecursive(int currService, int currServer, const shared_ptr<User> argUser, const int maxServer, vector<int> &argCurrVec, int &maxHop){

    argCurrVec.at(currService) = currServer;
    //cout << "Service: " << currService << "\tServer: " << currServer << endl;

    /*if (currService == argUser->service_length - 1){
        if (CheckAllConstraintIsOkay(argUser, argCurrVec, currService + 1)){
        int hops = CountHops(argUser, argCurrVec);
        if ((argUser->hops[1] - hops) < 4){
        maxHop = hops;
        return true;
        }
        else{
        return false;
        }
        }
        else{
        return false;
        }
        }*/

    if (CheckAllConstraintIsOkay(argUser, argCurrVec, currService + 1)){
        if (currService == argUser->service_length - 1){
            int hops = CountHops(argCurrVec);
            //cout << "hops: " << hops << endl;
            if ((argUser->hops[1] - hops) < 4){
                return true;
            }
            else{
                return false;
            }
        }
    }
    else{
        return false;
    }

    for (int i = currServer; i < maxServer; i++){
        bool answer = MaximizeHopRecursive(currService + 1, i, argUser, maxServer, argCurrVec, maxHop);
        if (answer){
            return true;
        }
        else{
            continue;
        }
    }
    return false;
}

void Simulation::PutServiceIntoServer(shared_ptr<User> argUser, vector<int> argServiceVec){

    //output << argUser->GetID() << ", ";
    for (unsigned int i = 0; i < argServiceVec.size(); i++){
        vector<Server>::iterator it = fattree->server.begin() + argServiceVec.at(i);

        it->SetActive(1);
        it->userBandwidth.insert(pair<int, double>(argUser->GetID(), argUser->bandwidth));

        //put service into server
        argUser->service_chain.at(i)->SetServerID(it->GetID());
        it->serviceInServer.push_back(argUser->service_chain.at(i));
		it->services.push_back(&(*(argUser->service_chain.at(i))));

        //update info(CPU,Memory,Bandwidth) of server
        it->UpdateServerBandwidth();
        it->UpdateServerCPUandMemory();

        //cout << "user id: " << argUser->GetID() << "\tservice id: " << argUser->service_chain.at(i)->GetID() << "\tserver id: " << it->GetID() << endl;
        //cout << it->GetCPU() << "\t" << it->GetMemory() << "\t" << it->GetBandwidth() << endl;
    }
}

int Simulation::CountHops(vector<int> argCurrVec){

    int hops = 0;
    //map<int, int> serviceMap;
    //map<int, int>::iterator it;
    for (unsigned i = 0; i < argCurrVec.size(); i++){
        /*it = serviceMap.find(currVec.at(i));
        if (it != serviceMap.end()){
        it->second++;
        }
        else{
        serviceMap.insert(pair<int, int>(currVec.at(i), 1));
        }*/
        //count hops
        if (i != 0){
            Server preS = fattree->server.at(argCurrVec.at(i - 1));
            Server s = fattree->server.at(argCurrVec.at(i));
            if (preS.GetID() == s.GetID()){
                hops += 0;
            }
            else if (preS.GetEdgeID() == s.GetEdgeID()){
                hops += 2;
            }
            else if (preS.GetPod() == s.GetPod()){
                hops += 4;
            }
            else{
                hops += 6;
            }
        }
    }
    return hops;
}



//int Simulation::DecideWindowSize(const shared_ptr<User> argUser){
//
//    unsigned int power_Pod, power_Edge, max_window_size;
//    int NumOfPodServer = (fattree->GetK())*(fattree->GetK()) / 4;
//    int NumOfEdgeServer = fattree->GetK() / 2;
//
//    if (argUser->service_chain.size() == 1){
//        max_window_size = 1;
//    }
//    else{
//        power_Pod = argUser->hops[1] / 6;
//        power_Edge = int(ceil(double(argUser->hops[1] - 6 * power_Pod) / 4.0));
//
//        //power = int(ceil(double(argUser->hops[1]) / 6.0));
//
//        max_window_size = NumOfPodServer * power_Pod + NumOfEdgeServer*power_Edge;
//
//        /*if (max_window_size % 2 == 0){
//            max_window_size -= 1;
//        }
//        else{ ; }*/
//        if (max_window_size > fattree->server.size() - 1){
//            max_window_size = fattree->server.size() - 1;
//        }
//        else{ ; }
//    }
//    return max_window_size;
//}

int Simulation::DecideWindowSize(const shared_ptr<User> argUser){

    //double power;
	unsigned int power;
	unsigned int max_window_size;
    int NumOfPodServer = (fattree->GetK())*(fattree->GetK()) / 4;

    //if ((argUser->service_chain.size() == 1) || (argUser->hops[1] == 0))
    if (argUser->service_chain.size() == 1)
    {
        max_window_size = 1;
    }
    else
    {
		//power = double(argUser->hops[1]) / 6.0;
        power = int(ceil(double(argUser->hops[1]) / 6.0));
        if (power == 0){
            power = 1;
        }
        max_window_size = NumOfPodServer * power;
        //max_window_size = int(ceil(NumOfPodServer * power));

        /*if (max_window_size % 2 == 0){
            max_window_size -= 1;
        }
        else{ ; }*/
        if (max_window_size > fattree->server.size()){
            max_window_size = fattree->server.size();
        }
		//else if (max_window_size < 1){
		//	max_window_size = 1;
		//}
        else{ ; }
    }
    return max_window_size;
}

bool Simulation::FindWindowToServe_SlidingFirstFit(shared_ptr<User> argUser, vector<int> argReOderVec, vector<int> &argAnswerVec, unsigned int argS, int argWindowSize){

    //double *serverBandwidth, *serverCPU, *serverMemory;
    //unsigned int windowSize = endServer - startServer + 1;
    //
    //vector<double> serverBandwidth(windowSize);
    //vector<double> serverCPU(windowSize);
    //vector<double> serverMemory(windowSize);
    //for (unsigned int i = 0; i < windowSize; i++){
    //	serverBandwidth.at(i) = fattree->server.at(startServer + i).GetBandwidth();
    //	serverCPU.at(i) = fattree->server.at(startServer + i).GetCPU();
    //	serverMemory.at(i) = fattree->server.at(startServer + i).GetMemory();
    //}
    //for (unsigned int i = 0; i < argUser->service_chain.size(); i++){
    //	argCurrVec.at(i) = -1;
    //	shared_ptr<Service> service_it = argUser->service_chain.at(i);
    //	unsigned int j;
    //	for (j = 0; j < windowSize; j++){
    //		if ((argUser->bandwidth <= serverBandwidth[j]) && (service_it->GetCPU() <= serverCPU[j]) && (service_it->GetMemory() <= serverMemory[j])){
    //			//serverBandwidth[j] -= argUser->bandwidth;
    //			serverCPU[j] -= service_it->GetCPU();
    //			serverMemory[j] -= service_it->GetMemory();
    //			argCurrVec.at(i) = startServer + j;
    //			break;
    //		}
    //	}//endfor server
    //	if (j == windowSize){//this service cannot put into any server
    //		//cout << "This service cannot put into any server!!!" << endl;
    //		break;
    //	}
    //}//endfor services

    vector<double> serverBandwidth(argWindowSize);
    vector<double> serverCPU(argWindowSize);
    vector<double> serverMemory(argWindowSize);

    for (int s = 0; s < argWindowSize; s++)
    {
        serverBandwidth.at(s) = fattree->server.at(argS + s).GetBandwidth();
        serverCPU.at(s) = fattree->server.at(argS + s).GetCPU();
        serverMemory.at(s) = fattree->server.at(argS + s).GetMemory();
    }

    for (unsigned i = 0; i < argAnswerVec.size(); i++)
    {
        unsigned int index = argReOderVec.at(i);
        argAnswerVec.at(index) = -1;
        shared_ptr<Service> service_it = argUser->service_chain.at(i);

        for ( int s = 0; s < argWindowSize; s++)
        {
            if ((argUser->bandwidth <= serverBandwidth.at(s)) && (service_it->GetCPU() <= serverCPU.at(s)) && (service_it->GetMemory() <= serverMemory.at(s))){
                serverCPU[s] -= service_it->GetCPU();
                serverMemory[s] -= service_it->GetMemory();
                argAnswerVec.at(index) = argS + s;
                break;
            }

        }

        if (argAnswerVec.at(index) == -1){
            return false;
        }
    }

    /*for (unsigned int i = 0; i < argAnswerVec.size(); i++){
        if (argAnswerVec.at(i) == -1){
            return false;
        }
    }*/

    return true;
}

bool Simulation::FindAnswer_FirstFit(shared_ptr<User> argUser, vector<int> argReOderVec, vector<int> &argAnswerVec, int argWindowSize){

    vector<Server> tmpServer = fattree->server;

    for (unsigned int i = 0; i < argReOderVec.size(); i++)
    {
        unsigned int index = argReOderVec.at(i);
        argAnswerVec.at(index) = -1;
        shared_ptr<Service> service_it = argUser->service_chain.at(index);
        vector<Server>::iterator server_it = tmpServer.begin();

        while (server_it != tmpServer.end())
        {
            if (CheckBandwidth(argUser->bandwidth, server_it->GetBandwidth()) && CheckService(service_it, server_it)){//server has enough bandwidth, cpu, memory
                argAnswerVec.at(index) = server_it->GetID();
                server_it->SetCPU(server_it->GetCPU() - service_it->GetCPU());
                server_it->SetMemory(server_it->GetMemory() - service_it->GetMemory());
                break;
            }//end if checkbandwidth and checkservice
            else{
                server_it++;
            }
        }
        if (server_it == tmpServer.end()){//this service cannot put into any server
            return false;
        }

    }
    return true;
}

bool Simulation::FindWindowToServe_SlidingBesFit(shared_ptr<User> argUser, 
                                                    vector<int> argReOderVec, 
                                                    vector<int> &argAnswerVec, 
                                                    unsigned int argS, 
                                                    int argWindowSize){
    vector<double> serverBandwidth(argWindowSize);
    vector<double> serverCPU(argWindowSize);
    vector<double> serverMemory(argWindowSize);

    #pragma omp parallel for
    for ( int s = 0; s < argWindowSize; s++)
    {
        serverBandwidth.at(s) = fattree->server.at(argS + s).GetBandwidth();
        serverCPU.at(s) = fattree->server.at(argS + s).GetCPU();
        serverMemory.at(s) = fattree->server.at(argS + s).GetMemory();
    }

    for (unsigned int i = 0; i < argReOderVec.size(); i++)
    {
        unsigned int index = argReOderVec.at(i);
        //unsigned int index = i;
        argAnswerVec.at(index) = -1;
        shared_ptr<Service> service_it = argUser->service_chain.at(index);
        vector<double> storage(argWindowSize, -1.0);
        vector<pair<int, double>> maxStorage;
        maxStorage.push_back(make_pair(-1, -1.0));
        //maxStorage.push_back(make_pair(-1, 1000.0));
        //unsigned int serverToServe;

        for ( int s = 0; s < argWindowSize; s++)
        {
            if ((argUser->bandwidth <= serverBandwidth.at(s)) && (service_it->GetCPU() <= serverCPU.at(s)) && (service_it->GetMemory() <= serverMemory.at(s))){
                //storage.at(s) = (serverCPU[s] - service_it->GetCPU()) + (serverMemory[s] - service_it->GetMemory());
                storage.at(s) = serverCPU.at(s) + serverMemory.at(s) - service_it->GetSumCPUMemory();

                if (storage.at(s) > maxStorage.at(0).second){
                //if (storage.at(s) < maxStorage.at(0).second){
                    maxStorage.at(0).first = s;
                    maxStorage.at(0).second = storage.at(s);
                }
            }
            else{
                storage.at(s) = -1.0;
            }
        }

        //argCurrVec.at(i) = argS + maxStorage.at(0).first;
        if (maxStorage.at(0).first == -1){
            return false;
        }
        else{
            //cout << maxStorage.at(0).first << endl;
            argAnswerVec.at(index) = argS + maxStorage.at(0).first;
            serverCPU.at(maxStorage.at(0).first) -= service_it->GetCPU();
            serverMemory.at(maxStorage.at(0).first) -= service_it->GetMemory();
        }
        //cout << service_it->GetID() << "\t" << argAnswerVec.at(i) << "\t" << serverCPU.at(maxStorage.at(0).first) << "\t" << serverMemory.at(maxStorage.at(0).first) << endl;
    }

    /*for (unsigned int i = 0; i < argCurrVec.size(); i++){
        if (argAnswerVec.at(i) == -1){
        return false;
        }
        }*/
    return true;
}

bool Simulation::FindWindowToServe_SLA(shared_ptr<User> argUser,
                                                    vector<int> argReOderVec,
                                                    vector<int> &argAnswerVec,
                                                    unsigned int argS,
                                                    int argWindowSize){

    vector<double> serverBandwidth(argWindowSize);
    vector<double> serverCPU(argWindowSize);
    vector<double> serverMemory(argWindowSize);

    #pragma omp parallel for
    for (int s = 0; s < argWindowSize; s++)
    {
        serverBandwidth.at(s) = fattree->server.at(argS + s).GetBandwidth();
        serverCPU.at(s) = fattree->server.at(argS + s).GetCPU();
        serverMemory.at(s) = fattree->server.at(argS + s).GetMemory();
    }

    for (unsigned int i = 0; i < argReOderVec.size(); i++)
    {
        unsigned int index = argReOderVec.at(i);
        argAnswerVec.at(index) = -1;
        shared_ptr<Service> service_it = argUser->service_chain.at(index);
        vector<double> storage(argWindowSize, -1.0);
        //vector<double> storage_CPU(argWindowSize, -1.0);
        //vector<double> storage_Memory(argWindowSize, -1.0);
        vector<double> diff(argWindowSize, -1.0);
        vector<pair<int, double>> maxStorage;
        maxStorage.push_back(make_pair(-1, 1000.0));
        //maxStorage.push_back(make_pair(-1, 0.0));


        for ( int s = 0; s < argWindowSize; s++)
        {
            if ((argUser->bandwidth <= serverBandwidth.at(s)) && (service_it->GetCPU() <= serverCPU.at(s)) && (service_it->GetMemory() <= serverMemory.at(s))){
                //storage_CPU.at(s)    = serverCPU[s] - service_it->GetCPU();
                //storage_Memory.at(s) = serverMemory[s] - service_it->GetMemory();
                diff.at(s) = abs((serverCPU[s] - service_it->GetCPU()) - (serverMemory[s] - service_it->GetMemory()));
				//storage.at(s) = ( serverCPU[s]-service_it->GetCPU()) + (serverMemory[s]-service_it->GetMemory());

                if (diff.at(s) < maxStorage.at(0).second){
                //if (storage.at(s) > maxStorage.at(0).second){
                    maxStorage.at(0).first = s;
                    maxStorage.at(0).second = diff.at(s);
                }
            }
            else{
                diff.at(s) = -1.0;
				//storage.at(s) = -1.0;
            }
        }

        //argCurrVec.at(i) = argS + maxStorage.at(0).first;
        if (maxStorage.at(0).first == -1){
            return false;
        }
        else{
            //cout << maxStorage.at(0).first << endl;
            argAnswerVec.at(index) = argS + maxStorage.at(0).first;
            serverCPU.at(maxStorage.at(0).first) -= service_it->GetCPU();
            serverMemory.at(maxStorage.at(0).first) -= service_it->GetMemory();
        }
        //cout << service_it->GetID() << "\t" << argAnswerVec.at(i) << "\t" << serverCPU.at(maxStorage.at(0).first) << "\t" << serverMemory.at(maxStorage.at(0).first) << endl;
    }

    /*for (unsigned int i = 0; i < argCurrVec.size(); i++){
    if (argAnswerVec.at(i) == -1){
    return false;
    }
    }*/
    return true;
}

void Simulation::FreeEndTimeUser(int argTime){
    //free the endtime user
    for (unsigned int i = 0; i < us->servingUser.size(); i++){
        shared_ptr<User> user_it = us->servingUser.at(i);
        if (user_it->endTime == argTime){
            //output << "Erase User: " << user_it->GetID() << endl;
            ResetService(user_it);
            us->servingUser.erase(us->servingUser.begin() + i);
            i--;
        }
        else{ ; }
    }
}

void Simulation::CountHopsViolation(int argMethod){
    switch (argMethod)
    {
    case 1:
        CountHopsViolation_Method1();
        break;
    case 2:
        CountHopsViolation_Method2();
        break;
    case 3:
        CountHopsViolation_Method3();
        break;
    default:
        break;
    }
}

void Simulation::CountHopsViolation_Method1(){
    for (unsigned int i = 0; i < us->waitingUser.size(); i++){
        shared_ptr<User> user_it = us->waitingUser.at(i);
        AddHops(user_it->hops[1]);
        AddViolateUser(user_it->hops[1]);
    }
    for (unsigned int i = 0; i < us->servingUser.size(); i++){
        shared_ptr<User> user_it = us->servingUser.at(i);
        if (user_it->CheckHops()){ ; }
        else{
            AddHops(user_it->hops[2] - user_it->hops[1]);
            AddViolateHops(user_it->hops[2] - user_it->hops[1]);
            //cout << user_it->hops[0] << "\t" << user_it->hops[1] << "\t" << user_it->hops[2] << endl;
        }
    }
}

void Simulation::CountHopsViolation_Method2(){
    for (unsigned int i = 0; i < us->waitingUser.size(); i++){
        shared_ptr<User> user_it = us->waitingUser.at(i);
        if (user_it->service_length == 1){
            AddHops(user_it->hops[1]);
            AddViolateUser(user_it->hops[1]);
        }
        else{
            AddHops(user_it->hops[0] - user_it->hops[1]);
            AddViolateUser(user_it->hops[0] - user_it->hops[1]);
            //AddHops(user_it->hops[0]);
            //AddViolateUser(user_it->hops[0]);
        }
    }
    for (unsigned int i = 0; i < us->servingUser.size(); i++){
        shared_ptr<User> user_it = us->servingUser.at(i);
        if (user_it->CheckHops()){ ; }
        else{
            AddHops(user_it->hops[2] - user_it->hops[1]);
            AddViolateHops(user_it->hops[2] - user_it->hops[1]);
        }
    }
}

void Simulation::CountHopsViolation_Method3(){
    for (unsigned int i = 0; i < us->waitingUser.size(); i++)
    {
        shared_ptr<User> user_it = us->waitingUser.at(i);
        if (user_it->service_length == 1){
            AddHops(6);
            AddViolateUser(6);
        }
        else{
            AddHops(user_it->hops[0]);
            AddViolateUser(user_it->hops[0]);
        }
    }
    for (unsigned int i = 0; i < us->servingUser.size(); i++)
    {
        shared_ptr<User> user_it = us->servingUser.at(i);
        if (user_it->CheckHops()){ ; }
        else{
            AddHops(user_it->hops[2] - user_it->hops[1]);
            AddViolateHops(user_it->hops[2] - user_it->hops[1]);
            /*if (user_it->GetID()<63 && user_it->GetID()>54){
                output << "user: " << user_it->GetID() << "(" << (user_it->hops[2] - user_it->hops[1]) << "," << user_it->hops[2] << "-" << user_it->hops[1] << "," << user_it->hops[0] << ") ";
                }*/
        }
        /*if (user_it->GetID()<63&&user_it->GetID()>55){
            output << "user: " << user_it->GetID() << "\t" << (user_it->hops[2] - user_it->hops[1]) <<"\t";
            }*/
    }
}

void Simulation::OutputRunTime(double argRunTime){
    output << "Run_time: " << argRunTime << " ms." << endl;
}

void Simulation::Compute_Server_Load(int argTime){
    
    double sumOfLoad = 0.0;
    //double avgOfLoad;
    int fattreeSize = fattree->server.size();
    for (int i = 0; i < fattreeSize; i++)
    {
        Server server_it = fattree->server.at(i);
        sumOfLoad += (200.0 - server_it.GetCPU() - server_it.GetMemory());
    }
    server_Load += sumOfLoad / (2 * fattreeSize);
}
