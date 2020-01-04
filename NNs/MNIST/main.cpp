#include <iostream>
#include <ctime>
#include "NeuralNetwork.hpp"
#include "MNIST_Reader.hpp"
#include "Instrumentor.h"

int main()
{
	BEGIN_PROFILING("MNIST", "MNIST_PROFILING.json");

	std::vector<Vec> images = MNIST::getImages();
	std::vector<Vec> labels = MNIST::getLabels();
	std::vector<Vec> img;
	std::vector<Vec> l;

	NeuralNetwork::split(images, labels, img, l, 0.50);


	int inputs = 28 * 28;
	int outputs = 10;
	double learning_rate = 0.001;
	NeuralNetwork net(inputs, outputs, learning_rate);
	net.add(150);
	net.add(30);

	unsigned epochs = 50;
	unsigned batchSize = 44;
	// net.train(images, labels, img, l, epochs, batchSize);
	net.train(images, labels, img, l, epochs, batchSize, NeuralNetwork::CROSS_ENTROPY);
	// net.train(img, l, images, labels, epochs, batchSize, NeuralNetwork::CROSS_ENTROPY);

	END_PROFILING("MNIST");

	///////////////////////////////////////TRAINING SET///////////////////////////////////////////
	unsigned start = 0;
	unsigned finnish = images.size();

	unsigned right_training = 0;
	unsigned wrong_training = 0;

	for (unsigned i = start; i < finnish; ++i) {
		Vec::Index guess;
		net.forwardProp(images[i]).maxCoeff(&guess);
		MNIST::print(images[i]);
		std::cout << guess << std::endl;

		int correct;

		for (unsigned j = 0; j < labels[i].size(); ++j)
			if (labels[i](j) == 1) correct = j;

		if (guess == correct)
			++right_training;
		else
			++wrong_training;
	}

	//////////////////////////////////TEST SET////////////////////////////////////////////////
	start = 0;
	finnish = img.size();

	unsigned right_test = 0;
	unsigned wrong_test = 0;

	for (unsigned i = start; i < finnish; ++i) {
		Vec::Index guess;
		net.forwardProp(img[i]).maxCoeff(&guess);
		MNIST::print(img[i]);
		std::cout << guess << std::endl;

		int correct;

		for (unsigned j = 0; j < l[i].size(); ++j)
			if (l[i](j) == 1) correct = j;

		if (guess == correct)
			++right_test;
		else
			++wrong_test;
	}

	///////////////////////////////////////RESULTS///////////////////////////////////////////

	std::cout << "TRAINING SET" << std::endl;
	std::cout << "RIGHT: " << right_training << std::endl;
	std::cout << "WRONG: " << wrong_training << std::endl;
	std::cout << "PERC: " << (double)right_training / (right_training + wrong_training) * 100. << "%" <<
	          std::endl;

	std::cout << "TEST SET" << std::endl;
	std::cout << "RIGHT: " << right_test << std::endl;
	std::cout << "WRONG: " << wrong_test << std::endl;
	std::cout << "PERC: " << (double)right_test / (right_test + wrong_test) * 100. << "%" << std::endl;

	std::cout << "ALL" << std::endl;
	std::cout << "RIGHT: " << right_training + right_test << std::endl;
	std::cout << "WRONG: " << wrong_training + wrong_test << std::endl;
	std::cout << "PERC: " << (double)(right_training + right_test) / (right_training + right_test +
	          wrong_training + wrong_test) * 100. << "%" << std::endl;

	return 0;
}
