#ifndef WEBSERVER_H_
#define WEBSERVER_H_
#include <sys/epoll.h>
#include "ProcessingThread.h"

#define MAX_EVENTS_PER_ITERATION 1000

class WebServer
{
private:
	int socketFD;
	int epollFD;
	int shutdownFD;
	bool keepRunning;
	struct epoll_event event;
	struct epoll_event *events;

	ProcessingThread thread;

public:

	WebServer(char* port);

	void run();
	void stop();
};

#endif /* WEBSERVER_H_ */
