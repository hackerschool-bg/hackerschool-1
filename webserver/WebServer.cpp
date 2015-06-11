#include "WebServer.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <errno.h>
using namespace std;

void error(const char* msg) {
	perror(msg);
	exit(1);
}

void flagNonBlocking(int socket) {
	int flags;
	flags = fcntl(socket, F_GETFL, 0);
	if (flags == -1)
		error("Failed to get socket flags");
	if (fcntl(socket, F_SETFL, flags | O_NONBLOCK) == -1)
		error("Failed to set socket flags");
}

WebServer::WebServer(char* port)
{
	keepRunning = true;

	struct addrinfo hints, *result, *i;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int s = getaddrinfo(NULL, port, &hints, &result);
	if (s != 0)
	{
		cerr << "Failed to get interfaces: " <<  gai_strerror(s) << endl;
		exit(1);
	}

	for (i = result; i != NULL; i = i->ai_next)
	{
		socketFD = socket(i->ai_family, i->ai_socktype, i->ai_protocol);
		if (socketFD == -1)
			continue;

		s = bind(socketFD, i->ai_addr, i->ai_addrlen);
		if (s == 0)
			break;

		close(socketFD);
	}

	if (i == NULL)
	{
		cerr << "Could not find a suitable interface to bind" << endl;
		exit(1);
	}

	freeaddrinfo(result);

	flagNonBlocking(socketFD);

	if (listen(socketFD, SOMAXCONN) == -1)
		error("Failed to setup server socket for listening");

	epollFD = epoll_create1(0);
	if (epollFD == -1)
		error("Failed to create epoll file descriptor");

	event.data.fd = socketFD;
	event.events = EPOLLIN | EPOLLET;
	if (epoll_ctl(epollFD, EPOLL_CTL_ADD, socketFD, &event) == -1)
		error("Failed to add server socket to epoll list");

	shutdownFD = eventfd(0,EFD_NONBLOCK);
	event.data.fd = shutdownFD;
	event.events = EPOLLIN | EPOLLET;
	if (epoll_ctl(epollFD,EPOLL_CTL_ADD, shutdownFD, &event) == -1)
		error("Failed to add shutdown fd to epoll list");

	events = new epoll_event[MAX_EVENTS_PER_ITERATION];
}

void WebServer::run()
{
	while (keepRunning) {
		int numEvents, currentEventIndex;
		numEvents = epoll_wait(epollFD, events, MAX_EVENTS_PER_ITERATION, -1);
		for (currentEventIndex = 0; currentEventIndex < numEvents; currentEventIndex++)
		{
			if (events[currentEventIndex].data.fd == shutdownFD)
			{
				keepRunning = false;
				break;
			}
			else if ((events[currentEventIndex].events & EPOLLERR) ||
				(events[currentEventIndex].events & EPOLLHUP) ||
				(!(events[currentEventIndex].events & EPOLLIN)))
			{
				// socket's not ready
				close(events[currentEventIndex].data.fd);
				continue;
			}
			else if (events[currentEventIndex].data.fd == socketFD)
			{
				// new connection
				while (1)
				{
					struct sockaddr saddr;
					socklen_t in_len;
					int newFD;

					in_len = sizeof saddr;
					newFD = accept(socketFD, &saddr, &in_len);
					if (newFD == -1)
					{
						if (errno != EAGAIN && errno != EWOULDBLOCK)
							cerr << "Failed while accepting new connection" << endl;
						break;
					}

					flagNonBlocking(newFD);

					event.data.fd = newFD;
					event.events = EPOLLIN | EPOLLET;
					if (epoll_ctl(epollFD, EPOLL_CTL_ADD, newFD, &event) == -1)
						error("Failed while adding new socket to epoll list");
				}
				continue;
			}
			else
			{
				thread.enqueueFileDescriptor(events[currentEventIndex].data.fd);
			}
		}
	}
	cout << "Shutting down gracefully" << endl;
	delete [] events;
	close(shutdownFD);
	close(socketFD);
}

void WebServer::stop()
{
	thread.join();
	eventfd_write(shutdownFD,1);
}

