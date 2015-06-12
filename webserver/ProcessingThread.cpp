#include "ProcessingThread.h"
#include "HttpProtocol.h"
#include "FileUtils.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sstream>
using namespace std;

void* processingThreadFunction(void* threadArg)
{
	((ProcessingThread*)threadArg)->run();
	pthread_exit(NULL);
}

int luaStdOutPrint(lua_State* luaState)
{
	stringstream* ssp = (stringstream*) lua_touserdata(luaState,lua_upvalueindex(1));
	const char* s = luaL_checkstring(luaState,1);
	ssp->write(s,strlen(s));
	return 0;
}

ProcessingThread::ProcessingThread()
{
	thread = 0;
	karThread = NULL;
	continueRunning = true;
	pthread_mutex_init(&queueLock,NULL);
	pthread_cond_init(&waitCondition,NULL);

	luaState = luaL_newstate();
	luaL_openlibs(luaState);
	lua_pushcfunction(luaState,luaStdOutPrint);
	lua_setglobal(luaState,"print");
}

void ProcessingThread::start()
{
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
	bool wasEmpty = fileDescriptorQueue.empty();
	fileDescriptorQueue.push(fd);
	pthread_mutex_unlock(&queueLock);
	if (wasEmpty)
		pthread_cond_signal(&waitCondition);
}

