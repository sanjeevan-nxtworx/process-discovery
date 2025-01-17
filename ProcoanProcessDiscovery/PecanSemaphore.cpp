#include "stdafx.h"

#include "PecanSemaphore.h"

PecanSemaphore::PecanSemaphore()
{
	count_ = 0L;
}

PecanSemaphore::~PecanSemaphore()
{
}

void PecanSemaphore::notify() 
{
	lock_guard<decltype(mutex_)> lock(mutex_);
	++count_;
	condition_.notify_one();
}

void PecanSemaphore::wait() 
{
	std::unique_lock<decltype(mutex_)> lock(mutex_);
	while (!count_) // Handle spurious wake-ups.
		condition_.wait(lock);
	--count_;
}

bool PecanSemaphore::try_wait() 
{
	std::lock_guard<decltype(mutex_)> lock(mutex_);
	if (count_) {
		--count_;
		return true;
	}
	return false;
}
