#ifndef CLASSIFIER_H
#define CLASSIFIER_H
#include <iostream>
#include <sstream>
#include <fstream>
#include <math.h>
#include <vector>

using namespace std;

class GNB {
public:

	vector<string> possible_labels = {"left","keep","right"};
	vector< vector<double> > left_list; //[ [] [] [] [] , [] [] [] [] , .. [] [] [] [] ]
	vector< vector<double> > left_list_tranposed; //[ [] [] .... []] .. [ [] [] [] [] ]


	vector<double> left_mean;
	vector<double> left_stds;
	vector<double> left_probs;

	vector< vector<double> > keep_list;
	vector< vector<double> > keep_transposed;

	vector<double> keep_mean;
	vector<double> keep_stds;
	vector<double> keep_probs;


	vector< vector<double> > right_list;
	vector< vector<double> > right_list_transposed;

	vector<double> right_mean;
	vector<double> right_stds;
	vector<double> right_probs;





	double gaussian_prob(double obs, double mu, double sig);

	/**
  	* Constructor
  	*/
 	GNB();

	/**
 	* Destructor
 	*/
 	virtual ~GNB();

 	void train(vector<vector<double> > data, vector<string>  labels);

  	string predict(vector<double>);

};

#endif
