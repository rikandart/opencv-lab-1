#define MISHA
#ifndef MISHA
// opencv lib
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

// c++
#include <iostream>
#include <vector>
#include <thread>
#include <math.h>

using namespace cv;
bool incorrect = false;


void show(const Mat& original, const Mat& edited, const Mat& hist_orig, const Mat& hist_edited, const int width, const int height) {
	if (original.empty()) 
		std::cout << "Could not open or find the image original" << std::endl;
	if (edited.empty())
		std::cout << "Could not open or find the image edited" << std::endl;
	if (hist_orig.empty())
		std::cout << "Could not open or find the image original hist" << std::endl;
	if (hist_edited.empty())
		std::cout << "Could not open or find the image edited hist" << std::endl;
	
	// edited image and original image are the same size
	// original
	namedWindow("Grayed image", WINDOW_NORMAL | WINDOW_FREERATIO | WINDOW_GUI_EXPANDED);
	namedWindow("Histogram original", WINDOW_NORMAL | WINDOW_FREERATIO | WINDOW_GUI_EXPANDED);
	resizeWindow("Grayed image", width, height);
	moveWindow("Grayed image", 520, 0);
	resizeWindow("Histogram original", 512, 512);
	moveWindow("Histogram original", 0, 0);
	
	// edited
	namedWindow("Quantized image", WINDOW_NORMAL | WINDOW_FREERATIO | WINDOW_GUI_EXPANDED);
	namedWindow("Histogram edited", WINDOW_NORMAL | WINDOW_FREERATIO | WINDOW_GUI_EXPANDED);
	resizeWindow("Quantized image", width, height);
	moveWindow("Quantized image", 520, 554);
	resizeWindow("Histogram edited", 512, 512);
	moveWindow("Histogram edited", 0, 554);

	imshow("Grayed image", original);
	imshow("Quantized image", edited);
	imshow("Histogram original", hist_orig);
	imshow("Histogram edited", hist_edited);
	waitKey(0);
}

double standartDeviation(const Mat& original, const Mat& edited) {
	const unsigned m = original.rows, n = original.cols;
	double result = 0;
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			result += pow(original.at<unsigned char>(i, j)
				- edited.at<unsigned char>(i, j), 2);
	return sqrt(result/(m*n));
}

double standartSampleDeviation(const Mat& original) {
	static double result = 0;
	const unsigned m = original.rows, n = original.cols;
	static double mean = [&]()->double const {
		double result = 0;
		for (int i = 0; i < m; i++)
			for (int j = 0; j < n; j++)
				result += original.at<unsigned char>(i, j);
		return result / (m * n);
	}();
	if(!result)
		for (int i = 0; i < m; i++)
			for (int j = 0; j < n; j++)
				result += pow(original.at<unsigned char>(i, j)
					- mean, 2);
	return sqrt(result / (m * n - 1));
}

void editQuantizeLevel(const unsigned level, const Mat& img, Mat& dest_img) {
	if (remainder(256.0, level) != 0.0 || level == 0 || level == 1) {
		std::cout << "Incorrect quantize level" << std::endl;
		incorrect = true;
		return;
	}
	incorrect = false;
	// заполнение матрицы нулями
	dest_img = Mat::zeros(img.rows, img.cols, CV_8U);
	// шаг квантования
	const double step = 255.0 / (level-1);
	// пробегаем по пикселям x и y
	for (unsigned i = 0; i < img.cols; i++) {
		for (unsigned j = 0; j < img.rows; j++) {
			unsigned br = img.at<unsigned char>(j, i);
			for (unsigned k = 0; k < level; k++) {
				// смотрим в каком интервале лежит число
				if (br >= k * step && br <= (k+1) * step/2) {
					// смотрим, какая разница меньше и присваиваем нужную градацию
					dest_img.at<unsigned char>(j, i) = k * step;
					break;
				}
				else if (br >= k * step / 2 && br <= (k + 1) * step) {
					dest_img.at<unsigned char>(j, i) = (k + 1) * step;
					break;
				}
			}
		}
	}	
}

Mat getHist(const Mat& image)
{
	// Создаем заполненный нулями Mat-контейнер размером 1 x 256
	Mat hist = Mat::zeros(1, 256, CV_64FC1);

	// последовательно считываем яркость каждого элемента изображения
	// и увеличиваем на единицу значение соответствующего элемента матрицы hist
	for (int i = 0; i < image.cols; i++)
		for (int j = 0; j < image.rows; j++) {
			int r = image.at<unsigned char>(j, i);
			hist.at<double>(0, r) = hist.at<double>(0, r) + 1.0;
		}

	double m = 0, M = 0;
	minMaxLoc(hist, &m, &M); // ищем глобальный минимум и максимум
	hist = hist / M; // используем максимум для нормировки по высоте

	Mat hist_img = Mat::zeros(100, 256, CV_8U);

	for (int i = 0; i < 256; i++)
		for (int j = 0; j < 100; j++) {
			if (hist.at<double>(0, i) * 100 > j) {
				hist_img.at<unsigned char>(99 - j, i) = 255;
			}
		}
	bitwise_not(hist_img, hist_img); // инвертируем изображение
	return hist_img;
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
    Mat image, edited_img, hist_orig, hist_edited;
    image = imread("E:\\Диск Работа\\2018 фото\\5 разное\\IMG_4432.JPG", IMREAD_GRAYSCALE); // Read the file
    if( image.empty() ) {
        std::cout << "Could not open or find the image" << std::endl;
		return -1;
    }
	unsigned width = image.cols, height = image.rows;
	if (image.cols * image.rows > 1024 * 1024) {
		width /= 10;
		height /= 10;
	}
	int level = 2;
	const double stSampDev = standartSampleDeviation(image);
	while (true) {
		std::cout << "Enter quantize level (-1 to exit): \n\t" << std::endl;
		std::cin >> level;
		system("cls");
		if (level == -1)
			break;
		if (remainder(256.0, level) != 0.0 || level == 0) {
			std::cout << "Incorrect quantize level" << std::endl;
			continue;
		}
		editQuantizeLevel(level, image, edited_img);
		if(!incorrect)
			std::cout << "Quantize level: " << level << "\nStandart sample deviation: " << stSampDev << "\nStandart deviation: " << standartDeviation(image, edited_img) <<
			"\nEstimated standart deviation: " << (255.0/(level-1))/sqrt(12.0) << std::endl;
		hist_orig = getHist(image);
		hist_edited = getHist(edited_img);
		std::thread(show, image, edited_img, hist_orig, hist_edited, width, height).detach();
	}
    return 0;
}
#endif

