//RandomHelper.h

#ifndef RANDOMHELPER_H
#define RANDOMHELPER_H

#include <iostream>
#include <random>

int UniformIntRandom(int argMinValue, int argMaxValue);
double NormalRandom(double argMean, double argStddev, double argMinValue, double argMaxValue);

int UniformIntRandom(int argMinValue, int argMaxValue){
	//every random will be different
	/*random_device rd;
	default_random_engine generator(rd());*/
	
	std::default_random_engine generator(rand());
	std::uniform_int_distribution<int> distribution(argMinValue, argMaxValue);

	int number = distribution(generator);
	return number;
}

double NormalRandom(double argMean, double argStddev, double argMinValue, double argMaxValue){
	//every random will be differnt
	/*random_device rd;
	default_random_engine generator(rd());*/

	std::default_random_engine generator(rand());
	std::normal_distribution<double> distribution(argMean, argStddev);

	/*double number = distribution(generator);
	if (number >= argMinValue && number <= argMaxValue){ return number; }
	else if (number < argMinValue){ return argMinValue; }
	else { return argMaxValue; }*/

	double number;
	do{
		number = distribution(generator);
	} while (number<argMinValue || number>argMaxValue);
	return number;
}

#endif