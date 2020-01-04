#pragma once
#include <vector>
#include <string>
#include "typedefs.hpp"


namespace MNIST
{
	struct Image {
		unsigned char data[28][28];
	};

	struct Label {
		unsigned char data;
	};

	std::vector<Vec> getImages(const std::string& MNIST_Path = "./");
	std::vector<Vec> getLabels(const std::string& MNIST_Path = "./");

	void print(const Vec& image);
	void print(const Vec& image, const Vec& label);
	void print(const std::vector<Vec>& images, const std::vector<Vec>& labels, int n = -1);

	static const std::string images_path = "train-images-idx3-ubyte.dat";
	static const std::string labels_path = "train-labels-idx1-ubyte.dat";
}