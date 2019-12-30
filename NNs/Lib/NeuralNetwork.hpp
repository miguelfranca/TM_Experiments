#pragma once
#include "Layer.hpp"

class NeuralNetwork
{
public:
	NeuralNetwork(unsigned inputSize, unsigned outputSize, real learningRate = 0.1);

	~NeuralNetwork();

	void add(unsigned layerSize, Layer::Activation a = Layer::SIGMOID);

	void backProp(const Vec& input, const Vec& output);
	Vec forwardProp(const Vec& input);

	static void split(std::vector<Vec>& inputsTrain, std::vector<Vec>& outputsTrain,
	                  std::vector<Vec>& inputsTests, std::vector<Vec>& outputTests, double perc);

	void train(const std::vector<Vec>& inputsTrain, const std::vector<Vec>& outputsTrain, const std::vector<Vec>& inputTests,
	           const std::vector<Vec>& outputTests, unsigned epochs);

private:
	std::vector<Layer> L;

	unsigned inputSize;
	unsigned outputSize;

	real learningRate;
};