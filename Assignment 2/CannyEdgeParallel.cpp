#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include <chrono>
#include <iostream>


#define IMAGE_LOCATION "Examples/lena.bmp"
#define FIRST_THRESHOLD 50
#define SECOND_THRESHOLD 150
#define KERNEL_SIZE 3
#define NUMBER_OF_THREADS 4

using namespace cv;
using namespace std;

Mat inputImage, outputImage;
vector<Mat> inputImageBlocks, outputImageBlock (NUMBER_OF_THREADS);
int blockNumber = 0;

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

void canny_edge_filter_for_block(Mat block, int threadID)
{
	Mat blockGrayscale, edgeImage, blockOutput;
	blockOutput.create(block.size(), block.type());
	cvtColor(block, blockGrayscale, CV_BGR2GRAY);
	blur(blockGrayscale, edgeImage, Size(3, 3));
	Canny(edgeImage, edgeImage, FIRST_THRESHOLD, SECOND_THRESHOLD, KERNEL_SIZE);

	blockOutput = Scalar::all(0);

	block.copyTo(blockOutput, edgeImage);
	cvtColor(blockOutput, blockOutput, CV_BGR2GRAY);
	outputImageBlock[threadID].push_back(blockOutput);
	//imwrite("Examples/cany_edge_detection.jpg", blockOutput);
}

int main()
{
	double t1, t2;
	Timer tmr;
	thread threads[NUMBER_OF_THREADS];

	inputImage = imread(IMAGE_LOCATION);

	if (inputImage.empty()) {
		printf("Image not found, please restart the program with different image path! \n");
		return EXIT_FAILURE;
	}
	printf("Image found... Starting program\n");

	Mat maskImage = inputImage.clone();

	if (inputImage.rows % NUMBER_OF_THREADS == 0)
	{
		for (int x = 0; x < inputImage.rows; x += inputImage.rows / NUMBER_OF_THREADS)
		{
			inputImageBlocks.push_back(inputImage(Rect(0, x, (inputImage.cols), (inputImage.rows / NUMBER_OF_THREADS))).clone());
			rectangle(maskImage, Point(0, x), Point(0 + (maskImage.cols) - 1, x + (maskImage.rows / NUMBER_OF_THREADS) - 1), CV_RGB(255, 0, 0), 1); // visualization
			imshow("Mask image", maskImage); // visualization
		}
	}

	outputImage.create(inputImage.size(), inputImage.type());

	t1 = tmr.elapsed();
	for (blockNumber = 0; blockNumber < inputImageBlocks.size(); blockNumber++)
	{
		threads[blockNumber]= thread(canny_edge_filter_for_block, inputImageBlocks.at(blockNumber), blockNumber);
	}

	for (blockNumber = 0; blockNumber < inputImageBlocks.size(); blockNumber++)
	{
		threads[blockNumber].join();
	}
	
	vconcat(outputImageBlock.at(0), outputImageBlock.at(1), outputImage);
	for (blockNumber = 0; blockNumber < inputImageBlocks.size() - 2; blockNumber++)
	{
		vconcat(outputImage, outputImageBlock.at(blockNumber + 2), outputImage);
	}
	t2 = tmr.elapsed();
	imshow("Output image", outputImage);

	cout << "Pthread sum time: " << t2 - t1 << endl << endl;
	printf("Program has ended. Press any key to continue... \n");
	waitKey(0);

	return EXIT_SUCCESS;
}