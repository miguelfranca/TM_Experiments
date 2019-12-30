#include <cassert>
#include <ctime>
#include <cmath>
#include <iostream>
#include "NeuralNetwork.hpp"

NeuralNetwork::NeuralNetwork(unsigned a_inputSize, unsigned a_outputSize, real a_learningRate)
	: inputSize(a_inputSize), outputSize(a_outputSize), learningRate(a_learningRate)
{
	std::srand(std::time(nullptr) + clock());
}

NeuralNetwork::~NeuralNetwork()
{

}

void NeuralNetwork::add(unsigned layerSize, Layer::Activation a)
{
	unsigned in = (L.size() == 0 ? inputSize : L.back().outputs());
	L.push_back(Layer(in, layerSize, a));
}

void NeuralNetwork::backProp(const Vec& input, const Vec& output)
{
	forwardProp(input);

	assert(output.size() == outputSize);

	Vec next = L.back().x - output;

	for (int i = L.size() - 1; i >= 0; --i) {
		Vec delta = L[i].z.unaryExpr(Layer::activations_1stD[L[i].func]);
		delta = next.cwiseProduct(delta);
		next = (delta.transpose() * L[i].W).transpose();
		L[i].W -= learningRate * (delta * (i == 0 ? input : L[i - 1].x).transpose());
		L[i].b -= learningRate * delta;
	}
}

Vec NeuralNetwork::forwardProp(const Vec& input)
{
	assert(input.size() == inputSize);

	Vec output = input;

	for (unsigned i = 0; i < L.size(); ++i)
		output = L[i].forwardProp(output);

	return output;
}

void NeuralNetwork::split(std::vector<Vec>& inputsTrain, std::vector<Vec>& outputsTrain,
                          std::vector<Vec>& inputsTest,  std::vector<Vec>& outputsTest, double perc)
{

	int newSize = std::ceil(inputsTrain.size() * perc);

	std::vector<Vec> second_half_inputs(inputsTrain.begin() + newSize,
	                                    inputsTrain.end());
	std::vector<Vec> second_half_outputs(outputsTrain.begin() + newSize,
	                                     outputsTrain.end());

	inputsTrain.resize(newSize);
	outputsTrain.resize(newSize);

	inputsTest = (std::vector<Vec>&&)second_half_inputs;
	outputsTest = (std::vector<Vec>&&)second_half_outputs;
}

void NeuralNetwork::train(const std::vector<Vec>& inputsTrain, const std::vector<Vec>& outputsTrain,
                          const std::vector<Vec>& inputTests,  const std::vector<Vec>& outputTests, unsigned epochs)
{
	unsigned in = (L.size() == 0 ? inputSize : L.back().outputs());
	L.push_back(Layer(in, outputSize));

	for (unsigned i = 0; i < epochs; ++i) {
		std::cout << "EPOCH: " << i << std::endl;

		for (unsigned j = 0; j < inputsTrain.size(); ++j) {
			backProp(inputsTrain[j], outputsTrain[j]);
		}
	}
}