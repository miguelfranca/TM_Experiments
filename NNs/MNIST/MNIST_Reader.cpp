#include "Instrumentor.h"
#include "MNIST_Reader.hpp"
#include <fstream>
#include <iostream>

namespace MNIST
{

	std::vector<Vec> getImages(const std::string& MNIST_Path)
	{
		PROFILE_FUNCTION();

		std::ifstream file(MNIST_Path + images_path, std::ios::in);

		unsigned magic_number;
		unsigned n_images;
		unsigned n_cols;
		unsigned n_rows;

		file.read((char*)&magic_number, sizeof(unsigned));
		file.read((char*)&n_images, 	sizeof(unsigned));
		file.read((char*)&n_rows, 		sizeof(unsigned));
		file.read((char*)&n_cols, 		sizeof(unsigned));

		std::vector<Image> images(n_images);

		file.read((char*)images.data(), n_images * sizeof(Image));

		file.close();

		std::vector<Vec> imagesVec(n_images);

		for (unsigned i = 0; i < n_images; ++i) {
			Vec imageData(28 * 28);

			for (unsigned j = 0; j < 28; ++j)
				for (unsigned k = 0; k < 28; ++k)
					imageData(j * 28 + k) = images[i].data[j][k];

			imagesVec[i] = imageData;
		}

		return imagesVec;
	}

	std::vector<Vec> getLabels(const std::string& MNIST_Path)
	{
		PROFILE_FUNCTION();

		std::ifstream file(MNIST_Path + labels_path, std::ios::in);

		unsigned magic_number;
		unsigned n_labels;

		file.read((char*)&magic_number, sizeof(unsigned));
		file.read((char*)&n_labels, 	sizeof(unsigned));

		std::vector<Label> labels(n_labels);

		file.read((char*)labels.data(), n_labels * sizeof(Label));

		file.close();

		std::vector<Vec> labelsVec(n_labels);

		for (unsigned i = 0; i < n_labels; ++i) {
			Vec labelData = Vec::Zero(10);
			labelData(labels[i].data) = 1;
			labelsVec[i] = labelData;
		}

		return labelsVec;
	}

	void print(const Vec& image)
	{
		for (unsigned j = 0; j < 28; ++j) {
			for (unsigned k = 0; k < 28; ++k)
				std::cout << (image(j * 28 + k) > 128 ? "1" : " ") << " ";

			std::cout << std::endl;
		}
	}

	void print(const Vec& image, const Vec& label)
	{
		for (unsigned j = 0; j < 28; ++j) {
			for (unsigned k = 0; k < 28; ++k)
				std::cout << (image(j * 28 + k) > 128 ? "1" : " ") << " ";

			std::cout << std::endl;
		}

		for (unsigned i = 0; i < label.size(); ++i)
			std::cout << i << ": " << label(i) << std::endl;
	}

	void print(const std::vector<Vec>& images, const std::vector<Vec>& labels, int n)
	{
		std::cout << "IMAGES: " << images.size() << std::endl;
		std::cout << "LABELS: " << labels.size() << std::endl;

		for (unsigned i = 0; i < n; ++i)
			print(images[i], labels[i]);
	}
}