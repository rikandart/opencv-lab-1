#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <math.h>

using namespace cv;

Mat getHist(const Mat& img)
{
	// cоздаем заполненный нулями Mat-контейнер размером 1 x 256
	Mat hist = Mat::zeros(1, 256, CV_64FC1);

	// последовательно считываем ¤ркость каждого элемента изображения
	// и увеличиваем на единицу значение соответствующего элемента матрицы hist
	for (int i = 0; i < img.cols; i++)
		for (int j = 0; j < img.rows; j++) {
			int r = img.at<unsigned char>(j, i);
			hist.at<double>(0, r) = hist.at<double>(0, r) + 1.0;
		}

	double m = 0, M = 0;
	minMaxLoc(hist, &m, &M); // ищем глобальный минимум и максимум
	hist = hist / M; // используем максимум дл¤ нормировки по высоте

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
	Mat img;
	Mat q_img;
	Mat hist_orig;
	Mat hist_q;
	/**
		читай комментарий ниже
	**/
	// вставь сюда путь к картинке и не забудь удалить этот комментарий и верхний
	img = imread("E:\\ƒиск –абота\\2018 фото\\5 разное\\IMG_4432.JPG", IMREAD_GRAYSCALE);
	int q_level = 0;
	std::cout << "¬ведите число уровней квантовани¤: ";
	std::cin >> q_level;
	std::cout << std::endl;
	// квантование
	q_img = Mat::zeros(img.rows, img.cols, CV_8U);
	const double inter = 255.0 / (q_level - 1);
	for (int i = 0; i < img.cols; i++) {
		for (int j = 0; j < img.rows; j++) {
			int br = img.at<unsigned char>(j, i);
			for (int k = 0; k < q_level; k++) {
				if (br >= k * inter && br <= (k + 1) * inter / 2) {
					q_img.at<unsigned char>(j, i) = k * inter;
					break;
				}
				else if (br >= k * inter / 2 && br <= (k + 1) * inter) {
					q_img.at<unsigned char>(j, i) = (k + 1) * inter;
					break;
				}
			}
		}
	}
	// расчет ско
	double sko = 0;
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			sko += pow(img.at<unsigned char>(i, j) - q_img.at<unsigned char>(i, j), 2);
		}
	}
	sko = sqrt(1 / (img.rows * img.cols) * sko);

	std::cout << "”ровни: " << q_level << "\n— ќ: " << sko <<
		"\nќценка: " << (255.0 / (q_level - 1)) / sqrt(12.0) << std::endl;

	hist_orig = getHist(img);
	hist_q = getHist(q_img);
	namedWindow("ќригинал", WINDOW_NORMAL | WINDOW_FREERATIO | WINDOW_GUI_EXPANDED);
	namedWindow("√истограмма ригинал", WINDOW_NORMAL | WINDOW_FREERATIO | WINDOW_GUI_EXPANDED);
	namedWindow(" вантованное изображение", WINDOW_NORMAL | WINDOW_FREERATIO | WINDOW_GUI_EXPANDED);
	namedWindow("√истограмма квантованного изображени¤", WINDOW_NORMAL | WINDOW_FREERATIO | WINDOW_GUI_EXPANDED);
	imshow("ќригинал", img);
	imshow(" вантованное изображение", q_img);
	imshow("√истограмма ригинал", hist_orig);
	imshow("√истограмма квантованного изображени¤", hist_q);
	waitKey(0);
	return 0;
}