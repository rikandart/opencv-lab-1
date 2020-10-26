#define MISHA
#ifdef MISHA
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <math.h>

using namespace cv;

Mat getHist(const Mat& img)
{
	// c������ ����������� ������ Mat-��������� �������� 1 x 256
	Mat hist = Mat::zeros(1, 256, CV_64FC1);

	// ��������������� ��������� ������� ������� �������� �����������
	// � ����������� �� ������� �������� ���������������� �������� ������� hist
	for (int i = 0; i < img.cols; i++)
		for (int j = 0; j < img.rows; j++) {
			int r = img.at<unsigned char>(j, i);
			hist.at<double>(0, r) = hist.at<double>(0, r) + 1.0;
		}

	double m = 0, M = 0;
	minMaxLoc(hist, &m, &M); // ���� ���������� ������� � ��������
	hist = hist / M; // ���������� �������� �� ���������� �� ������

	Mat hist_img = Mat::zeros(100, 256, CV_8U);

	for (int i = 0; i < 256; i++)
		for (int j = 0; j < 100; j++) {
			if (hist.at<double>(0, i) * 100 > j) {
				hist_img.at<unsigned char>(99 - j, i) = 255;
			}
		}
	bitwise_not(hist_img, hist_img); // ����������� �����������
	return hist_img;
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	Mat img;
	Mat q_img;
	Mat hist_orig;
	Mat hist_q;
	img = imread("E:\\���� ������\\2018 ����\\2 ��������\\IMG_4462.JPG", IMREAD_GRAYSCALE);
	int q_level = 0;
	std::cout << "������� ����� ������� �����������: ";
	std::cin >> q_level;
	
	// �����������
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
	// ������ ��������������������� ���������� �������� � ���
	double samp_sko = 0;
	double srednee = 0;
	double sko = 0;

	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			srednee += img.at<unsigned char>(i, j);
		}
	}
	srednee /= img.rows * img.cols;

	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			sko += pow(img.at<unsigned char>(i, j) - q_img.at<unsigned char>(i, j), 2);
			samp_sko += pow(img.at<unsigned char>(i, j) - srednee, 2);
		}
	}

	samp_sko = sqrt(samp_sko / (img.rows * img.cols - 1));
	sko = sqrt(sko / (img.rows * img.cols));
		
	std::cout << "������: " << q_level << "\n��� ��������: " << samp_sko << "\n���: " << sko <<
		"\n������: " << (255.0 / (q_level - 1)) / sqrt(12.0) << std::endl;

	hist_orig = getHist(img);
	hist_q = getHist(q_img);
	namedWindow("��������", WINDOW_NORMAL | WINDOW_FREERATIO | WINDOW_GUI_EXPANDED);
	namedWindow("����������� �������", WINDOW_NORMAL | WINDOW_FREERATIO | WINDOW_GUI_EXPANDED);
	namedWindow("������������ �����������", WINDOW_NORMAL | WINDOW_FREERATIO | WINDOW_GUI_EXPANDED);
	namedWindow("����������� ������������� �����������", WINDOW_NORMAL | WINDOW_FREERATIO | WINDOW_GUI_EXPANDED);
	imshow("��������", img);
	imshow("������������ �����������", q_img);
	imshow("����������� �������", hist_orig);
	imshow("����������� ������������� �����������", hist_q);
	waitKey(0);
	return 0;
}
#endif