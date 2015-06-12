#ifndef PROCESSINGTHREAD_H_
#define PROCESSINGTHREAD_H_

#include <queue>
#include <pthread.h>
#include <lua.hpp>
#include "KeepAliveKiller.h"

class ProcessingThread
{
private:
	KeepAliveKiller* karThread;
	std::queue<int> fileDescriptorQueue;
	pthread_mutex_t queueLock;
	pthread_cond_t waitCondition;
	bool continueRunning;

	pthread_t thread;

	lua_State* luaState;

public:
	void processFileDescriptor(int fd);

	ProcessingThread();

	void start();
	void setKeepAliveThread(KeepAliveKiller* kar);

	void run();
	void enqueueFileDescriptor(int fd);
	void join();
	unsigned int queueSize();
};


#endif /* PROCESSINGTHREAD_H_ */
