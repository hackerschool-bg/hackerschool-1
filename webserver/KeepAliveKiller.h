#ifndef KEEPALIVEKILLER_H_
#define KEEPALIVEKILLER_H_

#include <list>
#include <ctime>
#include <pthread.h>
#include <lua.hpp>

class KeepAliveKiller
{
private:
	class KeepAliveRecord
	{
	public:
		int fd;
		time_t killTime;
	};

	std::list<KeepAliveRecord*> records;
	pthread_mutex_t listLock;
	pthread_cond_t sleepCond;
	pthread_mutex_t sleepMutex;
	bool continueRunning;

	pthread_t thread;

public:
	KeepAliveKiller();
	~KeepAliveKiller();

	void run();
	void schedule(int fd, int seconds);
	void update(int fd, int seconds);
	void join();
};


#endif /* KEEPALIVEKILLER_H_ */
