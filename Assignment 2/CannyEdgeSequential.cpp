#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <chrono>

#define IMAGE_LOCATION "Examples/forest.jpg"
#define FIRST_THRESHOLD 50
#define SECOND_THRESHOLD 150
#define KERNEL_SIZE 3

cv::Mat inputImage, outputImage, inputImageGrayscale, edgeImage;

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

void canny_edge_filter()
{
	blur(inputImageGrayscale, edgeImage, cv::Size(3, 3));

	Canny(edgeImage, edgeImage, FIRST_THRESHOLD, SECOND_THRESHOLD, KERNEL_SIZE);

	outputImage = cv::Scalar::all(0);

	inputImage.copyTo(outputImage, edgeImage);

	cvtColor(outputImage, outputImage, CV_BGR2GRAY);
}

int main() 
{
	double t1, t2;
	Timer tmr;

	inputImage = cv::imread(IMAGE_LOCATION);
	if (inputImage.empty()) {
		std::cout << "Image not found, please restart the program with different image path!" << std::endl;
		getchar();

		return EXIT_FAILURE;
	}
	std::cout << "Image found... Starting program\n" << std::endl;

	imshow("Input image", inputImage);

	outputImage.create(inputImage.size(), inputImage.type());
	cvtColor(inputImage, inputImageGrayscale, CV_BGR2GRAY);
	t1 = tmr.elapsed();
	canny_edge_filter();
	t2 = tmr.elapsed();
	imshow("Output image", outputImage);

	std::cout << "Sequential canny edge time: " << t2 - t1 << std::endl << std::endl;

	printf("Program has ended. Press any key to continue... \n");
	cv::waitKey(0);

	return EXIT_SUCCESS;
}