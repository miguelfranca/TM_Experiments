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

	std::vector<Vec> getImages(std::string MNIST_Path = "./");
	std::vector<Vec> getLabels(std::string MNIST_Path = "./");

	void print(Vec& image);
	void print(Vec& image, Vec& label);
	void print(std::vector<Vec>& images, std::vector<Vec>& labels, int n = -1);

	static const std::string images_path = "train-images-idx3-ubyte.dat";
	static const std::string labels_path = "train-labels-idx1-ubyte.dat";
}