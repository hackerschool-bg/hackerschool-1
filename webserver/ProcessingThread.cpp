#include "ProcessingThread.h"
#include "HttpProtocol.h"
#include "FileUtils.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <unistd.h>
using namespace std;

void* processingThreadFunction(void* threadArg)
{
	((ProcessingThread*)threadArg)->run();
	pthread_exit(NULL);
}

ProcessingThread::ProcessingThread()
{
	continueRunning = true;
	pthread_mutex_init(&queueLock,NULL);
	pthread_cond_init(&waitCondition,NULL);

	pthread_create(&thread,NULL,processingThreadFunction,this);
}

void ProcessingThread::join()
{
	continueRunning = false;
	pthread_cond_signal(&waitCondition);
	pthread_join(thread,NULL);
}

void ProcessingThread::enqueueFileDescriptor(int fd)
{
	pthread_mutex_lock(&queueLock);
	fileDescriptorQueue.push(fd);
	if (fileDescriptorQueue.size() == 1)
		pthread_cond_signal(&waitCondition);
	pthread_mutex_unlock(&queueLock);
}

void ProcessingThread::run()
{
	while (continueRunning)
	{
		pthread_mutex_lock(&queueLock);
		if (fileDescriptorQueue.size() == 0)
			pthread_cond_wait(&waitCondition,&queueLock);
		if (fileDescriptorQueue.size() == 0)
			return;
		int fd = fileDescriptorQueue.front();
		fileDescriptorQueue.pop();
		pthread_mutex_unlock(&queueLock);
		processFileDescriptor(fd);
	}
}

unsigned int ProcessingThread::queueSize()
{
	return fileDescriptorQueue.size();
}

void ProcessingThread::processFileDescriptor(int fd)
{
	char buffer[1024*10]; // 10kb
	ssize_t count=1;

	while (count > 0)
	{
		count = read(fd, buffer, 1024*10);
		if (count < 1024*10)
			break;
	}

	HttpRequest req(buffer);

	if (!fileExists(req.getRequestURI().c_str()))
	{
		HttpResponse resp("HTTP/1.1 404 Not found");
		resp.addHeader("Content-Type","text/html; charset=utf-8");
		resp.addHeader("Content-Length","50");
		resp.setContent(((void*)"<html><body><h1>404 - Not found</h1></body></html>"),50);
		resp.writeToFD(fd);
	}
	else
	{
		if (isDir(req.getRequestURI().c_str()))
		{
			HttpResponse resp("HTTP/1.1 200 OK");
			resp.addHeader("Content-Type","text/html; charset=utf-8");
			unsigned length = 0;
			string body = "<html><body>";
			vector<string> files;
			dirLs(req.getRequestURI().c_str(),files);
			for (unsigned i=0;i<files.size();i++)
			{
				body += "<a href=\"/"+files[i]+"\">" + files[i] +"</a><br/>";
			}
			length = body.length();
			resp.setContent((void*)body.c_str(),length);
			char buff[10];
			sprintf(buff,"%d",length);
			resp.addHeader("Content-Length",buff);
			resp.writeToFD(fd);
		}
		else if (isFile(req.getRequestURI().c_str()))
		{
			ifstream file(req.getRequestURI().c_str());
			file.seekg(0,ios_base::end);
			streamsize fileSize = file.tellg();
			file.seekg(0,ios_base::beg);
			char* content = new char[fileSize];
			file.read(content,fileSize);
			file.close();
			char buff[10];
			sprintf(buff,"%d",fileSize);
			HttpResponse resp("HTTP/1.1 200 OK");
			resp.addHeader("Content-Length",buff);
			resp.setContent(content,fileSize);
			string fileExt = getExtension(req.getRequestURI());
			if (fileExt.compare("html") == 0)
			{
				resp.addHeader("Content-Type","text/html; charset=utf-8");
			}
			resp.writeToFD(fd);
		}
		HttpResponse resp("HTTP/1.1 200 OK");
		resp.addHeader("Content-Type","text/html; charset=utf-8");
		resp.addHeader("Content-Length","32");
		resp.setContent(((void*)"<html><body>foobar</h1></body></html>"),32);
		resp.writeToFD(fd);
	}

	//write(fd, "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: 32\r\n\r\n<html><body>foobar</body></html>",119);

	close(fd);
}
