#include <iostream>
#include <csignal>
#include "WebServer.h"
#include "FileUtils.h"
using namespace std;

WebServer* webServ = NULL;

void handleCtrlC(int s){
	if (webServ != NULL)
	{
		webServ->stop();
	}
}

int main(int argc, char** argv)
{
	signal(SIGINT,handleCtrlC);

	WebServer webserver(argv[1]);
	webServ = &webserver;
	webserver.run();

	return 0;
}
