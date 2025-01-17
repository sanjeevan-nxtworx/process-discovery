#pragma once

#include <mutex>
#include <condition_variable>
using namespace std;

class PecanSemaphore
{
private:
	mutex mutex_;
	condition_variable condition_;
	unsigned long count_; // Initialized as locked.

public:
	void notify();
	void wait();
	bool try_wait();
	PecanSemaphore();
	~PecanSemaphore();
};
