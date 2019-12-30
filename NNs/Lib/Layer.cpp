#include <cassert>
#include "Layer.hpp"

Func Layer::activations[] 		= { std::ptr_fun(Layer::sigmoid) } ;
Func Layer::activations_1stD[] 	= { std::ptr_fun(Layer::sigmoid_1stD) };

Layer::Layer(unsigned inputSize, unsigned outputSize, Activation a) : x(outputSize),
	z(outputSize), W(Mat::Random(outputSize,
	                                 inputSize)), b(outputSize), func(a)
{

}

Layer::~Layer()
{

}

const Vec& Layer::forwardProp(const Vec& input)
{
	z = W * input + b;
	x = z.unaryExpr(activations[func]);
	return x;
}