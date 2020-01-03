#include <cassert>
#include <ctime>
#include <cmath>
#include <iostream>
#include "NeuralNetwork.hpp"
#include "Graphics.hpp"

FuncLastD NeuralNetwork::calculateLastDelta[] = { NeuralNetwork::calculateLastDelta_MSE, NeuralNetwork::calculateLastDelta_CE };
FuncLoss NeuralNetwork::calculateLoss[] = { NeuralNetwork::calculateLoss_MSE, NeuralNetwork::calculateLoss_CE };

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

void NeuralNetwork::backProp(const Vec* inputs, const Vec* outputs, unsigned batchSize,
                             LossFunction loss)
{
	assert(outputs[0].size() == outputSize);

	for (unsigned b = 0; b < batchSize; ++b) {
		forwardProp(inputs[b]);

		Vec delta = calculateLastDelta[loss](outputs[b], L.back());
		Vec next;
		L.back().backProp(next, (L.size() == 1 ? inputs[b] : L[L.size() - 2].x), delta);

		for (int i = L.size() - 2; i >= 0; --i)
			L[i].backProp(next, (i == 0 ? inputs[b] : L[i - 1].x));

	}

	for (unsigned i = 0; i < L.size(); ++i)
		L[i].addGrads(learningRate);
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
                          const std::vector<Vec>& inputsTest,  const std::vector<Vec>& outputsTest,
                          unsigned epochs, unsigned batchSize, LossFunction loss)
{
	unsigned in = (L.size() == 0 ? inputSize : L.back().outputs());
	L.push_back(Layer(in, outputSize, (loss == CROSS_ENTROPY ? Layer::SOFTMAX : Layer::SIGMOID)));

	std::vector<std::vector<real>> errors_train;
	std::vector<std::vector<real>> errors_test;

	Graphics G;
	G.setTitle("LOSS vs EPOCHS")
	.setYLabel("Loss")
	.setXLabel("Epochs")
	.setLMargin(10);

	std::cout << "Train on " << inputsTrain.size() << " samples, validate on " << inputsTest.size() << " samples." << std::endl;

	for (unsigned i = 1; i <= epochs; ++i) {
		std::cout << "EPOCH: " << i << "/" << epochs << std::endl;

		unsigned j, tot = inputsTrain.size() - inputsTrain.size() % batchSize;

		for (j = 0; j < tot; j += batchSize)
			backProp(inputsTrain.data() + j, outputsTrain.data() + j, batchSize, loss);

		if (j < inputsTrain.size())
			backProp(inputsTrain.data() + j, outputsTrain.data() + j, inputsTrain.size() - j, loss);

		real current_loss_train = calculateTotalLoss(inputsTrain, outputsTrain, loss);
		real current_loss_test = calculateTotalLoss(inputsTest, outputsTest, loss);

		std::cout << "Loss: " << current_loss_train << ", Val. Loss: " << current_loss_test << std::endl;

		errors_train.push_back({(real)i, current_loss_train});
		errors_test.push_back({(real)i, current_loss_test});

		G.restart(false)
		.setXRange(0., i + .5)
		.add(errors_train, "Train", false, "blue", 2, true)
		.add(errors_test,  "Test",  false, "red",  2, true)
		.plot();
	}

	real current_loss_train = calculateTotalLoss(inputsTrain, outputsTrain, loss);
	real current_loss_test  = calculateTotalLoss(inputsTest, outputsTest, loss);
	errors_train.push_back({(real)epochs, current_loss_train});
	errors_test.push_back({(real)epochs, current_loss_test});
	G.restart(false)
	.setXRange(0., epochs + .5)
	.add(errors_train, "Train", false, "blue", 2, true)
	.add(errors_test,  "Test",  false, "red",  2, true)
	.plot();
}

real NeuralNetwork::calculateTotalLoss(const std::vector<Vec>& inputs,
                                       const std::vector<Vec>& outputs, LossFunction loss)
{
	real error = 0.;

	for (unsigned i = 0; i < outputs.size(); ++i) {
		forwardProp(inputs[i]);
		error += calculateLoss[loss](L.back().x, outputs[i]);
	}

	error /= outputs.size();

	return error;
}

real NeuralNetwork::calculateLoss_MSE(const Vec& x, const Vec& output)
{
	real error = 0.;

	for (unsigned i = 0; i < x.size(); ++i) {
		real mult = x[i] - output[i];
		error += mult * mult;
	}

	return 0.5 * error;
}

real NeuralNetwork::calculateLoss_CE(const Vec& x, const Vec& output)
{
	real error = 0.;

	for (unsigned i = 0; i < x.size(); ++i)
		error += output[i] * std::log(x[i]);

	return - error;
}

Vec NeuralNetwork::calculateLastDelta_MSE(const Vec& output, const Layer& lastL)
{
	return lastL.getCalculateDelta()(lastL.x - output, lastL.x, lastL.z);
}

Vec NeuralNetwork::calculateLastDelta_CE(const Vec& output, const Layer& lastL)
{
	Vec send(output.size());

	for (unsigned i = 0; i < output.size(); ++i)
		send[i] = - output[i] / lastL.x[i];

	return lastL.getCalculateDelta()(send, lastL.x, lastL.z);
}
