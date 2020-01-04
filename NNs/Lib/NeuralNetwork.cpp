#include <cassert>
#include <ctime>
#include <cmath>
#include <iostream>
#include "NeuralNetwork.hpp"
#include "Graphics.hpp"
#include "Instrumentor.h"

FuncLastD NeuralNetwork::calculateLastDelta[] = { NeuralNetwork::calculateLastDelta_MSE, NeuralNetwork::calculateLastDelta_CE };
FuncLoss NeuralNetwork::calculateLoss[] = { NeuralNetwork::calculateLoss_MSE, NeuralNetwork::calculateLoss_CE };

NeuralNetwork::NeuralNetwork(unsigned a_inputSize, unsigned a_outputSize, real a_learningRate)
	: inputSize(a_inputSize), outputSize(a_outputSize), learningRate(a_learningRate)
{
	PROFILE_FUNCTION();
	std::srand(std::time(nullptr) + clock());
}

NeuralNetwork::~NeuralNetwork()
{

}

void NeuralNetwork::add(unsigned layerSize, Layer::Activation a)
{
	PROFILE_FUNCTION();
	unsigned in = (L.size() == 0 ? inputSize : L.back().outputs());
	L.push_back(Layer(in, layerSize, a));
}

void NeuralNetwork::backProp(const Vec* inputs, const Vec* outputs, unsigned batchSize,
                             LossFunction loss)
{
	PROFILE_FUNCTION();
	assert(outputs[0].size() == outputSize);

	for (unsigned b = 0; b < batchSize; ++b) {
		forwardProp(inputs[b]);

		Vec delta = calculateLastDelta[loss](outputs[b], L.back());
		Vec next;
		L.back().backProp(next, (L.size() == 1 ? inputs
		                         [b] : L[L.size() - 2].x), delta);

		for (int i = L.size() - 2; i >= 0; --i)
			L[i].backProp(next, (i == 0 ? inputs[b] : L[i - 1].x));

	}

	for (unsigned i = 0; i < L.size(); ++i)
		L[i].addGrads(learningRate);
}

Vec NeuralNetwork::forwardProp(const Vec& input)
{
	PROFILE_FUNCTION();
	assert(input.size() == inputSize);

	for (unsigned i = 0; i < L.size(); ++i)
		L[i].forwardProp((i == 0 ? input : L[i - 1].x));

	return L.back().x;
}

void NeuralNetwork::split(std::vector<Vec>& inputsTrain, std::vector<Vec>& outputsTrain,
                          std::vector<Vec>& inputsTest,  std::vector<Vec>& outputsTest, double perc)
{
	PROFILE_FUNCTION();
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
	PROFILE_FUNCTION();
	unsigned in = (L.size() == 0 ? inputSize : L.back().outputs());
	L.push_back(Layer(in, outputSize, (loss == CROSS_ENTROPY ? Layer::SOFTMAX : Layer::SIGMOID)));

	std::vector<std::vector<real>> errors_train;
	std::vector<std::vector<real>> errors_test;

	Graphics G;
	G.setTitle("LOSS vs EPOCHS")
	.setYLabel("Loss")
	.setXLabel("Epochs")
	.setLMargin(10);

	std::cout << "Train on " << inputsTrain.size() << " samples, validate on " << inputsTest.size() <<
	          " samples." << std::endl;

	for (unsigned i = 1; i <= epochs; ++i) {
		std::cout << "EPOCH: " << i << "/" << epochs << std::endl;

		unsigned j, tot = inputsTrain.size() - inputsTrain.size() % batchSize;

		for (j = 0; j < tot; j += batchSize)
			backProp(inputsTrain.data() + j, outputsTrain.data() + j, batchSize, loss);

		if (j < inputsTrain.size())
			backProp(inputsTrain.data() + j, outputsTrain.data() + j, inputsTrain.size() - j, loss);

		auto current_loss_train = calculateTotalLoss(inputsTrain, outputsTrain, loss);
		auto current_loss_test = calculateTotalLoss(inputsTest, outputsTest, loss);

		std::cout << std::setprecision(2);
		std::cout << "Loss: " << current_loss_train[0]
		          << "(" << current_loss_train[1] << "," << current_loss_train[2] << ")"
		          << ", Val.Loss: " << current_loss_test[0]
		          << "(" << current_loss_test[1] << "," << current_loss_test[2] << ")"
		          << std::endl;

		errors_train.push_back({(real)i, current_loss_train[0], current_loss_train[1], current_loss_train[2]});
		errors_test.push_back({(real)i, current_loss_test[0],  current_loss_test[1],  current_loss_test[2]});

		G.restart(false)
		.setXRange(0., i + .5)
		.add(errors_train, "", 		"blue", 2)					// draw lines
		.add(errors_train, "Train", "blue", 0, false, false)	// draw error bars
		.add(errors_test,  "",  	"red",  2)					// draw lines
		.add(errors_test,  "Test",  "red",  0, false, false)	// draw error bars
		.plot();
	}
}

