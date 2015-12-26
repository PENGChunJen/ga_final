#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <random>
#include <sstream>
#include "fitnessFunction.h"
#define NDEBUG
using namespace std;

int FitnessFunction::calculate_fitness(int* chromosome)
{
	int max_bike_number = 88;
	int initial_bike_number = 88;
	int punish = -2000;
	double fitness = 0;
	
	double* d_instance = new double[1440];
	int* instance = new int[1440];
	int* instance_sum = new int[1440];

	for(int i = 0; i < model.size(); i++){
		random_device generator;
		normal_distribution<double> distribution(model[i].mean, model[i].var);
		//d_instance[i] = distribution(generator);
		d_instance[i] = -1*distribution(generator);
		instance[i] = (d_instance[i]>-0.5)?(int)(d_instance[i]+0.5):(int)(d_instance[i]-0.5);
        #ifdef DEBUG
		cout << "d_instance[" << i << "] = " << d_instance[i] << endl;
		cout << "  instance[" << i << "] = " << instance[i] << endl;
	    #endif
	}
	stringstream ss;
    ss << "../output/instance" << id << ".csv";
    output(instance, ss.str());
	
	
	instance_sum[0] = initial_bike_number+chromosome[0];
	fitness += (chromosome[0]==0) ? 0 : punish;
	if(chromosome[0] != 0)
		cout << "load " << setw(4) <<  chromosome[0] << " bikes and punish " << punish << " at    0 minute!" << endl;
	for(int i = 1; i< model.size(); i++){
		instance_sum[i] = instance_sum[i-1]+instance[i];
	    if( i%30 == 0){
	        instance_sum[i] += chromosome[(int)(i/30)];
	        fitness += (chromosome[(int)(i/30)]==0) ? 0 : punish;
            if(chromosome[(int)(i/30)] != 0)
				cout << "load " << setw(4) << chromosome[(int)(i/30)] << " bikes and punish " << punish << " at " << setw(4) << i << " minute!" << endl;
		}
		if( instance_sum[i] < 0 )
			fitness += instance_sum[i];
	    else if( instance_sum[i] > max_bike_number )
			fitness -= (instance_sum[i]-max_bike_number);
	}
	stringstream ss1;
    ss1 << "../output/total" << id << ".csv";
    output(instance_sum, ss1.str());


    delete [] d_instance;
	delete [] instance;
	delete [] instance_sum;
	return fitness;
}

void FitnessFunction::output(int* instance, string file_name)
{
	ofstream fout(file_name);
	if(fout.fail())
		cerr << "Cannot open file : " << file_name << endl;
	else
		for(int i = 0; i < model.size(); i++){
			fout << (i+1) << " ," << instance[i] << endl;
            #ifdef DEBUG
			cout << file_name << "[" << i << "] = " << instance[i] << endl;
            #endif
	    }

	fout.close();
}

void FitnessFunction::load_data(int station_id)
{
	id = station_id;
	model.clear();
	stringstream ss;
	ss << station_id;
    string str = "../include/model"+ss.str()+".txt";
    ifstream infile;
	infile.open(str);
	if(!infile)
		cerr << "Can not open file : " << str << endl;
	int minute;
	double mean, var;
	while(infile >> minute >> mean >> var){
		Norm n = {.minute = minute, .mean = mean, .var = var};
	    model.push_back(n);
	}
	#ifdef DEBUG
	for(int i = 0; i < model.size(); i++){
		cout << model[i].minute << " " 
		     << model[i].mean << " " 
			 << model[i].var << endl;
	}
	#endif
}
