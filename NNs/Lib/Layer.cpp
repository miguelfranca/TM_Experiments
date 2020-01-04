#include <cassert>
#include "Layer.hpp"
#include "Instrumentor.h"

Func Layer::activations[] 		= { Layer::sigmoidV, Layer::crossEntropyV } ;
FuncD Layer::calculateDelta[] 	= { Layer::calculateDelta_sigmoid, Layer::calculateDelta_crossEntropy };

std::pointer_to_unary_function<real, real> Layer::sigmoid_ptr = std::ptr_fun(sigmoid);

Layer::Layer(unsigned inputSize, unsigned outputSize, Activation a) : x(outputSize),
	z(outputSize), W(Mat::Random(outputSize, inputSize)),
	b(Vec::Random(outputSize)), func(a)
{
	resetGrads();
}

Layer::~Layer()
{

}

void Layer::resetGrads()
{
	PROFILE_FUNCTION();
	gradW = Mat::Zero(outputs(), inputs());
	gradb = Vec::Zero(outputs());
}

void Layer::addGrads(real learningRate)
{
	PROFILE_FUNCTION();
	W -= learningRate * gradW;
	b -= learningRate * gradb;
	resetGrads();
}

void Layer::forwardProp(const Vec& input)
{
	PROFILE_FUNCTION();
	z = W * input + b;
	activations[func](z, x);
}

void Layer::backProp(Vec& next, const Vec& prevX)
{
	PROFILE_FUNCTION();
	Vec delta = calculateDelta[func](next, x, z);
	backProp(next, prevX, delta);
}

void Layer::backProp(Vec& next, const Vec& prevX, const Vec& delta)
{
	PROFILE_FUNCTION();
	next = (delta.transpose() * W).transpose();
	gradW += (delta * prevX.transpose());
	gradb += delta;
}

Vec Layer::calculateDelta_sigmoid(const Vec& next, const Vec& x, const Vec& z)
{
	Vec delta(next.size());

	for (unsigned i = 0; i < next.size(); ++i)
		delta[i] = next[i] * sigmoid_1stD(x[i]);

	return delta;
}

void Layer::crossEntropyV(const Vec& z, Vec& x)
{
	x = z.unaryExpr([](real zi) { return std::exp(zi); });
	real total = x.sum();
	x = x.unaryExpr([total](real exp_zi) { return exp_zi / total;});
}

Vec Layer::calculateDelta_crossEntropy(const Vec& next, const Vec& x, const Vec& z)
{
	real dot = next.dot(x);
	return next.cwiseProduct(x) - x * dot;
}
