#include <iostream>
#include <ctime>
#include "NeuralNetwork.hpp"
#include "MNIST_Reader.hpp"

int main()
{

	std::vector<Vec> images = MNIST::getImages();
	std::vector<Vec> labels = MNIST::getLabels();
	std::vector<Vec> img;
	std::vector<Vec> l;

	NeuralNetwork::split(images, labels, img, l, 0.60);

	int inputs = 28 * 28;
	int outputs = 10;
	double lr = 0.01;
	NeuralNetwork net(inputs, outputs, lr);
	net.add(150);
	net.add(30);

	unsigned epochs = 20;
	net.train(images, labels, img, l, epochs);

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
