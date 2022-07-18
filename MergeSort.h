//MergeSort.h

#ifndef MERGESORT_H
#define MERGESORT_H


#include "Service.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <time.h>
//#include <memory>

//using namespace std;

void merge_largeTosmall(std::vector<std::shared_ptr<Service>> &argService_Chain, std::vector<int> &argCurrVec, int first, int mid, int last);
void merge_smallTolarge(std::vector<std::shared_ptr<Service>> &argService_Chain, std::vector<int> &argCurrVec, int first, int mid, int last);
void Merge_Sort_Recursive(std::vector<std::shared_ptr<Service>> &argService_Chain, std::vector<int> &argCurrVec, int first, int last);
void shift(std::vector<std::shared_ptr<Service>> &argService_Chain, std::vector<int> &argCurrVec, int first, int last);
void SmallToLarge_Sort_Recursive(std::vector<std::shared_ptr<Service>> &argService_Chain, std::vector<int> &argCurrVec, int first, int last);
void LargeToSmall_Sort_Recursive(std::vector<std::shared_ptr<Service>> &argService_Chain, std::vector<int> &argCurrVec, int first, int last);
void Random_Sort_Recursive(std::vector<std::shared_ptr<Service>> &argService_Chain, std::vector<int> &argCurrVec, int first, int last);

void Merge_Sort_Recursive(std::vector<std::shared_ptr<Service>> &argService_Chain, std::vector<int> &argCurrVec, int first, int last)
{
	/*for (unsigned int i = 0; i < argService_Chain.size(); i++)
	{
		std::cout << argService_Chain.at(i)->GetID() << "\t";
	}
	std::cout << std::endl;*/

	if (first < last)
	{
		int mid = (int)(((double)first + (double)last) / 2);
		Merge_Sort_Recursive(argService_Chain, argCurrVec, first, mid);
		Merge_Sort_Recursive(argService_Chain, argCurrVec, mid + 1, last);
		merge_largeTosmall(argService_Chain, argCurrVec, first, mid, last);
	}
}

void LargeToSmall_Sort_Recursive(std::vector<std::shared_ptr<Service>> &argService_Chain, std::vector<int> &argCurrVec, int first, int last)
{
	if (first < last)
	{
		int mid = (int)(((double)first + (double)last) / 2);
		LargeToSmall_Sort_Recursive(argService_Chain, argCurrVec, first, mid);
		LargeToSmall_Sort_Recursive(argService_Chain, argCurrVec, mid + 1, last);
		merge_largeTosmall(argService_Chain, argCurrVec, first, mid, last);
	}
}

void SmallToLarge_Sort_Recursive(std::vector<std::shared_ptr<Service>> &argService_Chain, std::vector<int> &argCurrVec, int first, int last)
{
	if (first < last)
	{
		int mid = (int)(((double)first + (double)last) / 2);
		SmallToLarge_Sort_Recursive(argService_Chain, argCurrVec, first, mid);
		SmallToLarge_Sort_Recursive(argService_Chain, argCurrVec, mid + 1, last);
		merge_smallTolarge(argService_Chain, argCurrVec, first, mid, last);
	}
}

void Random_Sort_Recursive(std::vector<std::shared_ptr<Service>> &argService_Chain, std::vector<int> &argCurrVec, int first, int last){

	std::vector<int> tmp(argCurrVec.size(), 0);

	srand(static_cast<unsigned int>(time(NULL)));

	//number = rand() % last;

	for (unsigned i = 0; i < argCurrVec.size(); i++)
	{
		
		bool answer = false;
		int number;
		do{
			number = rand() % (last + 1);
			//std::cout << number << std::endl;
			if (tmp.at(number) == 0){
				tmp.at(number) = 1;
				answer = true;
			}
		} while (!answer);

		argCurrVec.at(i) = number;
	}
}

void merge_largeTosmall(std::vector<std::shared_ptr<Service>> &argservice_chain, std::vector<int> &argcurrvec, int first, int mid, int last)
{
	int i = first;
	int j = mid + 1;
	while (j != last + 1)
	{
		if (argservice_chain.at(i)->GetSumCPUMemory() >= argservice_chain.at(j)->GetSumCPUMemory())
		{
			if (i == j)
				j++;
		}
		else
		{
			shift(argservice_chain, argcurrvec, i, j);
			j++;
		}
		i++;
	}
}

void merge_smallTolarge(std::vector<std::shared_ptr<Service>> &argservice_chain, std::vector<int> &argcurrvec, int first, int mid, int last)
{
	int i = first;
	int j = mid + 1;
	while (j != last + 1)
	{
		if (argservice_chain.at(i)->GetSumCPUMemory() <= argservice_chain.at(j)->GetSumCPUMemory())
		{
			if (i == j)
				j++;
		}
		else
		{
			shift(argservice_chain, argcurrvec, i, j);
			j++;
		}
		i++;
	}
}

void shift(std::vector<std::shared_ptr<Service>> &argService_Chain, std::vector<int> &argCurrVec, int first, int last)
{
	int tmpInt = argCurrVec.at(last);
	std::shared_ptr<Service> tmpService = argService_Chain.at(last);

	for (int i = last - 1; i >= first; i--)
	{
		argCurrVec.at(i + 1) = argCurrVec.at(i);
		argService_Chain.at(i + 1) = argService_Chain.at(i);
	}
	argCurrVec.at(first) = tmpInt;
	argService_Chain.at(first) = tmpService;
}

#endif
