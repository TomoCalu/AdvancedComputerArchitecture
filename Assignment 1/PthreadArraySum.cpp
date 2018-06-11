#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <chrono>

#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>

#define ARRAY_LENGTH 100000
#define NUM_THREADS 4

int numbersToAdd[ARRAY_LENGTH];
int sumWithOnePthread[ARRAY_LENGTH / NUM_THREADS] = { 0 };
int part = 0;

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

void* GetSumWithMultiplePthreads(void* threadID) {
	int thread_part = part++;

	for (int i = thread_part * (ARRAY_LENGTH / NUM_THREADS); i < (thread_part + 1) * (ARRAY_LENGTH / NUM_THREADS); i++)
		sumWithOnePthread[thread_part] += numbersToAdd[i];
	return (void*)sumWithOnePthread;
}

int main() {
	int arraySequentialSum = 0;
	double t1, t2;
	Timer tmr;
	int i;
	pthread_t threads[NUM_THREADS];
	pthread_attr_t attr;
	int rc;
	int totalPthreadSum = 0;

	std::cout<< "-------------Pthread array sum example-------------" << std::endl << std::endl;

	std::cout << "Generating array. " << std::endl;
	for (i = 0; i < ARRAY_LENGTH; i++) {
		numbersToAdd[i] = rand() % 100;
	}
	std::cout << std::endl;

	srand(time(NULL));
	t1 = tmr.elapsed();
	for (i = 0; i < ARRAY_LENGTH; i++) {
		arraySequentialSum = arraySequentialSum + numbersToAdd[i];
	}
	t2 = tmr.elapsed();

	std::cout << "Sequential sum = " << arraySequentialSum << std::endl;
	std::cout << "Sequential sum time: " << t2 - t1 << std::endl << std::endl;

	std::cout << "Starting pthreads. " << std::endl;

	t1 = tmr.elapsed();
	for (i = 0; i < NUM_THREADS; i++) {
		std::cout << "main() : creating thread, " << i << std::endl;
		rc = pthread_create(&threads[i], NULL, GetSumWithMultiplePthreads, (void *)NULL);

		if (rc) {
			std::cout << "Error:unable to create thread," << rc << std::endl;
			return EXIT_FAILURE;
		}
	}

	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}

	for (int i = 0; i < NUM_THREADS; i++) {
		totalPthreadSum += sumWithOnePthread[i];
	}
	std::cout << std::endl;
	std::cout << "Total sum with pthreads: " << totalPthreadSum << std::endl;

	t2 = tmr.elapsed();
	std::cout << "Pthread sum time: " << t2 - t1 << std::endl << std::endl;

	std::cout << "-----------------Program has ended-----------------" << std::endl;
	std::cout << "Press Enter to continue." << std::endl;
	std::cin.get();
	pthread_exit(NULL);
	return EXIT_SUCCESS;
}