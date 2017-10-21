#include <iostream>
#include <sstream>
#include <fstream>
#include <math.h>
#include <vector>
#include <numeric>

#include "classifier.h"

// Good description : https://www.youtube.com/watch?v=r1in0YNetG8


/**
 * Initializes GNB
 */
GNB::GNB() {

}

GNB::~GNB() {}

double GNB::gaussian_prob(double obs, double mu, double sig)
{

	double num = pow((obs - mu),2);
	double denum = 2 * pow(sig,2);
	double norm = 1 / sqrt(2*M_PI*pow(sig,2));
	return (norm * pow(M_E, (-1 * num/denum)));

}


void GNB::train(vector<vector<double>> data, vector<string> labels)
{

	/*
		Trains the classifier with N data points and labels.

		INPUTS
		data - array of N observations
		  - Each observation is a tuple with 4 values: s, d,
		    s_dot and d_dot.
		  - Example : [
			  	[3.5, 0.1, 5.9, -0.02],
			  	[8.0, -0.3, 3.0, 2.2],
			  	...
		  	]

		labels - array of N labels
		  - Each label is one of "left", "keep", or "right".
	*/

	// For all data in the trainig set - seperate to a per label list.
	for (int i=0; i < data.size() ; ++i) {

		if ( labels[i] == possible_labels[0])
			left_list.push_back(data[i]);


		if ( labels[i] == possible_labels[1])
			keep_list.push_back(data[i]);


		if ( labels[i] == possible_labels[2])
			right_list.push_back(data[i]);

	}

	//cout << "Sorted Training data set by label" << endl;
	//cout << "#Left:" << left_list.size() << endl;
	//cout << "#Keep:" << keep_list.size() << endl;
	//cout << "#Right:" << right_list.size() << endl;

// Left data.

// to do : make gneric for number of features and number of labels.
	for( int feat_vec_elem=0; feat_vec_elem < data[0].size(); ++feat_vec_elem) {
		// For each feature fector in this label - get gaussian mean and std deviation.
		double mean = 0.0;
		double sum=0.0;
		for ( int sample=0; sample < left_list.size(); ++sample) {
			sum += left_list[sample][feat_vec_elem];
		}
		mean = sum / left_list.size();
		//cout << "Left Mean :feature:" << feat_vec_elem << ":" << mean;
		left_mean.push_back(mean);

		double standardDeviation = 0.0;
		for ( int sample=0; sample < left_list.size(); ++sample) {
			standardDeviation += pow(left_list[sample][feat_vec_elem] - mean, 2);
		}
		standardDeviation = sqrt(standardDeviation / left_list.size());
		left_stds.push_back(standardDeviation);
		//cout << "Left standard :feature:" << feat_vec_elem << ":" << standardDeviation;
	}
	//cout << "Left : Means for Features:" ;
	//for ( double x : left_mean ) std::cout << x << ' ';
	//std::cout << std::endl;
	//cout << "Left : Stds for Features:" ;
	//for ( double x : left_stds ) std::cout << x << ' ';
	//std::cout << std::endl;




	for( int feat_vec_elem=0; feat_vec_elem < data[0].size(); ++feat_vec_elem) {
		// For each feature fector in this label - get gaussian mean and std deviation.
		double mean = 0.0;
		double sum=0.0;
		for ( int sample=0; sample < keep_list.size(); ++sample) {
			sum += keep_list[sample][feat_vec_elem];
		}
		mean = sum / keep_list.size();
		//cout << "Keep Mean :feature:" << feat_vec_elem << ":" << mean;
		keep_mean.push_back(mean);

		double standardDeviation = 0.0;
		for ( int sample=0; sample < keep_list.size(); ++sample) {
			standardDeviation += pow(keep_list[sample][feat_vec_elem] - mean, 2);
		}
		standardDeviation = sqrt(standardDeviation / keep_list.size());
		keep_stds.push_back(standardDeviation);
		//cout << "Keep standard :feature:" << feat_vec_elem << ":" << standardDeviation;
	}
	//cout << "Keep : Means for Features:" ;
	//for ( double x : keep_mean ) std::cout << x << ' ';
	//std::cout << std::endl;
	//cout << "Keep : Stds for Features:" ;
	//for ( double x : keep_stds ) std::cout << x << ' ';
	//std::cout << std::endl;

	for( int feat_vec_elem=0; feat_vec_elem < data[0].size(); ++feat_vec_elem) {
	  // For each feature fector in this label - get gaussian mean and std deviation.
	  double mean = 0.0;
	  double sum=0.0;
	  for ( int sample=0; sample < right_list.size(); ++sample) {
	    sum += right_list[sample][feat_vec_elem];
	  }
	  mean = sum / right_list.size();
	  //cout << "right Mean :feature:" << feat_vec_elem << ":" << mean;
	  right_mean.push_back(mean);

	  double standardDeviation = 0.0;
	  for ( int sample=0; sample < right_list.size(); ++sample) {
	    standardDeviation += pow(right_list[sample][feat_vec_elem] - mean, 2);
	  }
	  standardDeviation = sqrt(standardDeviation / right_list.size());
	  right_stds.push_back(standardDeviation);
	  //cout << "right standard :feature:" << feat_vec_elem << ":" << standardDeviation;
	}
	//cout << "right : Means for Features:" ;
	//for ( double x : right_mean ) std::cout << x << ' ';
	//std::cout << std::endl;
	//cout << "right : Stds for Features:" ;
	//for ( double x : right_stds ) std::cout << x << ' ';
	//std::cout << std::endl;



}

string GNB::predict(vector<double> sample)
{
	/*
		Once trained, this method is called and expected to return
		a predicted behavior for the given observation.

		INPUTS

		observation - a 4 tuple with s, d, s_dot, d_dot.
		  - Example: [3.5, 0.1, 8.5, -0.2]

		OUTPUT

		A label representing the best guess of the classifier. Can
		be one of "left", "keep" or "right".
		"""
		# TODO - complete this
	*/

	double maxlikelihood = 0.0;
	int label=-1;

	double prob_left = 1.0;
	for (int i=0;i < sample.size() ; ++i) {
		prob_left *= gaussian_prob(sample[i], left_mean[i], left_stds[i]);
	}
	if(prob_left > maxlikelihood){
		maxlikelihood = prob_left;
		label = 0; // left
	}
	//cout << "Predict Left Probability:" << prob_left << endl;


	double prob_keep = 1.0;
	for (int i=0;i < sample.size() ; ++i) {
		prob_keep *= gaussian_prob(sample[i], keep_mean[i], keep_stds[i]);
	}
	if(prob_keep > maxlikelihood){
		maxlikelihood = prob_keep;
		label = 1; // kee[]
	}
	//cout << "Predict Keep Probability:" << prob_keep << endl;


	double prob_right = 1.0;
	for (int i=0;i < sample.size() ; ++i) {
		prob_right *= gaussian_prob(sample[i], right_mean[i], right_stds[i]);
	}
	if(prob_right > maxlikelihood){
		maxlikelihood = prob_right;
		label = 2; // right
	}
	//cout << "Predict Right Probability:" << prob_right << endl;

  //cout << "Predicting:" << possible_labels[label] << endl;


	return this->possible_labels[label];

}
