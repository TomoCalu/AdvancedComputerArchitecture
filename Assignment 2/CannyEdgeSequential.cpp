#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <chrono>

#define IMAGE_LOCATION "Examples/lena.bmp"
#define FIRST_THRESHOLD 50
#define SECOND_THRESHOLD 150
#define KERNEL_SIZE 3

using namespace cv;
using namespace std;

Mat inputImage, outputImage, inputImageGrayscale, edgeImage;

class Timer
{
public:
	Timer() : beg_(clock_::now()) {}
	void reset() { beg_ = clock_::now(); }
	double elapsed() const {
		return std::chrono::duration_cast<std::chrono::microseconds>
			(clock_::now() - beg_).count();
	}

private:
	typedef std::chrono::high_resolution_clock clock_;
	typedef std::chrono::duration<double, std::ratio<1> > second_;
	std::chrono::time_point<clock_> beg_;
};

void canny_edge_filter(int, void*)
{
	blur(inputImageGrayscale, edgeImage, Size(3, 3));

	Canny(edgeImage, edgeImage, FIRST_THRESHOLD, SECOND_THRESHOLD, KERNEL_SIZE);

	outputImage = Scalar::all(0);

	inputImage.copyTo(outputImage, edgeImage);

	cvtColor(outputImage, outputImage, CV_BGR2GRAY);
	//imwrite("Examples/cany_edge_detection.jpg", outputImage);
}

int main() 
{
	double t1, t2;
	Timer tmr;

	inputImage = imread(IMAGE_LOCATION);
	if (inputImage.empty()) {
		printf("Image not found, please restart the program with different image path! \n");
		return EXIT_FAILURE;
	}
	printf("Image found... Starting program\n");

	imshow("Input image", inputImage);

	outputImage.create(inputImage.size(), inputImage.type());
	cvtColor(inputImage, inputImageGrayscale, CV_BGR2GRAY);
	t1 = tmr.elapsed();
	canny_edge_filter(0, 0);
	t2 = tmr.elapsed();
	imshow("Output image", outputImage);

	cout << "Pthread sum time: " << t2 - t1 << endl << endl;

	printf("Program has ended. Press any key to continue... \n");
	waitKey(0);

	return EXIT_SUCCESS;
}