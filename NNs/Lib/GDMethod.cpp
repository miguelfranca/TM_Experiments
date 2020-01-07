
#include "Layer.hpp"
#include "GDMethod.hpp"
#include <iostream>

using namespace NN;

void GradientDescent::updateGrads(Layer& L)
{
	L.W -= learningRate * L.gradW;
	L.b -= learningRate * L.gradb;
};

Adam::Adam(real a_learningRate, real a_beta1, real a_beta2, real a_epsilon):
	learningRate(a_learningRate),
	beta1(a_beta1), beta1_pow(a_beta1),
	beta2(a_beta2), beta2_pow(a_beta2),
	epsilon(a_epsilon)
{}

void Adam::setLayer(const Layer& L)
{
	m_W = Mat::Zero(L.W.rows(), L.W.cols());
	v_W = m_W;
	m_b = Vec::Zero(L.b.size(), 1);
	v_b = m_b;
};
void Adam::updateGrads(Layer& L)
{
	for (int i = 0; i < m_W.rows(); ++i) {
		for (int j = 0; j < m_W.cols(); ++j) {
			real gradW  = L.gradW(i, j);
			real gradW2 = gradW * gradW;

			m_W(i, j) = beta1 * m_W(i, j) + (1. - beta1) * gradW;
			v_W(i, j) = beta2 * v_W(i, j) + (1. - beta2) * gradW2;

			real m_W_no_bias = m_W(i, j) / (1. - beta1_pow);
			real v_W_no_bias = v_W(i, j) / (1. - beta2_pow);

			L.W(i, j) -= learningRate * m_W_no_bias / (std::sqrt(v_W_no_bias) + epsilon);
		}

		real gradb  = L.gradb[i];
		real gradb2 = gradb * gradb;

		m_b[i] = beta1 * m_b[i] + (1. - beta1) * gradb;
		v_b[i] = beta2 * v_b[i] + (1. - beta2) * gradb2;

		m_b[i] /= (1. - beta1_pow);
		v_b[i] /= (1. - beta2_pow);

		L.b[i] -= learningRate * m_b[i] / (std::sqrt(v_b[i]) + epsilon);
	}

	beta1_pow *= beta1;
	beta2_pow *= beta2;
};