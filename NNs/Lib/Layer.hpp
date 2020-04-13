#pragma once
#include <vector>
#include <cmath>
#include "typedefs.hpp"
#include "GDMethod.hpp"

namespace NN
{
	// typedef std::pointer_to_unary_function<void, const Vec&, Vec&> Func;
	typedef void (*Func)(const Vec&, Vec&);
	typedef Vec(*FuncD)(const Vec&, const Vec&, const Vec&);

	class NeuralNetwork;

	class Layer
	{
	public:
		// friend class NeuralNetwork;

		enum Activation {
			SIGMOID,
			SOFTMAX,
			RELU,
			UNDEFINED
		};

	public:
		Layer(unsigned inputSize, unsigned outputSize, GDMethod* gd, Activation a = SIGMOID);
		~Layer();

		void forwardProp(const Vec& input);
		void backProp(Vec& next, const Vec& prevX);
		void backProp(Vec& next, const Vec& prevX, const Vec& delta);

		void addGrads();

		inline Index outputs() const { return z.size(); }
		inline Index inputs() const { return W.cols(); }

		inline FuncD getCalculateDelta() const { return calculateDelta[func]; }

	private:
		void resetGrads();

		Activation func;

	public: // everyone needs to access them, see it like a struct
		Vec x;
		Vec z;
		Mat W;
		Vec b;

		Mat gradW;
		Vec gradb;

		GDMethod *gd;

	private:
		static Func activations[];
		static FuncD calculateDelta[];

		static std::pointer_to_unary_function<real, real> sigmoid_ptr;
		inline static real sigmoid(const real z) { return 1. / (1. + std::exp(-z)); }
		inline static real sigmoid_1stD(const real x) { return x * (1. - x); }
		inline static void sigmoidV(const Vec& z, Vec& x) { x = z.unaryExpr(sigmoid_ptr); }
		inline static Vec  calculateDelta_sigmoid(const Vec& next, const Vec& x, const Vec& z);

		inline static void crossEntropyV(const Vec& z, Vec& x);
		inline static Vec  calculateDelta_crossEntropy(const Vec& next, const Vec& x, const Vec& z);

		static std::pointer_to_unary_function<real, real> relu_ptr;
		inline static real relu(const real x){ return x < 0 ? 0 : x; }
		inline static void reluV(const Vec& z, Vec& x) { x = z.unaryExpr(relu_ptr); }
		inline static Vec  calculateDelta_relu(const Vec& next, const Vec& x, const Vec& z);
	};

}