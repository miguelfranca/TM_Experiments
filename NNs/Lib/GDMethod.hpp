#pragma once

namespace NN
{
	class Layer;

	class GDMethod
	{
	public:

		enum Type {
			GRADIENT_DESCENT,
			ADAM
		};

		virtual void setLayer(const Layer&) = 0;
		virtual void updateGrads(Layer& L) = 0;
	};


	class GradientDescent: public GDMethod
	{
	public:
		GradientDescent(real a_learningRate): learningRate(a_learningRate) {}

		void setLayer(const Layer&) {};
		void updateGrads(Layer& L);

	private:
		real learningRate;
	};

	class Adam: public GDMethod
	{
	public:
		Adam(real a_learningRate = 0.001, real a_beta1 = 0.9, real a_beta2 = 0.999, real a_epsilon = 1.e-8);

		void setLayer(const Layer& L);
		void updateGrads(Layer& L);

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