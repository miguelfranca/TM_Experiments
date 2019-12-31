#pragma once
#include <vector>
#include <cmath>
#include "typedefs.hpp"

// typedef std::pointer_to_unary_function<void, const Vec&, Vec&> Func;
typedef void (*Func)(const Vec&, Vec&);
typedef Vec(*FuncD)(const Vec&, const Vec&, const Vec&);

class NeuralNetwork;

class Layer
{
public:
	friend class NeuralNetwork;

	enum Activation {
		SIGMOID,
		SOFTMAX
	};

public:
	Layer(unsigned inputSize, unsigned outputSize, Activation a = SIGMOID);
	~Layer();

	const Vec& forwardProp(const Vec& input);
	void backProp(Vec& next, const Vec& prevX);
	void backProp(Vec& next, const Vec& prevX, const Vec& delta);

	void addGrads(real learningRate);

	inline Index outputs() const { return z.size(); }
	inline Index inputs() const { return W.cols(); }

	inline FuncD getCalculateDelta() const { return calculateDelta[func]; }

private:
	void resetGrads();

private:
	Vec x;
	Vec z;
	Mat W;
	Vec b;
	Activation func;

	Mat gradW;
	Vec gradb;

private:
	static Func activations[];
	static FuncD calculateDelta[];

	static std::pointer_to_unary_function<real, real> sigmoid_ptr;
	inline static real sigmoid(real z) { return 1. / (1. + std::exp(-z)); }
	inline static real sigmoid_1stD(real x) { return x * (1. - x); }
	inline static void sigmoidV(const Vec& z, Vec& x) { x = z.unaryExpr(sigmoid_ptr); }
	inline static Vec  calculateDelta_sigmoid(const Vec& next, const Vec& x, const Vec& z);

	inline static void crossEntropyV(const Vec& z, Vec& x);
	inline static Vec  calculateDelta_crossEntropy(const Vec& next, const Vec& x, const Vec& z);
};
