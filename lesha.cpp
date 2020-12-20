#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <math.h>
using namespace cv;
using namespace std;

Mat getHist(const Mat& img)
{
	// cоздаем заполненный нулями Mat-контейнер размером 1 x 256
	Mat hist = Mat::zeros(1, 256, CV_64FC1);

	// последовательно считываем яркость каждого элемента изображения
	// и увеличиваем на единицу значение соответствующего элемента матрицы hist
	for (int i = 0; i < img.cols; i++)
		for (int j = 0; j < img.rows; j++)
		{
			int r = img.at<unsigned char>(j, i);
			hist.at<double>(0, r) = hist.at<double>(0, r) + 1.0;
		}

	double m = 0, M = 0;
	minMaxLoc(hist, &m, &M); // ищем глобальный минимум и максимум
	hist = hist / M; // используем максимум дл¤ нормировки по высоте

	Mat hist_img = Mat::zeros(100, 256, CV_8U);

	for (int i = 0; i < 256; i++)
		for (int j = 0; j < 100; j++)
		{
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
	Mat image = imread("E:/Ford_model_t_touring_1.jpg", IMREAD_GRAYSCALE);;
	for(int q_level = 2; q_level <= 64; q_level *= 2){
		//алгоритм квантования
		Mat kvant_image = Mat::zeros(image.rows, image.cols, CV_8U);
		double inter = 255.0 / (q_level - 1);
		for (int i = 0; i < image.cols; i++)
		{
			for (int j = 0; j < image.rows; j++)
			{
				int pix = image.at<unsigned char>(j, i);
				for (int k = 0; k < q_level; k++)
				{
					if (pix >= k * inter && pix <= (k + 1) * inter / 2)
					{
						kvant_image.at<unsigned char>(j, i) = k * inter;
						break;
					}
					else if (pix >= k * inter / 2 && pix <= (k + 1) * inter)
					{
						kvant_image.at<unsigned char>(j, i) = (k + 1) * inter;
						break;
					}
				}
			}
		}
		//расчет среднеквадратического отклонения отсчетов и ско
		double srednee = 0;
		for (int i = 0; i < image.rows; i++)
		{
			for (int j = 0; j < image.cols; j++)
			{
				srednee += image.at<unsigned char>(i, j);
			}
		}
		srednee /= image.rows * image.cols;
		double diff_sko = 0;
		double diff_ots_sko = 0;
		for (int i = 0; i < image.rows; i++)
		{
			for (int j = 0; j < image.cols; j++)
			{
				int diff = image.at<unsigned char>(i, j) - kvant_image.at<unsigned char>(i, j);
				diff_sko += diff * diff;
				diff = image.at<unsigned char>(i, j) - srednee;
				diff_ots_sko += diff * diff;
			}
		}
		double ots_sko = sqrt(diff_ots_sko / (image.rows * image.cols - 1));
		double sko = sqrt(diff_sko / (image.rows * image.cols));
		double ocenka = (255.0 / (q_level - 1)) / sqrt(12.0);
		cout << "Уровней квантования " << q_level << endl;
		cout << "Ско отсчетов " << ots_sko << endl;
		cout << "Ско " << sko << endl;
		cout << "Оценка ско " << ocenka << endl;
		Mat gist_image = getHist(image);
		Mat gist_kvant = getHist(kvant_image);
		imshow("Изображение", image);
		imshow("Квантованное изображение", kvant_image);
		imshow("Гистограмма", gist_image);
		imshow("Гистограмма квантованного изображения", gist_kvant);
		waitKey(0);
		system("cls");
	}
	return 0;
}