void ProcessingThread::run()
{
	while (continueRunning)
	{
		pthread_mutex_lock(&queueLock);
		if (fileDescriptorQueue.size() == 0)
			pthread_cond_wait(&waitCondition,&queueLock);
		if (!continueRunning)
			return;
		if (fileDescriptorQueue.size() == 0)
			continue;
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

void ProcessingThread::setKeepAliveThread(KeepAliveKiller* kar)
{
	this->karThread = kar;
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
	HttpResponse resp;
	/*if (req.isKeepAlive())
	{
		resp.addHeader("Connection","Keep-Alive");
	}*/

	if (req.getRequestURI().compare("/sum") == 0)
	{
		int a=0,b=0;
		const vector<QueryParameter*>& params = req.getParams();
		for (unsigned i=0;i<params.size();i++)
		{
			if (params[i]->getParam().compare("a") == 0)
				a = atoi(params[i]->getValue().c_str());
			if (params[i]->getParam().compare("b") == 0)
				b = atoi(params[i]->getValue().c_str());
		}
		char buffer[128];
		sprintf(buffer,"<html><body>%d + %d = %d</body></html>",a,b,a+b);
		char len[10];
		sprintf(len,"%lu",strlen(buffer));
		HttpResponse resp("HTTP/1.1 200 OK");
		resp.addHeader("Content-Type","text/html; charset=utf-8");
		resp.addHeader("Content-Length",len);
		resp.setContent(((void*)buffer),strlen(buffer));
		resp.writeToFD(fd);
	}
	else if (!fileExists(req.getRequestURI().c_str()))
	{
		resp.setStatusLine("HTTP/1.1 404 Not found");
		resp.addHeader("Content-Type","text/html; charset=utf-8");
		resp.addHeader("Content-Length","50");
		resp.setContent(((void*)"<html><body><h1>404 - Not found</h1></body></html>"),50);
		resp.writeToFD(fd);
	}
	else
	{
		if (isDir(req.getRequestURI().c_str()))
		{
			resp.setStatusLine("HTTP/1.1 200 OK");
			resp.addHeader("Content-Type","text/html; charset=utf-8");
			unsigned length = 0;
			string body = "<html><body>";
			vector<string> files;
			dirLs(req.getRequestURI().c_str(),files);
			for (unsigned i=0;i<files.size();i++)
			{
				body += "<a href=\""+req.getRequestURI()+"/"+files[i]+"\">" + files[i] +"</a><br/>";
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
			string fileExt = getExtension(req.getRequestURI());
			if (fileExt != "lua")
			{
				ifstream file(req.getRequestURI().c_str());
				file.seekg(0,ios_base::end);
				streamsize fileSize = file.tellg();
				file.seekg(0,ios_base::beg);
				char* content = new char[fileSize];
				file.read(content,fileSize);
				file.close();
				char buff[10];
				sprintf(buff,"%ld",fileSize);
				resp.setStatusLine("HTTP/1.1 200 OK");
				resp.addHeader("Content-Length",buff);
				resp.setContent(content,fileSize);
				if (fileExt.compare("html") == 0)
				{
					resp.addHeader("Content-Type","text/html; charset=utf-8");
				}
				resp.writeToFD(fd);
				delete [] content;
			}
			else
			{
				int result = luaL_loadfile(luaState,req.getRequestURI().c_str());
				if (result != 0)
				{
					resp.setStatusLine("HTTP/1.1 500 Internal server error");
					resp.addHeader("Content-Type","text/html; charset=utf-8");
					char buffer[1024];
					sprintf(buffer,"<html><body><h1>500 Internal server error</h1><br/><p>%s</p></body></html>",lua_tostring(luaState,1));
					lua_pop(luaState,1); // pop the error message
					char lenBuff[10];
					sprintf(lenBuff,"%lu",strlen(buffer));
					resp.addHeader("Content-Length",lenBuff);
					resp.setContent(((void*)"<html><body><h1>404 - Not found</h1></body></html>"),strlen(buffer));
					resp.writeToFD(fd);
				}
				else
				{
					// environment table - replaced with globals, setting up an environment that includes all the built-in functions is a pain
					//lua_newtable(luaState);
					// print function
					stringstream ss;
					//lua_pushstring(luaState,"print");
					lua_pushlightuserdata(luaState,&ss);
					lua_pushcclosure(luaState,luaStdOutPrint,1);
					lua_setglobal(luaState,"print");
					//lua_settable(luaState,-3);
					// headers table
					//lua_pushstring(luaState,"HTTP_HEADERS");
					lua_newtable(luaState);
					const vector<HttpHeader*>& headers = req.getHeaders();
					for (unsigned i=0;i<headers.size();i++)
					{
						lua_pushstring(luaState,headers[i]->getField().c_str());
						lua_pushstring(luaState,headers[i]->getValue().c_str());
						lua_settable(luaState,-3);
					}
					lua_setglobal(luaState,"HTTP_HEADERS");
					//lua_settable(luaState,-3);
					// parameters table
					//lua_pushstring(luaState,"HTTP_QUERY_PARAMS");
					lua_newtable(luaState);
					const vector<QueryParameter*>& params = req.getParams();
					for (unsigned i=0;i<params.size();i++)
					{
						lua_pushstring(luaState,params[i]->getParam().c_str());
						lua_pushstring(luaState,params[i]->getValue().c_str());
						lua_settable(luaState,-3);
					}
					lua_setglobal(luaState,"HTTP_QUERY_PARAMS");
					//lua_settable(luaState,-3);
					// set the environment
					//lua_setupvalue(luaState, -2, 1);

					result = lua_pcall(luaState,0,LUA_MULTRET,0);
					if (result == 0)
					{
						resp.setStatusLine("HTTP/1.1 200 OK");
						resp.addHeader("Content-Type","text/html; charset=utf-8");
						const string& output = ss.str();
						char lenBuff[10];
						sprintf(lenBuff,"%lu",output.length());
						resp.addHeader("Content-Length",lenBuff);
						resp.setContent(((void*)output.c_str()),output.length());
						resp.writeToFD(fd);
					}
					else if (result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR)
					{
						resp.setStatusLine("HTTP/1.1 500 Internal server error");
						resp.addHeader("Content-Type","text/html; charset=utf-8");
						char buffer[1024];
						sprintf(buffer,"<html><body><h1>500 Internal server error</h1><br/><p>%s</p></body></html>",lua_tostring(luaState,1));
						lua_pop(luaState,1); // pop the error message
						char lenBuff[10];
						sprintf(lenBuff,"%lu",strlen(buffer));
						resp.addHeader("Content-Length",lenBuff);
						resp.setContent(((void*)buffer),strlen(buffer));
						resp.writeToFD(fd);
					}
				}
			}
		}
	}

	//if (!req.isKeepAlive())
		close(fd);
	/*else
		karThread->update(fd,15);*/
}
