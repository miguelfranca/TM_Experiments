#pragma once
#include "Layer.hpp"
#include "GDMethod.hpp"

namespace NN
{

	typedef Vec(*FuncLastD)(const Vec&, const Layer&);
	typedef real(*FuncLoss)(const Vec&, const Vec&);

	class NeuralNetwork
	{
	public:
		enum LossFunction {
			MSE,
			CROSS_ENTROPY
		};



		NeuralNetwork(unsigned inputSize, unsigned outputSize, GDMethod::Type a_gd = GDMethod::GRADIENT_DESCENT,
		              real learningRate = 0.001, int srand_seed = -1);

		~NeuralNetwork();

		void add(unsigned layerSize, Layer::Activation a = Layer::SIGMOID);

		Vec forwardProp(const Vec& input);

		static void split(std::vector<Vec>& inputsTrain, std::vector<Vec>& outputsTrain,
		                  std::vector<Vec>& inputsTests, std::vector<Vec>& outputTests, double perc);

		void train(const std::vector<Vec>& inputsTrain, const std::vector<Vec>& outputsTrain,
		           const std::vector<Vec>& inputTests,
		           const std::vector<Vec>& outputTests, unsigned epochs, unsigned batchSize = 1,
		           LossFunction loss = MSE, bool calculate_categorical_accucary = false);

		void setActivationLastLayer(Layer::Activation a);

	private:
		void backProp(const Vec* inputs, const Vec* outputs, unsigned batchSize, LossFunction loss);

		static Vec calculateLastDelta_MSE(const Vec& output, const Layer& lastL);
		static real calculateLoss_MSE(const Vec& x, const Vec& output);

		static Vec calculateLastDelta_CE(const Vec& output, const Layer& lastL);
		static real calculateLoss_CE(const Vec& x, const Vec& ouput);

		// outputs error and std.dev (to each side)
		std::vector<real> calculateTotalLoss(const std::vector<Vec>& inputs,
		                                     const std::vector<Vec>& outputs, LossFunction loss,
		                                     bool calculate_categorical_accucary);

		GDMethod* makeGDMethod();

		bool categorical_accucary(const Vec& x, const Vec& output);

	private:
		std::vector<Layer> L;
		GDMethod::Type gd_t;

		const unsigned inputSize;
		const unsigned outputSize;

		const real learningRate;

		Layer::Activation act_last_layer;

	private:
		static FuncLastD calculateLastDelta[];
		static FuncLoss calculateLoss[];
	};

}