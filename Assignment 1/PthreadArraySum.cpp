#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <chrono>

#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>

#define ARRAY_LENGTH 100000
#define NUM_THREADS 1

typedef std::chrono::high_resolution_clock Clock;
using namespace std;

int numbersToAdd[ARRAY_LENGTH];
int sumWithOnePthread[ARRAY_LENGTH/NUM_THREADS] = {0};
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

	cout << "-------------Pthread array sum example-------------"<< endl << endl;

	cout << "Generating array. " << endl;
	for (i = 0; i < ARRAY_LENGTH; i++) {
		numbersToAdd[i] = rand() % 100;
	}
	cout << endl;

	srand(time(NULL));
	t1 = tmr.elapsed();
	for (i = 0; i < ARRAY_LENGTH; i++) {
		arraySequentialSum = arraySequentialSum + numbersToAdd[i];
	}
	t2 = tmr.elapsed();

	cout << "Sequential sum = " << arraySequentialSum << endl;
	cout << "Sequential sum time: " << t2 - t1 << endl << endl;

	cout << "Starting pthreads. " << endl;

	t1 = tmr.elapsed();
	for (i = 0; i < NUM_THREADS; i++) {
		cout << "main() : creating thread, " << i << endl;
		rc = pthread_create(&threads[i], NULL, GetSumWithMultiplePthreads, (void *)NULL);

		if (rc) {
			cout << "Error:unable to create thread," << rc << endl;
			exit(-1);
		}
	}

	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}

	for (int i = 0; i < NUM_THREADS; i++) {
		totalPthreadSum += sumWithOnePthread[i];
	}
	cout << endl;
	cout << "Total sum with pthreads: " << totalPthreadSum << endl;

	t2 = tmr.elapsed();
	cout << "Pthread sum time: " << t2 - t1 << endl << endl;

	cout << "-----------------Program has ended-----------------" << endl;
	cout << "Press Enter to continue." << endl;
	cin.get();
	pthread_exit(NULL);
	return 0;
}