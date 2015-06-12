#include "KeepAliveKiller.h"
#include <ctime>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
using namespace std;

void* keepAliveThreadFunction(void* threadArg)
{
	((KeepAliveKiller*)threadArg)->run();
	pthread_exit(NULL);
}

KeepAliveKiller::KeepAliveKiller()
{
	continueRunning = true;
	pthread_mutex_init(&listLock,NULL);
	pthread_mutex_init(&sleepMutex,NULL);
	pthread_cond_init(&sleepCond,NULL);

	pthread_mutex_lock(&sleepMutex);

	//pthread_create(&thread,NULL,keepAliveThreadFunction,this);
}

KeepAliveKiller::~KeepAliveKiller()
{
	for (list<KeepAliveRecord*>::iterator i = records.begin();i!=records.end();++i)
	{
		delete (*i);
	}
}

void KeepAliveKiller::run()
{
	while (continueRunning)
	{
		timespec waitTarget;
		timeval now;
		gettimeofday(&now,NULL);
		waitTarget.tv_sec = now.tv_sec+1;
		waitTarget.tv_nsec = now.tv_usec*1000+1000000;
		pthread_cond_timedwait(&sleepCond,&sleepMutex,&waitTarget);
		if (!continueRunning) return;
		time_t current = time(NULL);
		pthread_mutex_lock(&listLock);
		list<KeepAliveRecord*>::iterator i = records.begin();
		while (i != records.end())
		{
			KeepAliveRecord* kar = (*i);
			if (kar->killTime <= current)
			{
				close(kar->fd);
				delete kar;
				records.erase(i++);
			}
			else
				++i;
		}
		pthread_mutex_unlock(&listLock);
	}
}

void KeepAliveKiller::join()
{
	continueRunning = false;
	pthread_cond_signal(&sleepCond);
	pthread_join(thread,NULL);
}

void KeepAliveKiller::schedule(int fd, int seconds)
{
	KeepAliveRecord* kar = new KeepAliveRecord;
	kar->fd = fd;
	kar->killTime = time(NULL)+seconds;
	pthread_mutex_lock(&listLock);
	records.push_back(kar);
	pthread_mutex_unlock(&listLock);
}

void KeepAliveKiller::update(int fd, int seconds)
{
	pthread_mutex_lock(&listLock);
	for (list<KeepAliveRecord*>::iterator i = records.begin(); i != records.end(); ++i)
	{
		if ((*i)->fd == fd)
		{
			(*i)->killTime = time(NULL)+seconds;
			pthread_mutex_unlock(&listLock);
			return;
		}
	}
	KeepAliveRecord* kar = new KeepAliveRecord;
	kar->fd = fd;
	kar->killTime = time(NULL)+seconds;
	records.push_back(kar);
	pthread_mutex_unlock(&listLock);
}
