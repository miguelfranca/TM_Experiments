#pragma once
#include <vector>
#include <cmath>
#include "typedefs.hpp"

typedef std::pointer_to_unary_function<real, real> Func;

class NeuralNetwork;

class Layer
{
public:
	friend class NeuralNetwork;

	enum Activation{
		SIGMOID
	};

public:
	Layer(unsigned inputSize, unsigned outputSize, Activation a = SIGMOID);
	~Layer();

	const Vec& forwardProp(const Vec& input);

	inline Index outputs() { return W.rows(); }
	inline Index inputs() { return W.cols(); }


private:
	Vec x;
	Vec z;
	Mat W;
	Vec b;
	Activation func;

private:
	static Func activations[];
	static Func activations_1stD[];

	inline static real sigmoid(real x) { return 1. / ( 1. + std::exp(-x)); }
	inline static real sigmoid_1stD(real x) { real s = sigmoid(x); return (1. - s) * s; }
};