// #include "MNIST_Reader.hpp"

std::vector<real> NeuralNetwork::calculateTotalLoss(const std::vector<Vec>& inputs,
        const std::vector<Vec>& outputs, LossFunction loss)
{
	PROFILE_FUNCTION();
	real average  = 0.;
	std::vector<real> all_err(outputs.size());
	real error2 = 0.;

	for (unsigned i = 0; i < outputs.size(); ++i) {
		forwardProp(inputs[i]);
		all_err[i] = calculateLoss[loss](L.back().x, outputs[i]);
/*		if(all_err[i]>3){
			std::cout << "Point " << i << " bigger than 2: " << all_err[i] << " ==> " << std::exp(-all_err[i]) << std::endl;
			MNIST::print(Vec(inputs[i]));
			std::cout << L.back().x << " && " << outputs[i] << std::endl;
		}*/
		average  += all_err[i];
	}

	average  /= outputs.size();

	real stddev_minus = 0.;
	real stddev_plus  = 0.;
	unsigned N_minus  = 0;
	unsigned N_plus   = 0;

	for (unsigned i = 0; i < outputs.size(); ++i) {
		if (all_err[i] < average) {
			++N_minus;
			real err = all_err[i] - average;
			stddev_minus += err * err;
		}
		else {
			++N_plus;
			real err = all_err[i] - average;
			stddev_plus += err * err;
		}
	}

	stddev_minus = sqrt(stddev_minus / N_minus);
	stddev_plus  = sqrt(stddev_plus / N_plus);

	return {average, stddev_minus, stddev_plus};
}

real NeuralNetwork::calculateLoss_MSE(const Vec& x, const Vec& output)
{
	PROFILE_FUNCTION();
	real error = 0.;

	for (unsigned i = 0; i < x.size(); ++i) {
		real mult = x[i] - output[i];
		error += mult * mult;
	}

	return 0.5 * error;
}

real NeuralNetwork::calculateLoss_CE(const Vec& x, const Vec& output)
{
	PROFILE_FUNCTION();
	real error = 0.;

	for (unsigned i = 0; i < x.size(); ++i)
		error += output[i] * std::log(x[i]);

	return - error;
}

Vec NeuralNetwork::calculateLastDelta_MSE(const Vec& output, const Layer& lastL)
{
	PROFILE_FUNCTION();
	return lastL.getCalculateDelta()(lastL.x - output, lastL.x, lastL.z);
}

Vec NeuralNetwork::calculateLastDelta_CE(const Vec& output, const Layer& lastL)
{
	PROFILE_FUNCTION();
	Vec send(output.size());

	for (unsigned i = 0; i < output.size(); ++i)
		send[i] = - output[i] / lastL.x[i];

	return lastL.getCalculateDelta()(send, lastL.x, lastL.z);
}

