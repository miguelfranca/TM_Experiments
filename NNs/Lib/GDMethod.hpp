#pragma once

#include "Layer.hpp"

namespace NN
{

	class GDMethod
	{
	public:

		virtual void setLayer(const Layer&) = 0;
		virtual void updateGrads(Layer& L) = 0;
	};


	class GradientDescent: public GDMethod
	{
	public:
		GradientDescent(real a_learningRate): learningRate(a_learningRate) {}

		void setLayer(const Layer&) {};
		void updateGrads(Layer& L)
		{
			W -= learningRate * gradW;
			b -= learningRate * gradb;
		};

	private:
		real learningRate;
	};

	class Adam: public GDMethod
	{
	public:
		Adam(real a_learningRate = 0.001, real a_beta1 = 0.9, real a_beta2 = 0.999, real a_epsilon = 1.e-7):
			learningRate(a_learningRate),
			beta1(a_beta1), beta1_pow(beta1),
			beta2(a_beta2), beta2_pow(beta2),
			epsilon(a_epsilon)
		{
		}

		void setLayer(const Layer& L)
		{
			m_W = Mat::Zero(L.W.cols(), L.W.cols());
			v_W = m_W;
			m_b = Vec::Zero(L.b.size(), 1);
			v_b = m_b;
		};
		void updateGrads(Layer& L)
		{
			auto gradb2 = L.gradb.square();
			auto gradW2 = L.gradW.square();

			for (int i = 0; i < m_b.cols(); ++i) {
				for (int j = 0; j < m_b.rows(); ++j) {
					m_W(i, j) = beta1 * m_W(i, j) + (1. - beta1) * L.gradW(i, j);
					v_W(i, j) = beta2 * v_W(i, j) + (1. - beta2) * gradW2(i, j);

					m_W(i, j) /= (1. - beta1_pow);
					v_W(i, j) /= (1. - beta2_pow);

					W(i, j) -= learningRate * m_W(i, j) / (std::sqrt(v_W(i, j)) + epsilon);
				}

				m_b[i] = beta1 * m_b[i] + (1. - beta1) * L.gradb[i];
				v_b[i] = beta2 * v_b[i] + (1. - beta2) * gradb2[i];

				m_b[i] /= (1. - beta1_pow);
				v_b[i] /= (1. - beta2_pow);

				b[i] -= learningRate * m_b[i] / (std::sqrt(v_b[i]) + epsilon);
			}

			beta1_pow *= beta1;
			beta2_pow *= beta2;
		};

	private:
		real learningRate;
		real beta1, beta1_pow;
		real beta2, beta2_pow;
		real epsilon;

		Mat m_W;
		Mat v_W;
		Vec m_b;
		Vec v_b;
	};

}