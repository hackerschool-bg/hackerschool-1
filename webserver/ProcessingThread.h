#ifndef PROCESSINGTHREAD_H_
#define PROCESSINGTHREAD_H_

#include <queue>
#include <pthread.h>

class ProcessingThread
{
private:
	std::queue<int> fileDescriptorQueue;
	pthread_mutex_t queueLock;
	pthread_cond_t waitCondition;
	bool continueRunning;

	pthread_t thread;

	void processFileDescriptor(int fd);

public:
	ProcessingThread();

	void run();
	void enqueueFileDescriptor(int fd);
	void join();
	unsigned int queueSize();
};


#endif /* PROCESSINGTHREAD_H_ */
