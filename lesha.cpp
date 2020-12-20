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
	// c������ ����������� ������ Mat-��������� �������� 1 x 256
	Mat hist = Mat::zeros(1, 256, CV_64FC1);

	// ��������������� ��������� ������� ������� �������� �����������
	// � ����������� �� ������� �������� ���������������� �������� ������� hist
	for (int i = 0; i < img.cols; i++)
		for (int j = 0; j < img.rows; j++)
		{
			int r = img.at<unsigned char>(j, i);
			hist.at<double>(0, r) = hist.at<double>(0, r) + 1.0;
		}

	double m = 0, M = 0;
	minMaxLoc(hist, &m, &M); // ���� ���������� ������� � ��������
	hist = hist / M; // ���������� �������� �� ���������� �� ������

	Mat hist_img = Mat::zeros(100, 256, CV_8U);

	for (int i = 0; i < 256; i++)
		for (int j = 0; j < 100; j++)
		{
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
	Mat image = imread("E:/Ford_model_t_touring_1.jpg", IMREAD_GRAYSCALE);;
	for(int q_level = 2; q_level <= 64; q_level *= 2){
		//�������� �����������
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
		//������ ��������������������� ���������� �������� � ���
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
		cout << "������� ����������� " << q_level << endl;
		cout << "��� �������� " << ots_sko << endl;
		cout << "��� " << sko << endl;
		cout << "������ ��� " << ocenka << endl;
		Mat gist_image = getHist(image);
		Mat gist_kvant = getHist(kvant_image);
		imshow("�����������", image);
		imshow("������������ �����������", kvant_image);
		imshow("�����������", gist_image);
		imshow("����������� ������������� �����������", gist_kvant);
		waitKey(0);
		system("cls");
	}
	return 0;
}